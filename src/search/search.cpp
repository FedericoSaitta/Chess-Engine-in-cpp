//
// Created by Federico Saitta on 04/07/2024.
//
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <cstdlib>
#include <algorithm>

#include "game_statistics.h"
#include "../include/macros.h"
#include "../include/inline_functions.h"

#include "config.h"

#include "search.h"
#include "searchparams.h"

#include <assert.h>
#include <cstdint>
#include <cmath>
#include <mach-o/dyld.h>

#include "board.h"

#include "../movegen/update.h"

#include "../movegen/update.h"
#include "../include/hashtable.h"
#include "timer.h"
#include "../include/uci.h"
#include "../include/board.h"
#include "../eval/evaluation.h"
#include "../include/misc.h"
#include "movesort.h"
#include "../logger/logger.h"

U64 repetitionTable[512]{};
int repetitionIndex{};

int searchPly{};

Move killerMoves[2][MAX_PLY]{}; // zero initialization to ensure no random bonuses to moves
int historyScores[64][64]{}; // zero initialization to ensure no random bonuses to moves

Move pvTable[MAX_PLY][MAX_PLY]{};
static int pvLength[MAX_PLY]{};

int scorePV{};
static int followPV{}; // if it is true then we follow the principal variation

static int LMR_table[MAX_PLY][MAX_PLY];
static int LMP_table[2][11];

Timer searchTimer{};
Timer singleDepthTimer{};

int nodes{};

void initSearchTables() {
		for(int depth = 1; depth < MAX_PLY; depth++) {
			for(int played = 1; played < 64; played++) {
				LMR_table[depth][played] = static_cast<int>( LMR_BASE + std::log(depth) * std::log(played) / LMR_DIVISION ); // formula from Berserk engine
			}
		}
	LMR_table[0][0] = LMR_table[1][0] =  LMR_table[0][1] = 0;

	// Implement this once you have got improving heuristic done
	// from Berserk chess engine
	for(int depth = 1; depth < 10; depth++) {
		LMP_table[0][depth] = static_cast<int>( 2.07 +  0.3743 * depth * depth );
		LMP_table[1][depth] = static_cast<int>( 3.87 +  0.712 * depth * depth );
	}
}
static void resetSearchStates() {
	memset(killerMoves, 0, sizeof(killerMoves));
	memset(pvLength, 0, sizeof(pvLength));
	memset(pvTable, 0, sizeof(pvTable));

	nodes = 0;
	followPV = 0;
	scorePV = 0;
	searchPly = 0;

	stopSearch = 0;
}


static void enablePVscoring(const MoveList& moveList) {
    followPV = 0;
    for (int count=0; count < moveList.count; count++) {
        if ( moveList.moves[count].first == pvTable[0][searchPly] ) {
            scorePV = 1; // if we do find a move
            followPV = 1; // we are in principal variation so we want to follow it
        	break; 
        }
    }
}

static int getMoveTime(const bool timeConstraint) {

	// std::cout << whiteClockTime << ' ' << blackClockTime << ' ' << whiteIncrementTime << ' ' << blackIncrementTime << ' ' << movesToGo << '\n';
	if (!timeConstraint) return 180'000; // maximum searching time of 3 minutes

	// We give 100 millisecond lag compensation
	const int timeAlloted = (board.side == WHITE) ? whiteClockTime - 100 : blackClockTime - 100;
	const int increment = (board.side == WHITE) ? whiteIncrementTime : blackIncrementTime;
	int moveTime{};

	if (movesToGo == 0) {
		 moveTime = timeAlloted / 30 + increment;
		if ( (increment > 0) && (timeAlloted < (5 * increment) ) ) moveTime = static_cast<int>(0.75 * increment);
	} else {
		moveTime = timeAlloted / movesToGo;
	}

	assert((moveTime > 0) && "getMoveTime: movetime is zero/negative");
	return moveTime;
}
static void isTimeUp() {
	if ( searchTimer.elapsed() > timePerMove) stopSearch = 1;
}
static int isRepetition() {
	for (int index=repetitionIndex - 1; index >= 0; index-= 2) {
		// looping backwards over our previous keys
		if (repetitionTable[index] == hashKey)
			return 1; // repetition found
	}
	return 0; // no repetition
}

static void updateKillers(const Move bestMove) {

	// update killer moves if we found a new unique bestMove
	if (killerMoves[0][searchPly] != bestMove) {
		assert(!bestMove.isNone() && "updateKillers: bestMove is empty");

		killerMoves[1][searchPly] = killerMoves[0][searchPly];
		killerMoves[0][searchPly] = bestMove; // store killer moves
	}
}

