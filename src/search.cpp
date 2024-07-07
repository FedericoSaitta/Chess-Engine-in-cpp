//
// Created by Federico Saitta on 04/07/2024.
//
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "globals.h"
#include "macros.h"
#include "inline_functions.h"
#include "evaluation.h"
#include "misc.h"


static int ply{};
constexpr int maxPly{64};
static std::uint32_t nodes{};

static int killerMoves[2][128]{}; // zero initialization to ensure no random bonuses to moves
static int historyMoves[12][64]{}; // zero initialization to ensure no random bonuses to moves

static int pvLength[64]{};
static int pvTable[64][64]{};

static int followPV{}; // if it is true then we follow the principal variation
static int scorePV{};

///// MOVES ARE SORTED IN THIS ORDER ////


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


// we will add different scorings for PV etc
int scoreMove(const int move, const int ply) {

	if (scorePV) {
		if (pvTable[0][ply] == move) {
			scorePV = 0; // as there is only one principal move in a moveList
	// for debugging		std::cout << "Current PV " << algebraicNotation(move) << " at ply" << ply << '\n';
			return 20'000;
		}
	}

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
		return mvv_lva[getMovePiece(move)][targetPiece] + 10'000; // important as we score MVV-LVA as better than killer
	}
	if (killerMoves[0][ply] == move) return 9000;

	if (killerMoves[1][ply] == move) return 8000;

	return historyMoves[getMovePiece(move)][getMoveTargetSQ(move)];
}

void sortMoves(MoveList& moveList, const int ply) {
	int *moveScores = static_cast<int *>(malloc(moveList.count * 4)); // as int has size of 4 bytes

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
	free(moveScores); // remember to free the memory at the end
}

static void enablePVscoring(const MoveList& moveList) {

    followPV = 0;
    for (int count=0; count < moveList.count; count++) {
        if ( moveList.moves[count] == pvTable[0][ply] ) {
            scorePV = 1; // if we do find a move
            followPV = 1; // we are in principal variation so we want to follow it
            // maybe we should break here
        }
    }
}

static int quiescenceSearch(int alpha, const int beta) {

    if ( ply > (maxPly - 1) ) return evaluate();

    const int staticEval{ evaluate() };

    nodes++;

    if (staticEval >= beta) return beta; // known as node that fails high

    // found a better move
    if (staticEval > alpha) { // Known as PV node (principal variatio)
        alpha = staticEval;
    }

    MoveList moveList;
    generateMoves(moveList); // second parameter not acc used
    sortMoves(moveList, ply);

    for (int count=0; count < moveList.count; count++) {
        COPY_BOARD()
        ply++;

        // makeMove returns 1 for legal moves, we only want to look at captures
        if( !makeMove(moveList.moves[count], 1) ) { // meaning its illegal or its not a capture
            ply--;
            continue;
        }

        const int score = -quiescenceSearch(-beta, -alpha);
        ply--;
        RESTORE_BOARD()

        // fail-hard beta cut off
        if (score >= beta) return beta; // known as node that fails high

        // found a better move
        if (score > alpha) { // Known as PV node (principal variatio)
            alpha = score;
        }
    }

    // node that fails low
    return alpha;
}

constexpr int FullDepthMoves { 4 }; // searching the first 4 moves at the full depth
constexpr int ReductionLimit { 3 };

static int canReduce(const int move) {

	if ( getMoveCapture(move) ) return 0; // no we cannot reduce this node of the search tree
	if ( getMovePromPiece(move) ) return 0;
	return 1;
}

