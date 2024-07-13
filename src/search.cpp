//
// Created by Federico Saitta on 04/07/2024.
//
#include <iostream>
#include <fstream>
#include <string>
#include <variant>
#include <vector>

#include "macros.h"
#include "inline_functions.h"

#include "search.h"
#include "update.h"
#include "hashtable.h"
#include "uci.h"
#include "board.h"
#include "evaluation.h"
#include "misc.h"

#define MAX_PLY 64



U64 repetitionTable[1'000]{};
int repetitionIndex{};

int ply{};
static std::uint32_t nodes{};

static int killerMoves[2][128]{}; // zero initialization to ensure no random bonuses to moves
static int historyMoves[12][64]{}; // zero initialization to ensure no random bonuses to moves

static int pvLength[64]{};
static int pvTable[64][64]{};

static int followPV{}; // if it is true then we follow the principal variation
static int scorePV{};

constexpr int fullDepthMoves { 4 }; // searching the first 4 moves at the full depth
constexpr int reductionLimit { 3 };
constexpr int nullMoveReduction { 2 };

constexpr int windowWidth{ 50 }; // the aspritation window, the width is 100

static int stopSearch { 0 };
static int timePerMove { 0 };

auto startSearchTime = std::chrono::high_resolution_clock::now();
std::chrono::duration<float> searchDuration{ 0 };

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

static void resetStates() {
	memset(killerMoves, 0, sizeof(killerMoves));
	memset(historyMoves, 0, sizeof(historyMoves));
	memset(pvLength, 0, sizeof(pvLength)); // though this is not 100% needed
	memset(pvTable, 0, sizeof(pvTable));

	nodes = 0;
	followPV = 0;
	scorePV = 0;
	ply = 0;

	stopSearch = 0;
}


// we will add different scorings for PV etc
inline int scoreMove(const int move, const int ply) {

	if (scorePV) {
		if (pvTable[0][ply] == move) {
			scorePV = 0; // as there is only one principal move in a moveList
			// std::cout << "Current PV " << algebraicNotation(move) << " at ply" << ply << '\n';
			// note that because of null move and late more pruning this will not print nicely because higher depths plies are
			// not considered because of the early pruning, PV is still followed  though.
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
			if ( GET_BIT(bitboards[bbPiece], getMoveTargetSQ(move)) ) {
				targetPiece = bbPiece;
				break;
			}
		}

		// score moves by MVV-LVA, it doesnt know if pieces are protected (SEE does though)
		return mvv_lva[getMovePiece(move)][targetPiece] + 10'000; // important as we score MVV-LVA as better than killer
	}

	// new addition
	// we score promotions as MVV-LVA
	if (getMovePromPiece(move) != 0) return mvv_lva[Pawn][getMovePromPiece(move)] + 10'000;

	if (killerMoves[0][ply] == move) return 9000;

	if (killerMoves[1][ply] == move) return 8000;

	return historyMoves[getMovePiece(move)][getMoveTargetSQ(move)];
}

inline void sortMoves(MoveList& moveList, const int ply, const int best_move) {
	int *moveScores = static_cast<int*>(malloc(moveList.count * 4)); // as int has size of 4 bytes

	// scoring the moves
	for (int count=0; count < moveList.count; count++) {
		// if we have a hash move available
		if (best_move == moveList.moves[count])
			// score move
			moveScores[count] = 30000;

		else
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

        	break; // I think this breaking condition is correct
        }
    }
}

static int getMoveTime(const bool timeConstraint) {

	if (!timeConstraint) return 180'000; // maximum searching time of 3 minutes

	// adding a delay of 50 milliseconds
	const int timeAlloted = (side == White) ? whiteClockTime - 100 : blackClockTime - 100;
	const int increment = (side == White) ? whiteIncrementTime : blackIncrementTime;

	int timePerMove{ timeAlloted / 30 + increment };
	if ( (increment > 0) && (timeAlloted < (5 * increment) ) ) timePerMove = (0.75 * increment);

	return timePerMove;
}

