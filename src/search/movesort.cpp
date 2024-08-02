#include "movesort.h"

#include "search.h"
#include "../include/board.h"
#include "../include/inline_functions.h"

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

// To implement En-Passant captures neatly we duplicate the values
// such that each side can capture their own pieces, this of course
// will not be allowed by the movegenerator
// eg. mvv_lva[1][1]: means white pawn captures white pawn
static int mvv_lva[12][12] = {
	{105, 205, 305, 405, 505, 605, 105, 205, 305, 405, 505, 605},
	{104, 204, 304, 404, 504, 604, 104, 204, 304, 404, 504, 604},
	{103, 203, 303, 403, 503, 603, 103, 203, 303, 403, 503, 603},
	{102, 202, 302, 402, 502, 602, 102, 202, 302, 402, 502, 602},
	{101, 201, 301, 401, 501, 601, 101, 201, 301, 401, 501, 601},
	{100, 200, 300, 400, 500, 600, 100, 200, 300, 400, 500, 600},

	{105, 205, 305, 405, 505, 605, 105, 205, 305, 405, 505, 605},
	{104, 204, 304, 404, 504, 604, 104, 204, 304, 404, 504, 604},
	{103, 203, 303, 403, 503, 603, 103, 203, 303, 403, 503, 603},
	{102, 202, 302, 402, 502, 602, 102, 202, 302, 402, 502, 602},
	{101, 201, 301, 401, 501, 601, 101, 201, 301, 401, 501, 601},
	{100, 200, 300, 400, 500, 600, 100, 200, 300, 400, 500, 600}
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

	if (move.isCapture()) {

		if (move.isEnPassant()) return 105 + captureBonus;
		// score moves by MVV-LVA, it doesnt know if pieces are protected (SEE does though)
		return mvv_lva[ board.mailbox[move.from()] ][ board.mailbox[move.to()] ] + captureBonus;
	}

	// scoring promotions, should check this....
	const int promPiece { move.promotionPiece() };
	if (promPiece != 0) return mvv_lva[PAWN][promPiece] + captureBonus;

	if (killerMoves[0][searchPly] == move) return firstKiller;
	if (killerMoves[1][searchPly] == move) return secondKiller;

	return historyScores[ board.mailbox[move.from()] ][ move.to() ];
}


void giveScores(MoveList& moveList, const Move bestMove) {
	for (int count = 0; count < moveList.count; ++count) {
		const Move move{ moveList.moves[count].first };

		// assigning the scores
		if (bestMove == move) moveList.moves[count].second = hashTableBonus;
		else moveList.moves[count].second = scoreMove(move);
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
