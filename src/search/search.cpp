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


#include "../include/hashtable.h"
#include "timer.h"
#include "../include/uci.h"
#include "../eval/evaluation.h"
#include "../include/misc.h"
#include "movesort.h"
#include "see.h"
#include "../logger/logger.h"

static int LMR_table[MAX_PLY][MAX_PLY];
static int LMP_table[2][11];

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


void Searcher::resetSearchStates() {
	memset(killerMoves, 0, sizeof(killerMoves));
	memset(pvLength, 0, sizeof(pvLength));
	memset(pvTable, 0, sizeof(pvTable));

	nodes = 0;
	followPV = 0;
	scorePV = 0;
	searchPly = 0;

	stopSearch = 0;
}


void Searcher::enablePVscoring(const MoveList& moveList) {
    followPV = 0;

    for (int count=0; count < moveList.count; count++) {

        if ( moveList.moves[count].first == pvTable[0][searchPly] ) {
        	assert(!pvTable[0][searchPly].isNone() && "enablePVscoring: pv is following a null move");
        	assert(!moveList.moves[count].first.isNone() && "enablePVscoring: pv is following a null move");

            scorePV = 1; // if we do find a move
            followPV = 1; // we are in principal variation so we want to follow it
        	break;
        }
    }
}

int Searcher::getMoveTime(const bool timeConstraint, const int turn) {
	if (!timeConstraint) return 180'000; // maximum searching time of 3 minutes

	// We give 100 millisecond lag compensation
	const int timeAlloted = (turn == WHITE) ? whiteClockTime - 100 : blackClockTime - 100;
	const int increment = (turn == WHITE) ? whiteIncrementTime : blackIncrementTime;
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
void Searcher::isTimeUp() {
	if ( searchTimer.elapsed() > timePerMove) stopSearch = 1;
}

int Searcher::isRepetition() {
	// look if up until our repetition we have already encountered this position, asssuming the opponent
	// plays optimally they (just like us) will avoid repeting even once unless the position is drawn.
	for (int index=0; index < repetitionIndex; index+= 1) {
		// looping backwards over our previous keys
		if (repetitionTable[index] == hashKey) {
			return 1; // repetition found
		}
	}
	return 0; // no repetition
}

void Searcher::updateKillers(const Move bestMove) {

	// update killer moves if we found a new unique bestMove
	if (killerMoves[0][searchPly] != bestMove) {
		assert(!bestMove.isNone() && "updateKillers: bestMove is empty");

		killerMoves[1][searchPly] = killerMoves[0][searchPly];
		killerMoves[0][searchPly] = bestMove; // store killer moves
	}
}

bool Searcher::isKiller(const Move move) {
	assert(!move.isNone());
	if ( (move == killerMoves[0][searchPly]) || (move == killerMoves[1][searchPly]) ) return true;
	return false;
}

void Searcher::updateHistory(const Move bestMove, const int depth, const Move* quiets, const int quietMoveCount) {


	const int bonus = std::min(2100, 300 * depth - 300);
	assert(bestMove.from() < 64 && "updateHistory: out of bounds table indexing");
	assert(bestMove.to() < 64 && "updateHistory: out of bounds table indexing");

	// Bonus to the move that caused the beta cutoff
	if (depth > 2) {
		historyScores[bestMove.from()][bestMove.to()] += bonus - historyScores[bestMove.from()][bestMove.to()] * std::abs(bonus) / MAX_HISTORY_SCORE;

		assert( std::abs(historyScores[bestMove.from()][bestMove.to()]) <= MAX_HISTORY_SCORE && "updateHistory: history bonus is too large");
	}

	// Penalize quiet moves that failed to produce a cut only if bestMove is also quiet
	assert((quietMoveCount <= 32) && "updateHistory: quietMoveCount is too large");
	for (int i = 0; i < quietMoveCount; i++) {
		Move m = quiets[i];
		assert(!m.isNone() && "updatedHistory: in loop, move is noen");

		// Could i avoid this if check with double the bonus?
		// We do not want to cancel the bonus we just handed to the bestMove
		if (m != bestMove) {
			historyScores[m.from()][m.to()] += -bonus - historyScores[m.from()][m.to()] * std::abs(bonus) / MAX_HISTORY_SCORE;
			assert( std::abs(historyScores[m.from()][m.to()]) <= MAX_HISTORY_SCORE && "updateHistory: history bonus is too large");
		}
	}

}

int Searcher::quiescenceSearch(int alpha, const int beta) {

	Move bestMove;
	if ((nodes & 4095) == 0) isTimeUp();
	if (stopSearch) return 0; // If the time is up, we return 0;

	if ( searchPly > (MAX_PLY - 1) ) return evaluate(pos);
	const int standPat{ evaluate(pos) };

	// delta pruning
	if (standPat < (alpha - 975) ) return alpha;

	if (standPat >= beta) return standPat; // fail soft

	if (standPat > alpha) {
		alpha = standPat; // Known as PV node (principal variation)
	}

	bool pvNode = (beta - alpha) > 1;

	int value = probeHash(alpha, beta, &bestMove, 0, searchPly);
	const bool ttHit = value != NO_HASH_ENTRY;
	if (searchPly && ttHit && !pvNode) return value;

	MoveList moveList;
	pos.generateMoves(moveList);

	giveScores(moveList, Move(0, 0), pos); // as there is no bestmove

	const int originalAlpha {alpha};
	int bestEval { standPat };

	for (int count=0; count < moveList.count; count++) {
		const std::pair scoredPair { pickBestMove(moveList, count ) };
		const Move move { scoredPair.first };

		// QS SEE Pruning, only prune loosing captures, we dont want to prune promotions (non-capture promotions)
		// we should prune promotions too at one point and also the movepicker in qs just should never hand out non-noises
		if (move.isCapture() && !see(move, -105, pos)) continue; // very basic SEE for now

		COPY_HASH()
		searchPly++;
		repetitionIndex++;
		repetitionTable[repetitionIndex] = hashKey;

		// maybe switch to only looking at quiet moves instead of captures
		// pos.undo Illegal Moves or non-captures
		if( !pos.makeMove(move, 1) ) {
			searchPly--;
			repetitionIndex--;
			continue;
		}

		const int score = -quiescenceSearch(-beta, -alpha);

		searchPly--;
		repetitionIndex--;
		pos.undo(move);
		RESTORE_HASH()

		// found a better move
		if (score > bestEval) { // Known as PV node (principal variation)
			bestEval = score;
			bestMove = move; // saving the TT move

			if (score > alpha) {
				alpha = score;

				if (score >= beta) {
					break; // fail soft
				}
			}
		}
	}

	int hashFlag = HASH_FLAG_EXACT;
	if (alpha >= beta) hashFlag = HASH_FLAG_BETA; // beta cutoff, fail high
	else if (alpha <= originalAlpha) hashFlag = HASH_FLAG_ALPHA; // failed to raise alpha, fail low

	if (bestEval != standPat) recordHash(bestEval, bestMove, hashFlag, 0, searchPly);

	return bestEval; // node that fails low
}


int Searcher::negamax(int alpha, const int beta, int depth, const NodeType canNull) {
	assert(depth >= 0 && "negamax: depth is negative");
	pvLength[searchPly] = searchPly;
	Move bestMove {}; // for now as tt is turned off this is just a null move
	int bestEval {-INF - 1};

	if (searchPly && isRepetition()) return 0; // we return draw score if we detect a three-fold repetition

	const bool pvNode = (beta - alpha) > 1; // Trick used to find if current node is pvNode

	// reading the TT table, if we the move has already been searched, we return its evaluation
	// ply && used to ensure we dont read from the transposition table at the root node
	int score = probeHash(alpha, beta, &bestMove, depth, searchPly);
	const bool ttHit = score != NO_HASH_ENTRY;
	if (searchPly && ttHit && !pvNode) return score;

	if ((nodes & 4095) == 0) isTimeUp();
	if (searchPly > MAX_PLY - 1) return evaluate(pos);
	if ( depth < 1 ) return quiescenceSearch(alpha, beta);


	const int inCheck{ pos.currentlyInCheck() };
	if (inCheck) depth++; // Search extension if pos.side is in check

	// quiet move pruning
	// most of these can be greatly improved with improving heuristic
	if (!pvNode && !inCheck && searchPly) {

        /*
		// reverse futility pruning
		const int eval { ttHit ? score : evaluate(pos) };
		if (depth < RFP_DEPTH && (eval - depth * RFP_MARGIN) >= beta)
			return eval - depth * RFP_MARGIN;
			*/

		// reverse futility pruning
		const int eval { ttHit ? score : evaluate(pos) };
		if (depth < this->RFP_DEPTH && (eval - depth * this->RFP_MARGIN) >= beta)
			return eval;


		// NULL MOVE PRUNING: https://web.archive.org/web/20071031095933/http://www.brucemo.com/compchess/programming/nullmove.htm
		// Do not attempt null move pruning in case our pos.side only has pawns on the pos
		// maybe you need a flag to make sure you dont re-attempt null move twice in a row?
		// no NULL flag used to ensure we dont do two null moves in a row
		if (depth > this->NMP_DEPTH  && canNull && pos.nonPawnMaterial()) {
			COPY_HASH()
			pos.nullMove();

			searchPly++;
			repetitionIndex++;
			repetitionTable[repetitionIndex] = hashKey;

			// more aggressive reduction
			const int r = std::min(static_cast<int>( this->NMP_BASE + depth / this->NMP_DIVISION ), depth);
			const int nullMoveScore = -negamax(-beta, -beta + 1, depth - r, DONT_NULL);

			pos.undoNullMove();
			searchPly--;
			repetitionIndex--;
			RESTORE_HASH() // un-making the null move

			if (nullMoveScore >= beta) return beta;
		}

		// razoring
		if (depth <= 3 && canNull){
			// get static eval and add first bonus
			// as we need to preserve score for FP, we make a new variable here
			int r_score = evaluate(pos) + 125;

			int newScore; // define new score

			// static evaluation indicates a fail-low node
			if (r_score < beta)
			{
				// on depth 1
				if (depth == 1)
				{
					// get quiscence score
					newScore = quiescenceSearch(alpha, beta);

					// return quiescence score if it's greater then static evaluation score
					return (newScore > r_score) ? newScore : r_score;
				}

				// add second bonus to static evaluation
				r_score += 175;

				// static evaluation indicates a fail-low node
				if (r_score < beta && depth <= 2)
				{
					// get quiscence score
					newScore = quiescenceSearch(alpha, beta);

					// quiescence score indicates fail-low node
					if (newScore < beta)
						// return quiescence score if it's greater then static evaluation score
							return (newScore > r_score) ? newScore : r_score;
				}
			}
		}
	}

    MoveList moveList;
    pos.generateMoves(moveList);

    if (followPV) enablePVscoring(moveList); // check if we are in a principal-variation node

	const int originalAlpha {alpha};
	int legalMoves{};
	int movesSearched{};

	// score the moves before picking the best one
	giveScores(moveList, bestMove, pos);

	Move quiets[32];
	int quietMoveCount{};
	bool skipQuiets{ false };

    for (int count=0; count < moveList.count; count++) {
	    std::pair scoredPair { pickBestMove(moveList, count ) };
    	const Move move { scoredPair.first };

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


    	/*
    	// SEE pruning for captures only
    	if (bestEval > -MATE_SCORE
			&& depth <= 5) {

    		if (move.isCapture()) {
    			if ( !see(move, depth * (-100), pos) ) {
    				continue;
    			}
    		}

    		/*
    		else {
    			if ( !see(move, depth * depth * (- this->SEE_QUIET_MARGIN), pos) ) {
    				continue;
    				}
    		}
    		*/

    	//}



        COPY_HASH()
        searchPly++;
    	repetitionIndex++;
    	repetitionTable[repetitionIndex] = hashKey;

        // Illegal Moves
        if( !pos.makeMove(move, 0) ) {
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
    		if( (movesSearched >= this->LMR_MIN_MOVES) && (depth >= this->LMR_MIN_DEPTH) && isQuiet ) {

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
    	pos.undo(move);
        RESTORE_HASH()

    	if (stopSearch) return 0;

    	movesSearched++;

    	// fail soft framework
    	if (score > bestEval) {
    		// Known as PV node (principal variation)
    		bestEval = score;
    		bestMove = move; // saving the TT move

    		if (score > alpha){
    			alpha = score;

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

	if (bestEval != (-INF - 1)) recordHash(bestEval, bestMove, hashFlag, depth, searchPly);

	return bestEval; // known as fail-low node
}

int Searcher::aspirationWindow(const int currentDepth, const int previousScore) {
	int alpha;
	int beta;
	int score{};

	int delta { this->windowWidth };

	if (currentDepth > 3) { // use aspiration window
		alpha = previousScore - delta;
		beta = previousScore + delta;
	} else {
		alpha = -INF;
		beta = INF;
	}

	while (true) {
		// perform a search starting at the root node
		score = negamax(alpha, beta, currentDepth, DO_NULL);

		if (score > alpha && score < beta) break;

		// If we fell outside the window, we expand it and try again
		delta = delta + delta / 2;
		alpha = previousScore - delta;
		beta = previousScore + delta;

	}
	return score;
}

void Searcher::sendUciInfo(const int score, const int depth, const int nodes, const Timer& depthTimer) {
	// Extracting the PV line and printing out in the terminal and logging file
	std::string pvString{};
	for (int count = 0; count < pvLength[0]; count++) { pvString += algebraicNotation(pvTable[0][count]) + ' '; }

	const auto nps = static_cast<std::int64_t>(1'000 * nodes / depthTimer.elapsed());

	std::string scoreType = "cp";
	int adjustedScore = score;

	if (score > -MATE_VALUE && score < -MATE_SCORE) {
		scoreType = "mate";
		adjustedScore = -(score + MATE_VALUE) / 2 - 1;
	} else if (score > MATE_SCORE && score < MATE_VALUE) {
		scoreType = "mate";
		adjustedScore = (MATE_VALUE - score) / 2 + 1;
	}

	// Print the information
	std::cout << "info score " << scoreType << " " << adjustedScore
			  << " depth " << depth
			  << " nodes " << nodes
			  << " nps " << nps
			  << " time " << depthTimer.roundedElapsed()
			  << " pv " << pvString << std::endl;
}

void Searcher::iterativeDeepening(const int maxDepth, const bool timeConstraint) {
	resetSearchStates();
	// note that bench command will not change as we create a separate thread for it
	timePerMove = getMoveTime(timeConstraint, pos.side);
	const int softTimeLimit = static_cast<int>(timePerMove / 3.0);

	searchTimer.reset();

	int score{};

	for (int depth = 1; depth <= maxDepth; depth++){
		nodes = 0;
        followPV = 1;

		if (stopSearch) break;
		if (searchTimer.elapsed() > softTimeLimit) break;

        singleDepthTimer.reset();

        score = aspirationWindow(depth, score);

		// checking after the search to prevent from printing empty pv string
		if (stopSearch) break;

		sendUciInfo(score, depth, nodes, singleDepthTimer);
    }
    std::cout << "bestmove " + algebraicNotation(pvTable[0][0]) << std::endl;
	LOG_INFO("bestmove " + algebraicNotation(pvTable[0][0]));

	assert((searchPly == 0) && "iterativeDeepening: searchPly too small");
	assert((generateHashKey() == hashKey) && "iterativeDeepening: hashKey is wrong illegal move");
}
