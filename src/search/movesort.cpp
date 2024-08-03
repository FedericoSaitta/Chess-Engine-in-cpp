#include "movesort.h"

#include "search.h"
#include "../include/board.h"
#include "../include/inline_functions.h"
#include "config.h"

#include <algorithm>
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
constexpr int hashTableBonus{ 30'000 };
constexpr int principalVariationBonus{ 20'000 }; // so PV is always above captures
constexpr int captureBonus{ 10'000 }; // so captures are always above killers
constexpr int firstKiller{ 9'000 };
constexpr int secondKiller{ 8'000 };

int scoreMove(const Move move) {

	if (scorePV && (pvTable[0][searchPly] == move)) {
		scorePV = 0; // as there is only one principal move in a moveList, so we disable further scoring
		// std::cout << "Current PV " << algebraicNotation(move) << " at ply" << ply << '\n';
		// note that because of null move and late more pruning this will not print nicely because higher depths plies are
		// not considered because of the early pruning, PV is still followed  though.
		return principalVariationBonus;
	}

	if (move.isNoisy() ) {
		// queen promotions get maximum bonus
		if (move.isPromotion()) return 605 + captureBonus;

		// score moves by MVV-LVA, it doesnt know if pieces are protected
		return mvv_lva[ board.mailbox[move.from()] ][ board.mailbox[move.to()] ] + captureBonus;
	}

	if (killerMoves[0][searchPly] == move) return firstKiller;
	if (killerMoves[1][searchPly] == move) return secondKiller;

	return (historyScores[move.from()][move.to() ]);
}


void giveScores(MoveList& moveList, const Move bestMove) {
	for (int count = 0; count < moveList.count; ++count) {
		const Move move{ moveList.moves[count].first };

		if (bestMove != move)
			moveList.moves[count].second = scoreMove(move);
		else
			moveList.moves[count].second = hashTableBonus;
	}
}


Move pickBestMove(MoveList& moveList, const int start) {

    int bestMoveScore{ moveList.moves[start].second };
    int bestMoveIndex{ start };

	for (int index = start; index < moveList.count; ++index) {
		if (moveList.moves[index].second > bestMoveScore) {
			bestMoveScore = moveList.moves[index].second;
			bestMoveIndex = index;
		}
	}
    std::swap(moveList.moves[start], moveList.moves[bestMoveIndex]);

	return moveList.moves[start].first;
}
