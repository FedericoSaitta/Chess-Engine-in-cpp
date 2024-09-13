#include "search.h"
#include "../../chess/board.h"
#include "../../chess/inline_functions.h"
#include "config.h"
#include "see.h"

#include "../../chess/misc.h"
#include <algorithm>
#include <assert.h>
#include <utility>

/*  MOVE SORTING ORDER
	1. PV Move
	2. Good Captures and all capture promotions
	3. Killer 1
	4. Killer 2
	5. History
	6. Bad Captures

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
constexpr int MAX_HISTORY_SCORE{ 16'384 };


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
constexpr int queenPromotionBonus{ 1'500'000 }; // so PV is always above captures
constexpr int underPromotionMalus{ -1'500'000 }; // so PV is always above captures
constexpr int captureBonus{ 1'000'000 }; // so captures are always above killers
constexpr int firstKiller{ 900'000 };
constexpr int secondKiller{ 800'000 };

constexpr int seeConversion[] {-1, 1};

int Searcher::scoreMove(const Move move, const Board& board) {

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
		// back to old code for now
		// adding one to offset captures from non-captures
		if (move.isPromotion()) {
			return mvv_lva[ PAWN ][ move.promotionPiece() ] + captureBonus;
		}

		// score moves by MVV-LVA, it doesnt know if pieces are protected
		assert( (move.isEnPassant() ? board.mailbox[move.to()] == NO_PIECE : board.mailbox[move.to()] != NO_PIECE)
				&& "scoreMove: enpassant or capture move are capturing wrong piece type");
		assert((board.mailbox[move.from()] != NO_PIECE) && "Starting piece is empty");

		return mvv_lva[ board.mailbox[move.from()] ][ board.mailbox[move.to()] ] + captureBonus * seeConversion[ see(move, 0, board) ];
	}

	if (killerMoves[0][searchPly] == move) return firstKiller;
	if (killerMoves[1][searchPly] == move) return secondKiller;

	return (historyScores[move.from()][move.to()]);
}


void Searcher::giveScores(MoveList& moveList, const Move bestMove, const Board& board) {
	for (int count = 0; count < moveList.count; count++) {
		const Move move{ moveList.moves[count].first };

		assert(!move.isNone() && "givesScores: move is None");
		assert((scoreMove(move, board) <= principalVariationBonus) && "giveScores: score is too large");

		if (bestMove == move) moveList.moves[count].second = hashTableBonus;
		else moveList.moves[count].second = scoreMove(move, board);
	}
}

std::pair<Move, int> Searcher::pickBestMove(MoveList& moveList, const int start) {
	assert(start < moveList.count && "pickBestMove: out of bounds start index");

    int bestMoveScore{ moveList.moves[start].second };
    int bestMoveIndex{ start };

	for (int index = start + 1; index < moveList.count; ++index) {
		const int score = moveList.moves[index].second;

		if (score > bestMoveScore) {
			bestMoveScore = score;
			bestMoveIndex = index;
		}
	}

	// Only swap if necessary to avoid unnecessary memory operations
	if (bestMoveIndex != start) {
		std::swap(moveList.moves[start], moveList.moves[bestMoveIndex]);
	}

	assert(!moveList.moves[start].first.isNone() && "pickBestMove: returned move is none");

	return moveList.moves[start];
}

void Searcher::updateKillers(const Move bestMove) {
	// update killer moves if we found a new unique bestMove
	assert(!bestMove.isPromotion() && "updateKillers: Killer move is promotion");

	if (killerMoves[0][searchPly] != bestMove) {
		assert(!bestMove.isNone() && "updateKillers: bestMove is empty");

		killerMoves[1][searchPly] = killerMoves[0][searchPly];
		killerMoves[0][searchPly] = bestMove;

		assert((killerMoves[1][searchPly] != killerMoves[0][searchPly]) && "updateKillers: moves are identical");
	}
}
void Searcher::updateHistory(const Move bestMove, const int depth, const Move* quiets, const int quietMoveCount) {
	const int bonus = std::min(2100, 300 * depth - 300);

	assert(!bestMove.isNone() && "updateHistory: bestMove is none");

	// Bonus to the move that caused the beta cutoff
	if (depth > 2) {
		historyScores[bestMove.from()][bestMove.to()] += bonus - historyScores[bestMove.from()][bestMove.to()] * std::abs(bonus) / MAX_HISTORY_SCORE;

		assert( std::abs(historyScores[bestMove.from()][bestMove.to()]) <= MAX_HISTORY_SCORE && "updateHistory: history bonus is too large");
	}

	// Penalize quiet moves that failed to produce a cut only if bestMove is also quiet
	assert((quietMoveCount <= 32) && "updateHistory: quietMoveCount is too large");
	for (int i = 0; i < quietMoveCount; i++) {
		Move m = quiets[i];
		assert(m != bestMove && "updateHistory: in loop, malus to bestMove");
		assert(!m.isNone() && "updatedHistory: in loop, move is noen");

		historyScores[m.from()][m.to()] += -bonus - historyScores[m.from()][m.to()] * std::abs(bonus) / MAX_HISTORY_SCORE;
		assert( std::abs(historyScores[m.from()][m.to()]) <= MAX_HISTORY_SCORE && "updateHistory: history bonus is too large");
	}

}