static bool isKiller(const Move move) {
	assert(!move.isNone());
	if ( (move == killerMoves[0][searchPly]) || (move == killerMoves[1][searchPly]) ) return true;
	return false;
}

void printKillerTable() {
	for(int i=0; i<MAX_PLY; i++) {
		std::cout << "\n KILLER 1: ";
		printMove(killerMoves[0][i]);

		std::cout << "  KILLER 2: ";
		printMove(killerMoves[1][i]);
	}
}

static void updateHistory(const Move bestMove, const int depth, const Move* quiets, const int quietMoveCount) {
	const int bonus = std::min(2100, 300 * depth - 300);

	// Bonus to the move that caused the beta cutoff
	if (depth > 2) {
		historyScores[bestMove.from()][bestMove.to()] += bonus - historyScores[bestMove.from()][bestMove.to()] * std::abs(bonus) / MAX_HISTORY_SCORE;
	}

	// Penalize quiet moves that failed to produce a cut only if bestMove is also quiet
	assert((quietMoveCount <= 32) && "updateHistory: quietMoveCount is too large");
	for (int i = 0; i < quietMoveCount; i++) {
		Move m = quiets[i];
		assert(m != Move(0, 0));

		// Could i avoid this if check with double the bonus?
		// We do not want to cancel the bonus we just handed to the bestMove
		if (m != bestMove) historyScores[m.from()][m.to()] += -bonus - historyScores[m.from()][m.to()] * std::abs(bonus) / MAX_HISTORY_SCORE;
	}

}

static int quiescenceSearch(int alpha, const int beta) {

	if ((nodes & 4095) == 0) isTimeUp();
	if (stopSearch) return 0; // If the time is up, we return 0;

	if ( searchPly > (MAX_PLY - 1) ) return evaluate(board);

	const int standPat{ evaluate(board) };

	// delta pruning
	if (standPat < (alpha - 975) ) return alpha;

	if (standPat >= beta) return standPat; // fail soft

	if (standPat > alpha) {
		alpha = standPat; // Known as PV node (principal variation)
	}

	MoveList moveList;
	generateMoves(moveList);

	giveScores(moveList, Move(0, 0)); // as there is no bestmove

	int bestEval { standPat };

	for (int count=0; count < moveList.count; count++) {
		const Move move { pickBestMove(moveList, count ) };

		COPY_HASH()
		searchPly++;
		repetitionIndex++;
		repetitionTable[repetitionIndex] = hashKey;

		// board.undo Illegal Moves or non-captures
		if( !board.makeMove(move, 1) ) {
			searchPly--;
			repetitionIndex--;
			continue;
		}

		const int score = -quiescenceSearch(-beta, -alpha);

		searchPly--;
		repetitionIndex--;
		board.undo(move);
		RESTORE_HASH()

		// found a better move
		if (score > bestEval) { // Known as PV node (principal variation)
			bestEval = score;

			if (score > alpha) {
				alpha = score;

				if (score >= beta) {
					break; // fail soft
				}
			}
		}
	}

	return bestEval; // node that fails low
}

static void ageHistory() {
	for (int a=0; a<64; a++) {
		for (int b=0; b<64; b++) {
			historyScores[a][b] /= 2;
		}
	}
}


