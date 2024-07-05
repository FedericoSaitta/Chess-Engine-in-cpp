//
// Created by Federico Saitta on 05/07/2024.
//
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

#include "globals.h"
#include "macros.h"
#include "inline_functions.h"
#include "misc.h"
#include "tests.h"


/*

	(Victims) Pawn Knight Bishop   Rook  Queen   King
  (Attackers)
		Pawn   105    205    305    405    505    605
	  Knight   104    204    304    404    504    604
	  Bishop   103    203    303    403    503    603
		Rook   102    202    302    402    502    602
	   Queen   101    201    301    401    501    601
		King   100    200    300    400    500    600

*/

static int mvv_lva[12][12] = {
 	105, 205, 305, 405, 505, 605,  105, 205, 305, 405, 505, 605,
	104, 204, 304, 404, 504, 604,  104, 204, 304, 404, 504, 604,
	103, 203, 303, 403, 503, 603,  103, 203, 303, 403, 503, 603,
	102, 202, 302, 402, 502, 602,  102, 202, 302, 402, 502, 602,
	101, 201, 301, 401, 501, 601,  101, 201, 301, 401, 501, 601,
	100, 200, 300, 400, 500, 600,  100, 200, 300, 400, 500, 600,

	105, 205, 305, 405, 505, 605,  105, 205, 305, 405, 505, 605,
	104, 204, 304, 404, 504, 604,  104, 204, 304, 404, 504, 604,
	103, 203, 303, 403, 503, 603,  103, 203, 303, 403, 503, 603,
	102, 202, 302, 402, 502, 602,  102, 202, 302, 402, 502, 602,
	101, 201, 301, 401, 501, 601,  101, 201, 301, 401, 501, 601,
	100, 200, 300, 400, 500, 600,  100, 200, 300, 400, 500, 600
};

// we will add different scorings for PV etc
int scoreMove(const int move, const int ply) {

	if (getMoveCapture(move)) {

		int targetPiece{ Pawn }; // in case we make an enPassant capture, which our loop would miss

		// copied from makeMove function
		int startPiece, endPiece;
		if (side == White ) { startPiece = Pawn + 6; endPiece = King + 6; }
		else { startPiece = Pawn; endPiece = King; }

		for (int bbPiece=startPiece; bbPiece <= endPiece; bbPiece++) {
			if ( getBit(bitboards[bbPiece], getMoveTargetSQ(move)) ) {
				targetPiece = bbPiece;
				break;
			}
		}

		// score moves by MVV-LVA, it doesnt know if pieces are protected (SEE does though)
		return mvv_lva[getMovePiece(move)][targetPiece];
	}
	if (killerMoves[0][ply] == move) return 9000;

	if (killerMoves[1][ply] == move) return 8000;

	return historyMoves[getMovePiece(move)][getMoveTargetSQ(move)];
}

void sortMoves(MoveList& moveList, const int ply) {
	int moveScores[moveList.count];

	// scoring the moves
	for (int count=0; count < moveList.count; count++) {
		moveScores[count] = scoreMove(moveList.moves[count], ply);
	}

	// sorting the moves
	for (int current=0; current < moveList.count; current++) {
		for (int next=(current + 1); next < moveList.count; next++) {
			// comparing current and next
			if (moveScores[current] < moveScores[next]) {
				const int tempScore{ moveScores[current] };
				const int tempMove{ moveList.moves[current] };

				moveScores[current] = moveScores[next];
				moveList.moves[current] = moveList.moves[next];

				moveScores[next] = tempScore;
				moveList.moves[next] = tempMove;
			}
		}
	}
}