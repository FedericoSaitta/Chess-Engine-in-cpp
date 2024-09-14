/* SEARCH DETAILS
 * In negamax we consider noisy: promotions and captures (en-passant too)
 *   - so killer moves are never promotions
 *
 * In quiesce we search: captures and queen promotions (promotion captures are all searched)
 *
 *
 *
 * NOTES:
 * - SEE should always be done before making the actual move

 *
 */

#include <iostream>
#include <fstream>
#include <string>

#include <cstdlib>
#include <algorithm>

#include "game_statistics.h"

#include "config.h"

#include "search.h"

#include <assert.h>
#include <cstdint>
#include <cmath>

#include "../../chess/board.h"


#include "../include/hashtable.h"
#include "timer.h"

#include "../eval/evaluation.h"
#include "../../chess/misc.h"
#include "see.h"
#include "../logger/logger.h"

// SEARCH PARAMETERS //
constexpr double LMR_BASE = 0.79;
constexpr double LMR_DIVISION = 2.87;

static int LMR_table[MAX_PLY][MAX_PLY];
int nodes{};

void initSearchTables() {
		for(int depth = 1; depth < MAX_PLY; depth++) {
			for(int played = 1; played < 64; played++) {
				LMR_table[depth][played] = static_cast<int>( LMR_BASE + std::log(depth) * std::log(played) / LMR_DIVISION ); // formula from Berserk engine
			}
		}
	LMR_table[0][0] = LMR_table[1][0] =  LMR_table[0][1] = 0;
}