static int negamax(int alpha, const int beta, const int depth) {

    pvLength[ply] = ply;

    if (depth == 0) return quiescenceSearch(alpha, beta);

    nodes++;

    const int inCheck{ isSqAttacked( (side == White) ? getLeastSigBitIndex(bitboards[King]) : getLeastSigBitIndex(bitboards[King + 6]), side^1) };
    int legalMoves{};

    MoveList moveList;
    generateMoves(moveList);

    if (followPV) { // if we are following PV line, we enable scoring
        enablePVscoring(moveList);
    }

    sortMoves(moveList, ply);

	int movesSearched{};
    for (int count=0; count < moveList.count; count++) {

        COPY_BOARD()
        ply++;

        // makeMove returns 1 for legal moves
        if( !makeMove(moveList.moves[count], 0) ) { // meaning its illegal
            ply--;
            continue;
        }
        // increment legalMoves
        legalMoves++;
    	int score{};

    	// LMR from https://web.archive.org/web/20150212051846/http://www.glaurungchess.com/lmr.html
    	if(movesSearched == 0) // First move, use full-window search
    		score = -negamax(-beta, -alpha, depth-1);
    	else {
    		if( (movesSearched >= FullDepthMoves) && (depth >= ReductionLimit) && canReduce(moveList.moves[count]) )
    				score = -negamax(-(alpha+1), -alpha, depth-2); // Search this move with reduced depth:

    		else score = alpha+1;  // Hack to ensure that full-depth search for non-reduced moves

    		// principal variation search (PVS)
    		if(score > alpha) {
    			score = -negamax(-(alpha+1), -alpha, depth-1);

    			if(score > alpha && score < beta)
    				score = -negamax(-beta, -alpha, depth-1);
    		}
    	}



        ply--;
        RESTORE_BOARD()

    	movesSearched++;

        // fail-hard beta cut off
        if (score >= beta) {
            // helps with better move ordering in branches at the same depth
            if (!getMoveCapture(moveList.moves[count])) {
                killerMoves[1][ply] = killerMoves[0][ply];
                killerMoves[0][ply] = moveList.moves[count]; // store killer moves
            }
            return beta; // known as node that fails high
        }

        // found a better move
        if (score > alpha) { // Known as PV node (principal variation)
            // store history moves
            if (!getMoveCapture(moveList.moves[count])) {
                historyMoves[getMovePiece(moveList.moves[count])][getMoveTargetSQ(moveList.moves[count])] += depth; // this can be dropped doesnt give much anyway
            }
            alpha = score;

            pvTable[ply][ply] = moveList.moves[count];
            // copy move from deeper plies to curernt ply
            for (int nextPly = (ply+1); nextPly < pvLength[ply + 1]; nextPly++) {
                pvTable[ply][nextPly] = pvTable[ply + 1][nextPly];
            }

            pvLength[ply] = pvLength[ply + 1];
        }
    }

    if (legalMoves == 0) {
        if (inCheck) {
            return -49'000 + ply; // we want to return the mating score, (slightly above negative infinity, +ply scores faster mates as better moves)
        }
        return 0; // we are in stalemate
    }

    return alpha; // known as fail-low node
}

static int getMoveTime(const bool timeConstraint) {

	if (!timeConstraint) return 180'000;

	const int timeAlloted = (side == White) ? whiteClockTime : blackClockTime;
	const int increment = (side == White) ? whiteIncrementTime : blackIncrementTime;
	const int idealTimePerMove = gameLengthTime / 35; // lets say each game is 35 moves long

	int timePerMove{};

	// mostly done with the idea of blitz and bullet in mind
	if (timeAlloted > (gameLengthTime * 0.75)) timePerMove = idealTimePerMove;
	else if (timeAlloted > (gameLengthTime * 0.50)) timePerMove = idealTimePerMove / 2;
	else if (timeAlloted > (gameLengthTime * 0.25)) timePerMove = idealTimePerMove / 3;
	else if (timeAlloted > (gameLengthTime * 0.05)) timePerMove = idealTimePerMove / 4;
	else timePerMove = timeAlloted / 10; // switch to using the time alloted so we dont flag

	// we want to use up about 75% of the increment
	return timePerMove + increment * 0.75;
}


// this time management kind of makes sense but you should really implement a stopping function
void iterativeDeepening(const int depth, const bool timeConstraint){

    memset(killerMoves, 0, sizeof(killerMoves));
    memset(historyMoves, 0, sizeof(historyMoves));
    memset(pvLength, 0, sizeof(pvLength)); // though this is not 100% needed
    memset(pvTable, 0, sizeof(pvTable));

    nodes = 0;
    followPV = 0;
    scorePV = 0;

    int bestMove{};
	int currentDepth{ 1 };

	const int timePerMove { getMoveTime(timeConstraint) };
//	logFile << "Time per move: " << timePerMove << '\n';


	const auto startSearchTime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> searchDuration{ 0 };

	while ( (currentDepth <= depth) && ( (searchDuration.count() * 1'000) < timePerMove) ){
        followPV = 1;

        const auto startDepthTime = std::chrono::high_resolution_clock::now();

        const int score { negamax(-50'000, 50'000, currentDepth) };

		std::chrono::duration<float> depthDuration { std::chrono::high_resolution_clock::now() - startDepthTime };
		searchDuration = std::chrono::high_resolution_clock::now() - startSearchTime;

        bestMove = pvTable[0][0];

		// extracting the PV line and printing out in the terminal and logging file
        std::string pvString{};
        for (int count=0;  count < pvLength[0]; count++) { pvString += algebraicNotation(pvTable[0][count]) + ' '; }

        std::cout << "info score cp " << score << " depth " << currentDepth << " nodes " << nodes << " nps " << static_cast<int>(nodes / depthDuration.count()) << " pv " << pvString << '\n';
    //    logFile << "info score cp " << score << " depth " << currentDepth << " nodes " << nodes << " nps " << static_cast<int>(nodes / depthDuration.count()) << " pv " << pvString << '\n';

		currentDepth++;
    }
    // search time is up so we return the bestMove
    std::cout << "bestmove " + algebraicNotation(bestMove) << '\n';
}