static void isTimeUp() {
	searchDuration = std::chrono::high_resolution_clock::now() - startSearchTime;
	if ( (searchDuration.count() * 1'000) > timePerMove)
		stopSearch = 1;
}

static int isRepetition() {
	for (int index=0; index < repetitionIndex; index++) { if (repetitionTable[index] == hashKey) return 1; } // repetition found
	return 0; // if no repetition found
}

static int canReduceMove(const int move) {
	// This does cause some slow down clearly, but improves search stability
	// should definetely be tested in many games
	COPY_BOARD()
	makeMove(move, 0);

	const int opponentInCheck { isSqAttacked( (side == White) ? getLeastSigBitIndex(bitboards[King]) : getLeastSigBitIndex(bitboards[King + 6]) , !side) };

	RESTORE_BOARD()
	return !opponentInCheck;
}


static int quiescenceSearch(int alpha, const int beta) {
	if ((nodes & 2047) == 0)
		isTimeUp();

	nodes++;

    if ( ply > (MAX_PLY - 1) ) return evaluate();

    const int staticEval{ evaluate() };

	// delta pruning, to be implemented
	if (staticEval < alpha - 975) return alpha;

    if (staticEval > alpha) {
    	if (staticEval >= beta)
    		return beta; // known as node that fails high

	    alpha = staticEval; // Known as PV node (principal variation)
    }

    MoveList moveList;
    generateMoves(moveList);
    sortMoves(moveList, ply, 0);

    for (int count=0; count < moveList.count; count++) {
        COPY_BOARD()
        ply++;
    	repetitionIndex++;
    	repetitionTable[repetitionIndex] = hashKey;

        // makeMove returns 1 for legal moves, we only want to look at captures
        if( !makeMove(moveList.moves[count], 1) ) { // meaning its illegal or its not a capture
            ply--;
        	repetitionIndex--;
            continue;
        }

        const int score = -quiescenceSearch(-beta, -alpha);
        ply--;
    	repetitionIndex--;
        RESTORE_BOARD()

    	if (stopSearch) return 0;

        // found a better move
        if (score > alpha) { // Known as PV node (principal variatio)
            alpha = score;

        	// fail-hard beta cut off
        	if (score >= beta) return beta; // known as node that fails high
        }
    }

    return alpha; // node that fails low
}

static inline U64 nonPawnMat() {
	return ( bitboards[Queen + 6 * side] | bitboards[Rook + 6 * side] | bitboards[Bishop + 6 * side] | bitboards[Knight + 6 * side]);
}


static inline int negamax(int alpha, const int beta, int depth) {

	pvLength[ply] = ply;
	int score;
	int bestMove {};
	int hashFlag{ HASH_FLAG_ALPHA };

	if (ply && isRepetition()) return 0; // we return draw score if we detect a draw

	// to figure ot if the current node is a principal variation node
	// reading the TT table, if we the move has already been searched, we return its evaluation
	// ply && used to ensure we dont read from the transposition table at the root node
	int pvNode = beta - alpha > 1;
	if (ply && (score = probeHash(alpha, beta, &bestMove, depth)) != NO_HASH_ENTRY && !pvNode) return score;

	if ((nodes & 2047) == 0)
		isTimeUp();


	if ( depth == 0 ) return quiescenceSearch(alpha, beta);

	if (ply > MAX_PLY - 1)
		// evaluate position
		return evaluate();

	nodes++;

	const int inCheck{ isSqAttacked( (side == White) ? getLeastSigBitIndex(bitboards[King]) : getLeastSigBitIndex(bitboards[King + 6]), side^1) };
	int legalMoves{};

	// increase search depth if the king has been exposed into a check
	if (inCheck) depth++;

	const int static_eval { evaluate() };


	// evaluation pruning / static null move pruning
	if (depth < 3 && !pvNode && !inCheck &&  abs(beta - 1) > -MAX_VALUE + 100)
	{
		// define evaluation margin
		const int evalMargin = 120 * depth;

		// evaluation margin substracted from static evaluation score fails high
		if (static_eval - evalMargin >= beta)
			// evaluation margin substracted from static evaluation score
				return static_eval - evalMargin;
	}


	// maybe you can write TT entries here too???
	// NULL MOVE PRUNING: https://web.archive.org/web/20071031095933/http://www.brucemo.com/compchess/programming/nullmove.htm
	// position fen 8/k7/3p4/p2P1p2/P2P1P2/8/8/K7 w - - 0 1, in this position if we disable null move pruning it finds the correct sequence
	if (depth >= 3 && !inCheck && ply && nonPawnMat()) { // we do not attempt null move pruning in case our side only has pawns on the board
		COPY_BOARD()

		hashKey ^= sideKey;
		if (enPassantSQ != 64) hashKey ^= randomEnPassantKeys[enPassantSQ];

		side ^= 1; // make null move
		enPassantSQ = 64; // resetting en-passant to null-square

		// we change plies so white and black killers remain in sync for negamax search
		ply++;
		repetitionIndex++;
		repetitionTable[repetitionIndex] = hashKey;

		// possible more aggressive reduction? int reduction{ 3 + depth / 6 };
		// old reduction code: depth - 1 - nullMoveReduction
		const int nullMoveScore = -negamax(-beta, -beta + 1, depth - 1 - nullMoveReduction);
		ply--;
		repetitionIndex--;

		RESTORE_BOARD() // un-making the null move

		if (stopSearch)
			return 0;

		if (nullMoveScore >= beta) return beta;
	}
	// NOTE THAT NULL MOVE PRUNING IS MAKING ONE OF THE MATES NOT TO BE FOUND


	// razoring
	if (!pvNode && !inCheck && depth <= 3)
	{
		// get static eval and add first bonus
		score = static_eval + 125;

		int newScore; // define new score

		// static evaluation indicates a fail-low node
		if (score < beta)
		{
			// on depth 1
			if (depth == 1)
			{
				// get quiscence score
				newScore = quiescenceSearch(alpha, beta);

				// return quiescence score if it's greater then static evaluation score
				return (newScore > score) ? newScore : score;
			}

			// add second bonus to static evaluation
			score += 175;

			// static evaluation indicates a fail-low node
			if (score < beta && depth <= 2)
			{
				// get quiscence score
				newScore = quiescenceSearch(alpha, beta);

				// quiescence score indicates fail-low node
				if (newScore < beta)
					// return quiescence score if it's greater then static evaluation score
						return (newScore > score) ? newScore : score;
			}
		}
	}


    MoveList moveList;
    generateMoves(moveList);

    if (followPV) { // if we are following PV line, we enable scoring
        enablePVscoring(moveList);
    }

    sortMoves(moveList, ply, bestMove);

	int movesSearched{};

	//MoveList quietList;
	//int quietCount{};

    for (int count=0; count < moveList.count; count++) {

        COPY_BOARD()
        ply++;
    	repetitionIndex++;
    	repetitionTable[repetitionIndex] = hashKey;

        // makeMove returns 1 for legal moves
        if( !makeMove(moveList.moves[count], 0) ) { // meaning its illegal
            ply--;
        	repetitionIndex--;
            continue;
        }

        // increment legalMoves
        legalMoves++;

    	// no need to check enPassant as we do count it as a capture
    	const bool isQuiet = ( !getMoveCapture(moveList.moves[count]) && !getMovePromPiece(moveList.moves[count]) );

    	// needed for improvements to move ordereing etc.
    	//if (isQuiet){
    	//	quietList.moves[quietCount] = moveList.moves[count];
    	//	quietCount++;
    	//}

    	// LMR from https://web.archive.org/web/20150212051846/http://www.glaurungchess.com/lmr.html
    	if(movesSearched == 0) {
    		// First move, use full-window search // this is the principal variation move
    		score = -negamax(-beta, -alpha, depth-1);
    	} else {
    		if( (movesSearched >= fullDepthMoves) && (depth >= reductionLimit)
    			&& isQuiet			// will reduce quiet moves
    			&& !inCheck         // will not reduce in case we are in check
    			&& canReduceMove(moveList.moves[count]))    // will not reduce in case we put opponent in check, is this worth the speed loss?

    				// some other heuristics can also be implemented though they are more complicated
    				score = -negamax(-alpha-1, -alpha, depth-2); // Search this move with reduced depth:

    		else score = alpha+1;  // Hack to ensure that full-depth search for non-reduced moves

    		// principal variation search (PVS)
    		if(score > alpha) {
    			score = -negamax(-alpha-1, -alpha, depth-1);

    			if( (score > alpha) && (score < beta) )
    				score = -negamax(-beta, -alpha, depth-1);
    		}
    	}
        ply--;
    	repetitionIndex--;
        RESTORE_BOARD()

    	if (stopSearch)
    		return 0;

    	movesSearched++;

        // found a better move
        if (score > alpha) { // Known as PV node (principal variation)
        	hashFlag = HASH_FLAG_EXACT;
            alpha = score;

        	// store best move (for TT)
        	bestMove = moveList.moves[count];

            pvTable[ply][ply] = moveList.moves[count];
            // copy move from deeper plies to curernt ply
            for (int nextPly = (ply+1); nextPly < pvLength[ply + 1]; nextPly++) {
                pvTable[ply][nextPly] = pvTable[ply + 1][nextPly];
            }

            pvLength[ply] = pvLength[ply + 1];

        	// fail-hard beta cut off
        	if (score >= beta) {
        		// helps with better move ordering in branches at the same depth
        		if (!getMoveCapture(moveList.moves[count])) {
        			killerMoves[1][ply] = killerMoves[0][ply];
        			killerMoves[0][ply] = bestMove; // store killer moves

        			// can do more sophisticated code tho
        			// check if this should go anywhere alpha increases or where we get a beta cut off
        			historyMoves[getMovePiece(bestMove)][getMoveTargetSQ(bestMove)] += depth * depth;

					/*
        			for (int i = 0; i < quietCount; i++) {
        				const int quietMove = quietList.moves[i];
        				if (quietMove == bestMove) continue;

        				// penalize history of moves which didn't cause beta-cutoffs
        				historyMoves[getMovePiece(quietMove)][getMoveTargetSQ(quietMove)] -= depth * depth;
        			}
        			*/
        		}

        		recordHash(beta, bestMove, HASH_FLAG_BETA, depth);
        		return beta; // known as node that fails high
        	}
        }

    }

    if (!legalMoves) { // we dont have any legal moves to make in this position
        if (inCheck) {
        	// we need to adjust this before sending it to the transposition table to make it independent of the path
        	// from the root node to the mating node
            return -MATE_VALUE+ ply; // we want to return the mating score, (slightly above negative MAX_VALUE, +ply scores faster mates as better moves)
        }
        return 0; // we are in stalemate
    }

	recordHash(alpha, bestMove, hashFlag, depth);
    return alpha; // known as fail-low node
}

void iterativeDeepening(const int depth, const bool timeConstraint) {

	resetStates();

	timePerMove = getMoveTime(timeConstraint);
	//logFile << "Time per move: " << timePerMove << '\n';
	if (timePerMove < 0) logFile << "NEGATIVE TIME********\n";

	int alpha { -MAX_VALUE };
	int beta { MAX_VALUE };

	startSearchTime = std::chrono::high_resolution_clock::now();

	//logFile << "Starting Iterative deepening\n";
	for (int currentDepth = 1; currentDepth <= depth; ){
        followPV = 1;

		if (stopSearch)
			break;

        const auto startDepthTime = std::chrono::high_resolution_clock::now();

        const int score { negamax(alpha, beta, currentDepth) };

		std::chrono::duration<float> depthDuration { std::chrono::high_resolution_clock::now() - startDepthTime };

		if ( (searchDuration.count() * 1'000) > timePerMove) break;

        if ( (score <= alpha) || (score >= beta) ) { // we fell outside the window
	        alpha = -MAX_VALUE;
        	beta = MAX_VALUE;
        	continue; // we redo the search at the same depth
        }
		// otherwise we set up the window for the next iteration
		alpha = score - windowWidth;
		beta = score + windowWidth;

		// extracting the PV line and printing out in the terminal and logging file
        std::string pvString{};
        for (int count=0;  count < pvLength[0]; count++) { pvString += algebraicNotation(pvTable[0][count]) + ' '; }

		// check if we need to send mating scores
		if ( score > -MATE_VALUE && score < -MATE_SCORE) {
			std::cout << "info score mate " << -(score + MATE_VALUE) / 2 - 1 << " depth " << currentDepth << " nodes " << nodes << " nps " << static_cast<int>(nodes / depthDuration.count())
			<< " time " << static_cast<int>(depthDuration.count() * 1'000) << " pv " << pvString << '\n';
		} else if( score > MATE_SCORE && score < MATE_VALUE) {
			std::cout << "info score mate " << (MATE_VALUE - score) / 2 + 1 << " depth " << currentDepth << " nodes " << nodes << " nps " << static_cast<int>(nodes / depthDuration.count())
			<< " time " << static_cast<int>(depthDuration.count() * 1'000) << " pv " << pvString << '\n';
		} else {
			std::cout << "info score cp " << score << " depth " << currentDepth << " nodes " << nodes << " nps " << static_cast<int>(nodes / depthDuration.count())
			<< " time " << static_cast<int>(depthDuration.count() * 1'000) << " pv " << pvString << '\n';
		}

		currentDepth++; // we can proceed to the next iteration

    }

//	logFile << "bestmove " + algebraicNotation(pvTable[0][0]) << '\n';
    // search time is up so we return the bestMove
    std::cout << "bestmove " + algebraicNotation(pvTable[0][0]) << '\n';
}