static int negamax(int alpha, const int beta, int depth, const NodeType canNull) {
	pvLength[searchPly] = searchPly;
	Move bestMove {}; // for now as tt is turned off this is just a null move
	int bestEval {-INF - 1};

	if (searchPly && isRepetition()) return 0; // we return draw score if we detect a three-fold repetition

	const bool pvNode = (beta - alpha) > 1; // Trick used to find if current node is pvNode

	// reading the TT table, if we the move has already been searched, we return its evaluation
	// ply && used to ensure we dont read from the transposition table at the root node
	int score = probeHash(alpha, beta, &bestMove, depth);
	const bool ttHit = score != NO_HASH_ENTRY;
	if (searchPly && ttHit && !pvNode) return score;

	if ((nodes & 4095) == 0) isTimeUp();
	if (searchPly > MAX_PLY - 1) return evaluate(board);
	if ( depth < 1 ) return quiescenceSearch(alpha, beta);

	const int inCheck{ board.currentlyInCheck() };
	if (inCheck) depth++; // Search extension if board.side is in check

	// STATIC NULL MOVE PRUNING / REVERSE FUTILITY PRUNING
	if (depth < 3 && (!pvNode) && (!inCheck) &&  (std::abs(beta - 1) > (-INF + 100) ) )
	{
		// new addition avoid having to re-evaluate if we already have a tt eval
		const int staticEval{ (ttHit) ? score : evaluate(board) };
		// evaluation margin substracted from static evaluation score fails high
		if (staticEval - (120 * depth) >= beta)
			// evaluation margin substracted from static evaluation score
				return staticEval - 120 * depth;
	}

	// most of these can be greatly improved with improving heuristic
	if (!pvNode && !inCheck && searchPly) {

		// i have not added the weird trick that the old_search has


		// NULL MOVE PRUNING: https://web.archive.org/web/20071031095933/http://www.brucemo.com/compchess/programming/nullmove.htm
		// Do not attempt null move pruning in case our board.side only has pawns on the board
		// maybe you need a flag to make sure you dont re-attempt null move twice in a row?
		// no NULL flag used to ensure we dont do two null moves in a row
		if (depth > 3  && canNull && board.nonPawnMaterial()) {
			COPY_HASH()
			board.nullMove();

			searchPly++;
			repetitionIndex++;
			repetitionTable[repetitionIndex] = hashKey;

			// more aggressive reduction
			const int r = std::min(4 + depth / 4, depth);
			const int nullMoveScore = -negamax(-beta, -beta + 1, depth - r, DONT_NULL);

			board.undoNullMove();
			searchPly--;
			repetitionIndex--;
			RESTORE_HASH() // un-making the null move

			if (nullMoveScore >= beta) return beta;
		}

		// razoring
		if (depth <= 3 && canNull){
			// get static eval and add first bonus
			score = evaluate(board) + 125;

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
	}



    MoveList moveList;
    generateMoves(moveList);

    if (followPV) enablePVscoring(moveList); // check if we are in a principal-variation node

	const int originalAlpha {alpha};
	int legalMoves{};
	int movesSearched{};

	// score the moves before picking the best one
	giveScores(moveList, bestMove);

	Move quiets[32];
	int quietMoveCount{};
	bool skipQuiets{ false };

    for (int count=0; count < moveList.count; count++) {
    	const Move move { pickBestMove(moveList, count ) };

    	// En-passant, captures and promotions are noisy
    	const bool isQuiet = ( !move.isNoisy() );

    	if (isQuiet && skipQuiets) continue;

    	// late move pruning, we use the more conservative measure right now as i dont have
    	// improving heuristic
    	if (searchPly && !inCheck && isQuiet && bestEval > -MATE_SCORE) {

    		// parameters obtained from CARP
    		if (!pvNode && depth <= 8 && quietMoveCount >= (4 + depth * depth)) {
    			skipQuiets= true;
    			continue;
    		}
    	}

        COPY_HASH()
        searchPly++;
    	repetitionIndex++;
    	repetitionTable[repetitionIndex] = hashKey;

        // Illegal Moves
        if( !board.makeMove(move, 0) ) {
            searchPly--;
        	repetitionIndex--;

        	assert((searchPly >= 0) && "negamax: searchPly too small");
        	assert((repetitionIndex >= 0) && "negamax: repetition index too small");
        	assert((generateHashKey() == hashKey) && "negamax: hashKey is wrong illegal move");
            continue;
        }

        legalMoves++;

    	if (isQuiet && (quietMoveCount < 31) ) {
    	    quiets[quietMoveCount] = move;
    	    quietMoveCount++;
    	}

    	// ****  LATE MOVE REDUCTION (LMR) **** // +45.88 +/- 20.68
    	if(movesSearched == 0) {
    		// https://web.archive.org/web/20150212051846/http://www.glaurungchess.com/lmr.html
    		// First move, use full-window search // this is the principal variation move
    		score = -negamax(-beta, -alpha, depth-1, DO_NULL);
    	}
    	else {
    		// do not reduce noisy moves
    		if( (movesSearched >= LMR_MIN_MOVES) && (depth >= LMR_MIN_DEPTH) && isQuiet ) {

    			int reduction = LMR_table[std::min(depth, 63)][std::min(count, 63)];

    			reduction = std::min(depth - 1, std::max(reduction, 1)); // to avoid dropping into qs directly
    			score = -negamax(-alpha-1, -alpha, depth-1-reduction, DO_NULL); // Search this move with reduced depth:
    		}

    		else score = alpha+1;  // Hack to ensure that full-depth search for non-reduced moves

    		// principal variation search (PVS)
    		if(score > alpha) {
    			score = -negamax(-alpha-1, -alpha, depth-1, DO_NULL);

    			if( (score > alpha) && (score < beta) )
    				score = -negamax(-beta, -alpha, depth-1, DO_NULL);
    		}
    	}
        searchPly--;
    	repetitionIndex--;
    	board.undo(move);
        RESTORE_HASH()

    	if (stopSearch) return 0;

    	movesSearched++;

    	// fail soft framework
    	if (score > bestEval) {
    		// Known as PV node (principal variation)
    		bestEval = score;

    		if (score > alpha){
    			alpha = score;
    			bestMove = move; // store best move (for TT)

    			pvTable[searchPly][searchPly] = move;
    			// copy move from deeper plies to curernt ply
    			for (int nextPly = (searchPly+1); nextPly < pvLength[searchPly + 1]; nextPly++) {
    				pvTable[searchPly][nextPly] = pvTable[searchPly + 1][nextPly];
    			}

    			pvLength[searchPly] = pvLength[searchPly + 1];

    			// fail-hard beta cut off
    			if (score >= beta) {
    				// helps with better move ordering in branches at the same depth
    				if (isQuiet) {
    					updateKillers(bestMove);
    					updateHistory(bestMove, depth, quiets, quietMoveCount);
    				}
    				break;
    			}
    		}
    	}
    }

	// After we have looped over the possible moves, check for stalemate or checkmate
	if (!legalMoves) { // we dont have any legal moves to make in this position
		if (inCheck) {
			// we need to adjust this before sending it to the transposition table to make it independent of the path
			// from the root node to the mating node
			return -MATE_VALUE+ searchPly; // we want to return the mating score, (slightly above negative INF, +ply scores faster mates as better moves)
		}
		return 0; // we are in stalemate
	}

	int hashFlag = HASH_FLAG_EXACT;
	if (alpha >= beta) hashFlag = HASH_FLAG_BETA; // beta cutoff, fail high
	else if (alpha <= originalAlpha) hashFlag = HASH_FLAG_ALPHA; // failed to raise alpha, fail low

	if (bestEval != (-INF - 1)) recordHash(bestEval, bestMove, hashFlag, depth);

	return bestEval; // known as fail-low node
}



void iterativeDeepening(const int depth, const bool timeConstraint) {
	resetSearchStates();

	timePerMove = getMoveTime(timeConstraint);

	const int softTimeLimit = static_cast<int>(timePerMove / 3.0);


	int alpha { -INF };
	int beta { INF };

	searchTimer.reset();

	for (int currentDepth = 1; currentDepth <= depth; ){
		nodes = 0;
        followPV = 1;
		if (stopSearch) break;
		if (searchTimer.elapsed() > softTimeLimit) break;

        singleDepthTimer.reset();

        const int score { negamax(alpha, beta, currentDepth, DO_NULL) };

		// If the previous search exceeds the hard or soft time limit, we stop searching
        if ( (score <= alpha) || (score >= beta) ) { // we fell outside the window
	        alpha = -INF;
        	beta = INF;
        	continue; // we redo the search at the same depth
        }
		// otherwise we set up the window for the next iteration
		alpha = score - windowWidth;
		beta = score + windowWidth;

		// checking after the search to prevent from printing empty pv string
		if (stopSearch) break;

		// extracting the PV line and printing out in the terminal and logging file
        std::string pvString{};
        for (int count=0;  count < pvLength[0]; count++) { pvString += algebraicNotation(pvTable[0][count]) + ' '; }

		// check if we need to send mating scores
		if ( score > -MATE_VALUE && score < -MATE_SCORE) {
			std::cout << "info score mate " << -(score + MATE_VALUE) / 2 - 1 << " depth " << currentDepth << " nodes " << nodes << " nps " << static_cast<std::int64_t>(1000 * nodes / singleDepthTimer.elapsed())
			<< " time " << singleDepthTimer.roundedElapsed() << " pv " << pvString << std::endl;

		} else if( score > MATE_SCORE && score < MATE_VALUE) {
			std::cout << "info score mate " << (MATE_VALUE - score) / 2 + 1 << " depth " << currentDepth << " nodes " << nodes << " nps " << static_cast<std::int64_t>(1000 * nodes / singleDepthTimer.elapsed())
			<< " time " << singleDepthTimer.roundedElapsed() << " pv " << pvString << std::endl;
		} else {
			std::cout << "info score cp " << score << " depth " << currentDepth << " nodes " << nodes << " nps " << static_cast<int>(1000 * nodes / singleDepthTimer.elapsed())
			<< " time " << singleDepthTimer.roundedElapsed() << " pv " << pvString << std::endl;
		}

		currentDepth++; // we can proceed to the next iteration
    }
    std::cout << "bestmove " + algebraicNotation(pvTable[0][0]) << std::endl;
	LOG_INFO("bestmove " + algebraicNotation(pvTable[0][0]));

	ageHistory();

	assert((searchPly == 0) && "iterativeDeepening: searchPly too small");
	assert((generateHashKey() == hashKey) && "iterativeDeepening: hashKey is wrong illegal move");
}