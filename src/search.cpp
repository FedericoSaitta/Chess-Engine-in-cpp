//
// Created by Federico Saitta on 04/07/2024.
//
#include <iostream>
#include <fstream>
#include <string>
#include <variant>

#include "macros.h"
#include "inline_functions.h"

#include "search.h"
#include "update.h"
#include "hashtable.h"
#include "uci.h"
#include "board.h"
#include "evaluation.h"
#include "misc.h"
#include "movesort.h"

#define MAX_PLY 64

U64 repetitionTable[1'000]{};
int repetitionIndex{};

int ply{};
static std::uint32_t nodes{};

int killerMoves[2][128]{}; // zero initialization to ensure no random bonuses to moves
int historyMoves[12][64]{}; // zero initialization to ensure no random bonuses to moves

int pvTable[64][64]{};
static int pvLength[64]{};

int scorePV{};
static int followPV{}; // if it is true then we follow the principal variation

constexpr int fullDepthMoves { 4 }; // searching the first 4 moves at the full depth
constexpr int reductionLimit { 3 };
constexpr int nullMoveReduction { 2 };

constexpr int windowWidth{ 50 }; // the aspritation window, the width is 100

static int stopSearch { 0 };
static int timePerMove { 0 };

auto startSearchTime = std::chrono::high_resolution_clock::now();
std::chrono::duration<float> searchDuration{ 0 };


static void resetStates() {
	memset(killerMoves, 0, sizeof(killerMoves));
	memset(historyMoves, 0, sizeof(historyMoves));
	memset(pvLength, 0, sizeof(pvLength));
	memset(pvTable, 0, sizeof(pvTable));

	nodes = 0;
	followPV = 0;
	scorePV = 0;
	ply = 0;

	stopSearch = 0;
}
static void enablePVscoring(const MoveList& moveList) {
    followPV = 0;
    for (int count=0; count < moveList.count; count++) {
        if ( moveList.moves[count] == pvTable[0][ply] ) {
            scorePV = 1; // if we do find a move
            followPV = 1; // we are in principal variation so we want to follow it

        	break; 
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

	if ( (searchDuration.count() * 1'000) > timePerMove) stopSearch = 1;
}

static int isRepetition() {
	for (int index=0; index < repetitionIndex; index++) { if (repetitionTable[index] == hashKey) return 1; } // repetition found
	return 0; // if no repetition found
}

static U64 nonPawnMaterial() {
	return ( bitboards[Queen + 6 * side] | bitboards[Rook + 6 * side] | bitboards[Bishop + 6 * side] | bitboards[Knight + 6 * side]);
}

static int canReduceMove(const int move) {
	// This does cause some slow down clearly, but improves search stability should definetely be tested in many games
	COPY_BOARD()
	makeMove(move, 0);

	const int opponentInCheck { isSqAttacked( (side == White) ? getLeastSigBitIndex(bitboards[King]) : getLeastSigBitIndex(bitboards[King + 6]) , !side) };

	RESTORE_BOARD()
	return !opponentInCheck;
}


static int quiescenceSearch(int alpha, const int beta) {

	if ((nodes & 2047) == 0) isTimeUp();

	nodes++;

    if ( ply > (MAX_PLY - 1) ) return evaluate();

    const int staticEval{ evaluate() };

	// delta pruning
	if (staticEval < alpha - 975) return alpha;

    if (staticEval > alpha) {
    	if (staticEval >= beta) {
    		return beta; // known as node that fails high
    	}

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

 		if (stopSearch) return 0; // If the time is up, we return 0;

        // found a better move
        if (score > alpha) { // Known as PV node (principal variation)
            alpha = score;

        	// fail-hard beta cut off
        	if (score >= beta) return beta; // known as node that fails high
        }
    }

    return alpha; // node that fails low
}




static inline int negamax(int alpha, const int beta, int depth) {

	pvLength[ply] = ply;
	int score;
	int bestMove {};
	int hashFlag{ HASH_FLAG_ALPHA };

	if (ply && isRepetition()) return 0; // we return draw score if we detect a three-fold repetition

	
	// ply && used to ensure we dont read from the transposition table at the root node
	const bool pvNode = (beta - alpha) > 1; // Trick used to find if current node is pvNode

	// reading the TT table, if we the move has already been searched, we return its evaluation
	if (ply && (score = probeHash(alpha, beta, &bestMove, depth)) != NO_HASH_ENTRY && !pvNode) return score;

	if ((nodes & 2047) == 0) isTimeUp();
	if (ply > MAX_PLY - 1) return evaluate();

	if ( depth == 0 ) return quiescenceSearch(alpha, beta);

	nodes++;

	const int inCheck{ isSqAttacked( (side == White) ? getLeastSigBitIndex(bitboards[King]) : getLeastSigBitIndex(bitboards[King + 6]), side^1) };

	int legalMoves{};

	// Search extension if side is in check
	if (inCheck) depth++;

	const int static_eval { evaluate() };

	// static null move pruning
	if (depth < 3 && !pvNode && !inCheck &&  std::abs(beta - 1) > -MAX_VALUE + 100)
	{
		const int evalMargin = 120 * depth; // the evaluation margin

		// evaluation margin substracted from static evaluation score fails high
		if (static_eval - evalMargin >= beta)
			// evaluation margin substracted from static evaluation score
			return static_eval - evalMargin;
	}


	// maybe you can write TT entries here too???
	// NULL MOVE PRUNING: https://web.archive.org/web/20071031095933/http://www.brucemo.com/compchess/programming/nullmove.htm
	// Do not attempt null move pruning in case our side only has pawns on the board
	if (depth >= 3 && !inCheck && ply && nonPawnMaterial()) {
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

		if (stopSearch) return 0;
		if (nullMoveScore >= beta) return beta;
	}
	// NOTE THAT NULL MOVE PRUNING IS MAKING ONE OF THE MATES NOT TO BE FOUND

	// razoring
	if (!pvNode && !inCheck && depth <= 3){
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

	// if we are following PV line, we enable scoring
    if (followPV) enablePVscoring(moveList);

    sortMoves(moveList, ply, bestMove);
	int movesSearched{};

	//MoveList quietList;
	//int quietCount{};
	// bool skipQuietMoves{ false };

    for (int count=0; count < moveList.count; count++) {
    	const int move { moveList.moves[count] };
    	const bool isQuiet = ( !getMoveCapture(move) && !getMovePromPiece(move) );


    	//if (isQuiet && skipQuietMoves)
    	//	continue;

        COPY_BOARD()
        ply++;
    	repetitionIndex++;
    	repetitionTable[repetitionIndex] = hashKey;

        // makeMove returns 1 for legal moves
        if( !makeMove(move, 0) ) { // meaning its illegal
            ply--;
        	repetitionIndex--;
            continue;
        }

        // increment legalMoves
        legalMoves++;

    	// no need to check enPassant as we do count it as a capture
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
    			&& canReduceMove(move) ) {
    			// will not reduce in case we put opponent in check, is this worth the speed loss?

    			// by one ply for the first 6 moves and by depth / 3 for remaining moves.
    			const int reduction = (count < 7) ? 1 : depth / 3;
    			// some other heuristics can also be implemented though they are more complicated
    			score = -negamax(-alpha-1, -alpha, depth-1-reduction); // Search this move with reduced depth:
    		}
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
        	bestMove = move;

            pvTable[ply][ply] = moveList.moves[count];
            // copy move from deeper plies to curernt ply
            for (int nextPly = (ply+1); nextPly < pvLength[ply + 1]; nextPly++) {
                pvTable[ply][nextPly] = pvTable[ply + 1][nextPly];
            }

            pvLength[ply] = pvLength[ply + 1];

        	// fail-hard beta cut off
        	if (score >= beta) {
        		// helps with better move ordering in branches at the same depth
        		if (!getMoveCapture(move)) {
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