void Searcher::iterativeDeepening(const int maxDepth, const bool timeConstraint) {
	calculateMoveTime(timeConstraint);

	resetSearchStates();
	searchTimer.reset();

	Move previousBestMove { Move::Null };
	int score{}, avgScore{ -100'000 };
	int bestMoveStabilityFactor{}, evalStabilityFactor{};

	for (int depth = 1; depth <= maxDepth; depth++) {
		nodes = 0;
		followPV = 1;

		if (stopSearch) break;
		if (timeConstraint && searchTimer.elapsed() > softBoundTime) break;

		singleDepthTimer.reset();

		score = aspirationWindow(depth, score);
		avgScore = (avgScore == -100'000) ? score : (avgScore + score) / 2;

		// checking after the search to prevent from printing empty pv string
		if (stopSearch) break;

		// we scale the time control
		if (previousBestMove == pvTable[0][0]){
			bestMoveStabilityFactor = std::min(bestMoveStabilityFactor + 1, 4);
		} else {
			previousBestMove = pvTable[0][0];
			bestMoveStabilityFactor = 0;
		}

		// Keep track of eval stability
		if (score > avgScore - 10 && score < avgScore + 10) {
			evalStabilityFactor = std::min(evalStabilityFactor + 1, 4);
		}
		else {
			evalStabilityFactor = 0;
		}

		if (depth > 7) {
			scaleTimeControl(bestMoveStabilityFactor, evalStabilityFactor);
		}

		sendUciInfo(score, depth, nodes);
	}
	std::cout << "bestmove " << pvTable[0][0] << std::endl;
	LOG_INFO("bestmove " + algebraicNotation(pvTable[0][0]) );

	assert((searchPly == 0) && "iterativeDeepening: searchPly too small");
	assert((generateHashKey(pos) == hashKey) && "iterativeDeepening: hashKey is wrong illegal move");
}


int Searcher::aspirationWindow(const int currentDepth, const int previousScore) {
	int alpha, beta, score;
	int delta { this->ASP_WINDOW_WIDTH };

	if (currentDepth > 3) { // use aspiration window
		alpha = previousScore - delta;
		beta = previousScore + delta;
	} else {
		alpha = -INF;
		beta = INF;
	}

	while (true) {
		if ((nodes & 4095) == 0) isTimeUp();
		if (stopSearch) return 0;

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


int Searcher::negamax(int alpha, const int beta, int depth, const NodeType canNull) {
	assert(depth >= 0 && "negamax: depth is negative");
	const bool pvNode { beta - alpha > 1 }; // Trick used to find if current node is pvNode

	pvLength[searchPly] = searchPly;

	Move bestMove {}; // for now as tt is turned off this is just a null move
	int bestEval {-INF - 1};

	// we return draw score if we detect a three-fold repetition or insufficient material draw
	if (searchPly && isRepetition() ) return 0;

	// reading the TT table, if we the move has already been searched, we return its evaluation
	// ply && used to ensure we dont read from the transposition table at the root node
	int score { probeHash(alpha, beta, &bestMove, depth, searchPly, pos.hashkey) };
	const bool ttHit { score != NO_HASH_ENTRY };

	if (searchPly && ttHit && !pvNode) return score;
	if ((nodes & 4095) == 0) isTimeUp();
	if (searchPly > MAX_PLY - 1) return evaluate(pos);
	// Very first step is QS search.
	if ( depth < 1 ) return quiescenceSearch(alpha, beta); // pv here

	const int inCheck{ pos.currentlyInCheck() };
	if (inCheck) depth++; // Search extension if pos.side is in check

	// quiet move pruning
	// most of these can be greatly improved with improving heuristic
	if (!pvNode && !inCheck && searchPly) {

		// reverse futility pruning
		const int eval { ttHit ? score : evaluate(pos) };
		if (depth < this->RFP_DEPTH && (eval - depth * this->RFP_MARGIN) >= beta)
			return eval;


		// NULL MOVE PRUNING: https://web.archive.org/web/20071031095933/http://www.brucemo.com/compchess/programming/nullmove.htm
		// Do not attempt null move pruning in case our pos.side only has pawns on the pos
		// maybe you need a flag to make sure you dont re-attempt null move twice in a row?
		// no NULL flag used to ensure we dont do two null moves in a row
		if (depth > this->NMP_DEPTH  && canNull && pos.nonPawnMaterial()) {
			const Board copyBoard = pos;
			pos.nullMove();

			searchPly++;
			repetitionIndex++;
			repetitionTable[repetitionIndex] = pos.hashkey;

			// more aggressive reduction
			const int r = std::min(static_cast<int>( (this->NMP_BASE/100.0) + depth / (this->NMP_DIVISION/100.0) ), depth);
			const int nullMoveScore { -negamax(-beta, -beta + 1, depth - r, DONT_NULL) };

			searchPly--;
			repetitionIndex--;
			pos = copyBoard;

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
	    const std::pair scoredPair { pickBestMove(moveList, count ) };
    	const Move move { scoredPair.first };

    	// En-passant, captures and promotions are noisy
    	const bool isQuiet = ( !move.isNoisy() );

    	if (isQuiet && skipQuiets) continue;

    	//#### LMP with CARP parameters, it can be improved with improving heurisitc ####//
    	if (searchPly && !inCheck && isQuiet && bestEval > -MATE_SCORE) {
    		if (!pvNode && depth <= this->LMP_DEPTH && quietMoveCount >= (this->LMP_MULTIPLIER + depth * depth)) {
    			skipQuiets= true;
    			continue;
    		}
    	}

    	//#### PVS SEE Pruning ####//
    	if (bestEval > -MATE_SCORE && depth <= this->SEE_PRUNING_DEPTH) {
    		// CAPTURE MOVES
    		if (move.isCapture()) {
    			if ( !see(move, depth * depth * (this->SEE_CAPTURE_MARGIN), pos) ) {
    				continue;
    			}
    		}

    		// QUIET MOVES
    		else {
    			if ( !see(move, depth * depth * (this->SEE_QUIET_MARGIN), pos) ) {
    				continue;
    				}
    		}

    	}

    	const Board copyBoard = pos;

	    searchPly++;
    	repetitionIndex++;
    	repetitionTable[repetitionIndex] = pos.hashkey;

        // Illegal Moves
        if( !pos.makeMove(move, 0) ) {
            searchPly--;
        	repetitionIndex--;

        	assert((searchPly >= 0) && "negamax: searchPly too small");
        	assert((repetitionIndex >= 0) && "negamax: repetition index too small");
        	assert((generateHashKey(pos) == hashKey) && "negamax: hashKey is wrong illegal move");
            continue;
        }

        legalMoves++;


    	//#### LATE MOVE REDUCTION (LMR) ####//
    	if(movesSearched == 0) {
    		// First move is always searched fully
    		score = -negamax(-beta, -alpha, depth-1, DO_NULL);
    	} else {
    		// do not reduce noisy moves
    		if( (movesSearched >= this->LMR_MIN_MOVES) && (depth >= this->LMR_DEPTH) && isQuiet ) {

    			//int reduction = ( (this->LMR_BASE / 100.0) + std::log(depth) * std::log(count) / (this->LMR_DIVISION / 100.0) );
    			int reduction = LMR_table[std::min(depth, MAX_PLY)][std::min(count, MAX_PLY)];

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
    	pos = copyBoard;

    	movesSearched++;

    	if (stopSearch) return 0;

    	//#### FAIL SOFT FRAMEWORK ####//
    	if (score > bestEval) {
    		bestEval = score;
    		bestMove = move;

    		if (score > alpha){
    			alpha = score;


    			pvTable[searchPly][searchPly] = move;
    			// copy move from deeper plies to curernt ply
    			for (int nextPly = (searchPly+1); nextPly < pvLength[searchPly + 1]; nextPly++) {
    				pvTable[searchPly][nextPly] = pvTable[searchPly + 1][nextPly];
    			}

    			pvLength[searchPly] = pvLength[searchPly + 1];

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

    	// By putting this after beta cut-off we avoid storing bestMove in the quiets array
    	// so we do not need to check the moves we apply a malus to
    	if (isQuiet && (quietMoveCount < 31) ) {
    		quiets[quietMoveCount] = move;
    		quietMoveCount++;
    	}

    }

	// Checking if we are in stalemate or check-mate
	if (legalMoves == 0) {
		if (inCheck) {
			return -MATE_VALUE + searchPly; // adding searchPly ensures that faster mates are chosen over slower ones
		}
		return 0; // we are in stalemate
	}

	int hashFlag = HASH_FLAG_EXACT;
	if (alpha >= beta) hashFlag = HASH_FLAG_BETA; // beta cutoff, fail high
	else if (alpha <= originalAlpha) hashFlag = HASH_FLAG_ALPHA; // failed to raise alpha, fail low

	if (bestEval != (-INF - 1)) recordHash(bestEval, bestMove, hashFlag, depth, searchPly, pos.hashkey);

	return bestEval;
}


int Searcher::quiescenceSearch(int alpha, const int beta) {

	Move bestMove;
	if ((nodes & 4095) == 0) isTimeUp();
	if (stopSearch) return 0; // If the time is up, we return 0;
	if ( searchPly > (MAX_PLY - 1) ) return evaluate(pos);
	//if (searchPly && ( isRepetition() || pos.isInsufficientMaterial()) ) return 0;

	const int standPat{ evaluate(pos) };

	// delta pruning
	if (standPat < (alpha - 975) ) return alpha;

	if (standPat >= beta) return standPat; // fail soft

	if (standPat > alpha) {
		alpha = standPat; // Known as PV node (principal variation)
	}

	const bool pvNode = (beta - alpha) > 1;

	// there is something wrong here with TT table
	const int value { probeHash(alpha, beta, &bestMove, 0, searchPly, pos.hashkey) };
	const bool ttHit { value != NO_HASH_ENTRY };
	if (searchPly && ttHit && !pvNode) return value;

	MoveList moveList;
	pos.generateMoves(moveList);

	giveScores(moveList, Move::Null, pos); // as there is no bestmove

	const int originalAlpha {alpha};
	int bestEval { standPat };

	for (int count=0; count < moveList.count; count++) {
		const std::pair scoredPair { pickBestMove(moveList, count ) };
		const Move move { scoredPair.first };

		// QS SEE Pruning, only prune loosing captures or loosing queen promotions
		if ( (move.isCapture() || move.isQueenPromotion() ) && !see(move, this->SEE_QS_THRESHOLD, pos)) {
			continue; // very basic SEE for now
		}

		const Board copyBoard = pos;
		searchPly++;
		repetitionIndex++;
		repetitionTable[repetitionIndex] = pos.hashkey;

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
		pos = copyBoard;

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

	if (bestEval != standPat) recordHash(bestEval, bestMove, hashFlag, 0, searchPly, pos.hashkey);

	return bestEval; // node that fails low
}
