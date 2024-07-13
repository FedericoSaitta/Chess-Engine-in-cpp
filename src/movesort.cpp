#include "movesort.h"

#include "search.h"
#include "board.h"

#include <vector>
#include <algorithm>

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

constexpr int hashTableBonus{ 30'000 }; // so if we have a hashed move we search it first
constexpr int principalVariationBonus{ 20'000 }; // so PV is always above captures
constexpr int captureBonus{ 10'000 }; // so captures are always above killers
constexpr int firstKiller{ 9'000 };
constexpr int secondKiller{ 8'000 };

int scoreMove(const int move, const int ply) {

	if (scorePV && (pvTable[0][ply] == move)) {
		scorePV = 0; // as there is only one principal move in a moveList, so we disable further scoring
		// std::cout << "Current PV " << algebraicNotation(move) << " at ply" << ply << '\n';
		// note that because of null move and late more pruning this will not print nicely because higher depths plies are
		// not considered because of the early pruning, PV is still followed  though.
		return principalVariationBonus;
	}

	const int movePiece = getMovePiece(move);
	const int targetSquare = getMoveTargetSQ(move);

	if (getMoveCapture(move)) {
		int targetPiece{ Pawn }; // in case we make an enPassant capture, which our loop would miss

		// copied from makeMove function
		int startPiece, endPiece;
		if (side == White ) { startPiece = Pawn + 6; endPiece = King + 6; }
		else { startPiece = Pawn; endPiece = King; }

		for (int bbPiece=startPiece; bbPiece <= endPiece; bbPiece++) {
			if ( GET_BIT(bitboards[bbPiece], targetSquare) ) {
				targetPiece = bbPiece;
				break;
			}
		}

		// score moves by MVV-LVA, it doesnt know if pieces are protected (SEE does though)
		return mvv_lva[movePiece][targetPiece] + captureBonus;
	}

	// scoring promotions, should check this....
	const int promPiece { getMovePromPiece(move) };
	if (promPiece != 0) return mvv_lva[Pawn][promPiece] + captureBonus;

	if (killerMoves[0][ply] == move) return firstKiller;
	if (killerMoves[1][ply] == move) return secondKiller;

	return historyMoves[movePiece][targetSquare];
}

void sortMoves(MoveList& moveList, const int ply, const int bestMove) {
	// Pair moves with their scores
	std::vector<std::pair<int, int>> scoredMoves(moveList.count);

	for (int count = 0; count < moveList.count; ++count) {
		int score;

		if (bestMove == moveList.moves[count]) score = hashTableBonus;

		else score = scoreMove(moveList.moves[count], ply);

		scoredMoves[count] = std::make_pair(score, moveList.moves[count]);
	}

	// Sort moves based on their scores in descending order
	std::ranges::sort(scoredMoves, [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
		return a.first > b.first; // Descending order
	});

	// Update moveList with sorted moves
	for (int i = 0; i < moveList.count; ++i) {
		moveList.moves[i] = scoredMoves[i].second;
	}
}
