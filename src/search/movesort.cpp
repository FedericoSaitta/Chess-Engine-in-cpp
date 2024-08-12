#include "movesort.h"

#include "search.h"
#include "../include/board.h"
#include "../include/inline_functions.h"
#include "searchparams.h"
#include "config.h"

#include "misc.h"
#include <algorithm>
#include <assert.h>
#include <utility>

/*  MOVE SORTING ORDER
	1. PV Move
	2. Captures and Promotions
	3. Killer 1
	4. Killer 2
	5. History
	6. Other moves

	In the case we run into a position that has already been store in the Transposition Table, we search the
	stored move first.

	Thanks to Maksim Korzh for the implementation of
	MVV-LVA (Most Valuable Victim - Least Valuable Attacker)

	 (Victims) Pawn Knight Bishop   Rook  Queen   King
  (Attackers)
		Pawn   105    205    305    405    505    605
	  Knight   104    204    304    404    504    604
	  Bishop   103    203    303    403    503    603
		Rook   102    202    302    402    502    602
	   Queen   101    201    301    401    501    601
		King   100    200    300    400    500    600
*/

// The table contains combinations of pieces will never capture each other
// though this is kept for perfomance. Notably the last entry of each row
// is of value 105 and this is to avoid checking whether the move is
// an en-passant move. En-passant moves appear to 'capture' empty squares
// hence index 12 (NO_PIECE = 12) is reserved for en-passants
static int mvv_lva[12][13] = {
	{105, 205, 305, 405, 505, 605, 105, 205, 305, 405, 505, 605, 105},
	{104, 204, 304, 404, 504, 604, 104, 204, 304, 404, 504, 604, 105},
	{103, 203, 303, 403, 503, 603, 103, 203, 303, 403, 503, 603, 105},
	{102, 202, 302, 402, 502, 602, 102, 202, 302, 402, 502, 602, 105},
	{101, 201, 301, 401, 501, 601, 101, 201, 301, 401, 501, 601, 105},
	{100, 200, 300, 400, 500, 600, 100, 200, 300, 400, 500, 600, 105},

	{105, 205, 305, 405, 505, 605, 105, 205, 305, 405, 505, 605, 105},
	{104, 204, 304, 404, 504, 604, 104, 204, 304, 404, 504, 604, 105},
	{103, 203, 303, 403, 503, 603, 103, 203, 303, 403, 503, 603, 105},
	{102, 202, 302, 402, 502, 602, 102, 202, 302, 402, 502, 602, 105},
	{101, 201, 301, 401, 501, 601, 101, 201, 301, 401, 501, 601, 105},
	{100, 200, 300, 400, 500, 600, 100, 200, 300, 400, 500, 600, 105},
};
constexpr int hashTableBonus{ 3'000'000 };
constexpr int principalVariationBonus{ 2'000'000 }; // so PV is always above captures
constexpr int captureBonus{ 1'000'000 }; // so captures are always above killers
constexpr int firstKiller{ 900'000 };
constexpr int secondKiller{ 800'000 };

int scoreMove(const Move move) {

	assert(searchPly < MAX_PLY && "scoreMove: out of bounds table indexing");
	assert(move.from() < 64 && "scoreMove: out of bounds table indexing");
	assert(move.to() < 64 && "scoreMove: out of bounds table indexing");

	if (scorePV && (pvTable[0][searchPly] == move)) {
		assert((scorePV == 1) && "scoreMove: scorePV is unitialized");
		assert(!pvTable[0][searchPly].isNone() && "scoreMove: pv move is None");
		assert(!move.isNone() && "scoreMove: current move is None");

		scorePV = 0;

		return principalVariationBonus;
	}

	if (move.isCapture()) {
		// promotion captures
		if (move.isPromotion()) return mvv_lva[ PAWN ][ move.promotionPiece() ] + captureBonus;

		// score moves by MVV-LVA, it doesnt know if pieces are protected
		assert( (move.isEnPassant() ? board.mailbox[move.to()] == NO_PIECE : board.mailbox[move.to()] != NO_PIECE)
				&& "scoreMove: enpassant or capture move are capturing wrong piece type");
		assert((board.mailbox[move.from()] != NO_PIECE) && "Starting piece is empty");

		return mvv_lva[ board.mailbox[move.from()] ][ board.mailbox[move.to()] ] + captureBonus;
	}

	if (killerMoves[0][searchPly] == move) return firstKiller;
	if (killerMoves[1][searchPly] == move) return secondKiller;

	return (historyScores[move.from()][move.to()]);
}


void giveScores(MoveList& moveList, const Move bestMove) {
	for (int count = 0; count < moveList.count; count++) {
		const Move move{ moveList.moves[count].first };

		assert(!move.isNone() && "givesScores: move is None");
		assert((scoreMove(move) <= principalVariationBonus) && "giveScores: score is too large");
		assert((scoreMove(move) >= -MAX_HISTORY_SCORE) && "giveScores: score is too small");

		if (bestMove == move) moveList.moves[count].second = hashTableBonus;
		else moveList.moves[count].second = scoreMove(move);
	}
}

Move pickBestMove(MoveList& moveList, const int start) {
	assert(start < moveList.count && "pickBestMove: out of bounds start index");

    int bestMoveScore{ moveList.moves[start].second };
    int bestMoveIndex{ start };

	for (int index = start; index < moveList.count; ++index) {
		if (moveList.moves[index].second > bestMoveScore) {
			bestMoveScore = moveList.moves[index].second;
			bestMoveIndex = index;
		}
	}

	// swap position of the current index with the best move
	const std::pair<Move, int> tempMove = moveList.moves[start];
	moveList.moves[start] = moveList.moves[bestMoveIndex];
	moveList.moves[bestMoveIndex] = tempMove;

	assert(!moveList.moves[start].first.isNone() && "pickBestMove: returned move is none");

	return moveList.moves[start].first;
}
