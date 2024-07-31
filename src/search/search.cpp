//
// Created by Federico Saitta on 04/07/2024.
//
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>

#include "../include/macros.h"
#include "../include/inline_functions.h"

#include "search.h"
#include "searchparams.h"

#include <assert.h>
#include <cstdint>
#include <cmath>
#include "board.h"

#include "../movegen/update.h"

#include "../movegen/update.h"
#include "../include/hashtable.h"
#include "../include/uci.h"
#include "../include/board.h"
#include "../eval/evaluation.h"
#include "../include/misc.h"
#include "movesort.h"

U64 repetitionTable[1'000]{};
int repetitionIndex{};

int ply{};
static std::int64_t nodes{};

Move killerMoves[2][128]{}; // zero initialization to ensure no random bonuses to moves
int historyScores[12][64]{}; // zero initialization to ensure no random bonuses to moves

Move pvTable[64][64]{};
static int pvLength[64]{};

int scorePV{};
static int followPV{}; // if it is true then we follow the principal variation

static int LMR_table[MAX_PLY][MAX_PLY];
static int LMP_table[2][MAX_PLY];


auto startSearchTime = std::chrono::steady_clock::now();
std::chrono::duration<double> searchDuration{ 0.0 };

void initSearchTables() {
	constexpr std::double_t base = 75.0 / 100.0;
	constexpr std::double_t division = 300.0 / 100.0;
		for(int depth = 1; depth < MAX_PLY; depth++) {
			for(int played = 1; played < 64; played++) {
				LMR_table[depth][played] = static_cast<int>( base + std::log(depth) * std::log(played) / division ); // formula from Berserk engine
			}
		}
	LMR_table[0][0] = LMR_table[1][0] =  LMR_table[0][1] = 0;

	// Implement this once you have got improving heuristic done
	// from Berserk chess engine
	for(int depth = 1; depth < 64; depth++) {
		LMP_table[0][depth] = static_cast<int>( 2.5 +  2.5 * depth * depth / 4.5 );
		LMP_table[1][depth] = static_cast<int>( 6.0 +  4.0 * depth * depth / 4.5 );
	}
}
static void resetSearchStates() {
	memset(killerMoves, 0, sizeof(killerMoves));
	memset(pvLength, 0, sizeof(pvLength));
	memset(pvTable, 0, sizeof(pvTable));

	nodes = 0;
	followPV = 0;
	scorePV = 0;
	ply = 0;

	stopSearch = 0;
}


static void ageHistoryTable() {
	for (int a=0; a < 12; a++) {
		for (int b=0; b<64; b++) {
			// make sure we dont go over the limit
			historyScores[a][b] = std::min(maxHistoryScore, static_cast<int>(historyScores[a][b] * historyAgeRatio) );
		}
	}
}

static void enablePVscoring(const MoveList& moveList) {
    followPV = 0;
    for (int count=0; count < moveList.count; count++) {
        if ( moveList.moves[count].first == pvTable[0][ply] ) {
            scorePV = 1; // if we do find a move
            followPV = 1; // we are in principal variation so we want to follow it
        	break; 
        }
    }
}

static int getMoveTime(const bool timeConstraint) {

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

	return moveTime;
}
static void isTimeUp() {
	searchDuration = std::chrono::high_resolution_clock::now() - startSearchTime;
	if ( (searchDuration.count() * 1'000) > timePerMove) stopSearch = 1;
}
static int isRepetition() {
	for (int index=repetitionIndex - 1; index >= 0; index-= 2) {
		// looping backwards over our previous keys
		if (repetitionTable[index] == hashKey)
			return 1; // repetition found
	}
	return 0; // no repetition
}

static U64 nonPawnMaterial() {
	return ( board.bitboards[QUEEN + 6 * board.side] | board.bitboards[ROOK + 6 * board.side] | board.bitboards[BISHOP + 6 * board.side] | board.bitboards[KNIGHT + 6 * board.side]);
}

static int givesCheck(const Move move) {
	// Returns true if the current move puts the opponent in check
	COPY_BOARD()
	board.makeMove(move, 0);

	const int opponentInCheck { isSqAttacked( bsf(board.bitboards[KING + 6 * board.side]) , board.side^1 ) };

	board.undo(move);
	RESTORE_BOARD()
	return opponentInCheck;
}


static int quiescenceSearch(int alpha, const int beta) {

	if ((nodes & 4095) == 0) isTimeUp();

	nodes++;

    if ( ply > (MAX_PLY - 1) ) return evaluate();

    const int standPat{ evaluate() };

	// delta pruning
	if (standPat < (alpha - 975) ) return alpha;

	if (standPat >= beta) return standPat; // fail soft

    if (standPat > alpha) {
	    alpha = standPat; // Known as PV node (principal variation)
    }

    MoveList moveList;
    generateMoves(moveList);

	const int totalMoves { moveList.count };
	giveScores(moveList, 0); // as there is no bestmove

	int bestEval { standPat };

    for (int count=0; count < totalMoves; count++) {
    	const Move move { pickBestMove(moveList, count ) };

    	COPY_BOARD()
        ply++;
    	repetitionIndex++;
    	repetitionTable[repetitionIndex] = hashKey;

        // board.undo Illegal Moves or non-captures
        if( !board.makeMove(move, 1) ) {
            ply--;
        	repetitionIndex--;
            continue;
        }

        const int score = -quiescenceSearch(-beta, -alpha);

        ply--;
    	repetitionIndex--;
    	board.undo(move);
        RESTORE_BOARD()

 		if (stopSearch) return 0; // If the time is up, we return 0;

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

static void updateKillersAndHistory(const Move bestMove, const int depth) {
//	if (killerMoves[0][ply] != bestMove) { // FROM WEISS CHESS ENGINE
		killerMoves[1][ply] = killerMoves[0][ply];
		killerMoves[0][ply] = bestMove; // store killer moves
//	}

	// can do more sophisticated code tho, not giving maluses for now
	historyScores[board.mailbox[bestMove.from()]][bestMove.to()] += (depth * depth);
}

static void makeNullMove() {
	hashKey ^= sideKey;
	if (board.history[board.gamePly].enPassSq != 64) hashKey ^= randomEnPassantKeys[board.history[board.gamePly].enPassSq];

	board.side ^= 1; // make null move
	board.history[board.gamePly].enPassSq = 64; // resetting en-passant to null-square

	// we change plies so white and black killers remain in sync for negamax search
	ply++;
	repetitionIndex++;
	repetitionTable[repetitionIndex] = hashKey;
}

static void undoNullMove() {
	ply--;
	repetitionIndex--;
}

static int negamax(int alpha, const int beta, int depth, const NodeType canNull) {

	pvLength[ply] = ply;
	Move bestMove {};
	int bestEval {-INF - 1};

	if (ply && isRepetition()) return 0; // we return draw score if we detect a three-fold repetition

	const bool pvNode = (beta - alpha) > 1; // Trick used to find if current node is pvNode

	// reading the TT table, if we the move has already been searched, we return its evaluation
	// ply && used to ensure we dont read from the transposition table at the root node
	int score = probeHash(alpha, beta, &bestMove, depth);
	const bool ttHit = score != NO_HASH_ENTRY;
	if (ply && ttHit && !pvNode) return score;


	if ((nodes & 4095) == 0) isTimeUp();
	if (ply > MAX_PLY - 1) return evaluate();
	if ( depth < 1 ) return quiescenceSearch(alpha, beta);

	nodes++;

	const int inCheck{ isSqAttacked( bsf(board.bitboards[KING + 6 * board.side]), board.side^1) };

	if (inCheck) depth++; // Search extension if board.side is in check

	// STATIC NULL MOVE PRUNING / REVERSE FUTILITY PRUNING
	if (depth < 3 && (!pvNode) && (!inCheck) &&  (std::abs(beta - 1) > (-INF + 100) ) )
	{
		// new addition avoid having to re-evaluate if we already have a tt eval
		const int staticEval{ (ttHit) ? score : evaluate() };
		// evaluation margin substracted from static evaluation score fails high
		if (staticEval - (120 * depth) >= beta)
			// evaluation margin substracted from static evaluation score
			return staticEval - 120 * depth;
	}


	/*
	if (!pvNode && !inCheck && ply) {
		 // WHATEVER THIS IS IM REMOVING IT FOR NOW, DO THIS AS V8 AND TEST VS V7
		// reverse futility pruning
		// gains Elo: -35.91 +/- 14.86, nElo: -52.43 +/- 21.53
		const int eval { ttHit ? score : evaluate() };
		if (depth < 9 && (eval - depth * 80) >= beta)
			return eval;

		// maybe you can write TT entries here too???
		// NULL MOVE PRUNING: https://web.archive.org/web/20071031095933/http://www.brucemo.com/compchess/programming/nullmove.htm
		// Do not attempt null move pruning in case our board.side only has pawns on the board
		// maybe you need a flag to make sure you dont re-attempt null move twice in a row?
		// no NULL flag used to ensure we dont do two null moves in a row
		if (depth >= 3 && nonPawnMaterial() && canNull) {
			COPY_BOARD()

			makeNullMove();

			// more aggressive reduction
			const int R = static_cast<int>(3 + depth / 3);
			const int nullMoveScore = -negamax(-beta, -beta + 1, depth - R, DONT_NULL);
			board.undoNullMove();

			RESTORE_BOARD() // un-making the null move

			if (stopSearch) return 0;
			if (nullMoveScore >= beta) return beta;
		}

		// razoring
		if (depth <= 3 && canNull){
			// get static eval and add first bonus
			score = evaluate() + 125;

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
	*/

    MoveList moveList;
    generateMoves(moveList);

    if (followPV) enablePVscoring(moveList); // check if we are in a principal-variation node

	const int totalMoves { moveList.count };
	const int originalAlpha {alpha};
	int legalMoves{};
	int movesSearched{};
	int quietMoveCount{};
	bool skipQuietMoves{ false };

	// score the moves before picking the best one
	giveScores(moveList, bestMove);

    for (int count=0; count < totalMoves; count++) {
    	const Move move { pickBestMove(moveList, count ) };

    	// En-passant are captures, so they are non-quiet
    	const bool isQuiet = ( !move.is_capture() && !move.is_promotion());

    	if (isQuiet && skipQuietMoves) continue;

    	// ****  LATE MOVE PRUNING (LMP) **** //
    	if (ply && !inCheck && isQuiet && alpha > -MATE_SCORE) {

    		// parameters obtained from CARP
    		if (!pvNode && depth <= 8 && quietMoveCount >= (4 + depth * depth)) {
    			skipQuietMoves= true;
    			continue;
    		}
    	}

        COPY_BOARD()
        ply++;
    	repetitionIndex++;
    	repetitionTable[repetitionIndex] = hashKey;

        // board.undo Illegal Moves
        if( !board.makeMove(move, 0) ) { // meaning its illegal
            ply--;
        	repetitionIndex--;
            continue;
        }

        legalMoves++;
    	if (isQuiet) quietMoveCount++; // we only add to counter if the quiet move is actually legal

    	// ****  LATE MOVE REDUCTION (LMR) **** //
    	if(movesSearched == 0) {
    		// https://web.archive.org/web/20150212051846/http://www.glaurungchess.com/lmr.html
    		// First move, use full-window search // this is the principal variation move
    		score = -negamax(-beta, -alpha, depth-1, DO_NULL);
    	}
    	else {
    		if( (movesSearched >= fullDepthMoves) && (depth >= reductionLimit)
    			&& isQuiet			// will reduce quiet moves
    			&& !inCheck         // will not reduce in case we are in check
    			&& !givesCheck(move)) { // maybe you should use this....

    			const int reduction = LMR_table[std::min(depth, 63)][std::min(count, 63)];

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

        ply--;
    	repetitionIndex--;
    	board.undo(move);
        RESTORE_BOARD()

    	if (stopSearch) return 0;

    	movesSearched++;

        // fail soft framework
        if (score > bestEval) {
	        // Known as PV node (principal variation)
        	bestEval = score;

        	if (score > alpha){
        		alpha = score;
        		bestMove = move; // store best move (for TT)

        		pvTable[ply][ply] = move;
        		// copy move from deeper plies to curernt ply
        		for (int nextPly = (ply+1); nextPly < pvLength[ply + 1]; nextPly++) {
        			pvTable[ply][nextPly] = pvTable[ply + 1][nextPly];
        		}

        		pvLength[ply] = pvLength[ply + 1];


        		// fail-hard beta cut off
        		if (alpha >= beta) {
        			// helps with better move ordering in branches at the same depth

        			if (isQuiet) {
        				updateKillersAndHistory(bestMove, depth);
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
            return -MATE_VALUE+ ply; // we want to return the mating score, (slightly above negative INF, +ply scores faster mates as better moves)
        }
        return 0; // we are in stalemate
    }

	int hashFlag = HASH_FLAG_EXACT;

	if (alpha >= beta)
	{
		// beta cutoff, fail high
		hashFlag = HASH_FLAG_BETA;
	}
	else if (alpha <= originalAlpha)
	{
		// failed to raise alpha, fail low
		hashFlag = HASH_FLAG_ALPHA;
	}

	if (bestEval != (-INF - 1)) recordHash(bestEval, bestMove, hashFlag, depth);

    return bestEval; // known as fail-low node
}



void iterativeDeepening(const int depth, const bool timeConstraint) {
	resetSearchStates();

	timePerMove = getMoveTime(timeConstraint);

	const int softTimeLimit = static_cast<int>(timePerMove / 3.0);
	assert( (timePerMove > 0) && "Negative Time Per Move");

	int alpha { -INF };
	int beta { INF };


	startSearchTime = std::chrono::high_resolution_clock::now();

	for (int currentDepth = 1; currentDepth <= depth; ){
        followPV = 1;

		if (stopSearch) break;
		searchDuration = std::chrono::high_resolution_clock::now() - startSearchTime;
		if ( (searchDuration.count() * 1'000) > softTimeLimit) {
		//	std::cout << "stopped by soft limit" << '\n';
		//	std::cout << softTimeLimit << '\n';
		//	std::cout << "time per move: " << timePerMove << '\n';
			break;
		}

        const auto startDepthTime = std::chrono::high_resolution_clock::now();

        const int score { negamax(alpha, beta, currentDepth, DO_NULL) };

		std::chrono::duration<float> depthDuration { std::chrono::high_resolution_clock::now() - startDepthTime };

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
			std::cout << "info score mate " << -(score + MATE_VALUE) / 2 - 1 << " depth " << currentDepth << " nodes " << nodes << " nps " << static_cast<std::int64_t>(nodes / depthDuration.count())
			<< " time " << static_cast<int>(depthDuration.count() * 1'000) << " pv " << pvString << '\n';
		} else if( score > MATE_SCORE && score < MATE_VALUE) {
			std::cout << "info score mate " << (MATE_VALUE - score) / 2 + 1 << " depth " << currentDepth << " nodes " << nodes << " nps " << static_cast<std::int64_t>(nodes / depthDuration.count())
			<< " time " << static_cast<int>(depthDuration.count() * 1'000) << " pv " << pvString << '\n';
		} else {
			std::cout << "info score cp " << score << " depth " << currentDepth << " nodes " << nodes << " nps " << static_cast<std::int64_t>(nodes / depthDuration.count())
			<< " time " << static_cast<int>(depthDuration.count() * 1'000) << " pv " << pvString << '\n';
		}
		currentDepth++; // we can proceed to the next iteration

    }
    std::cout << "bestmove " + algebraicNotation(pvTable[0][0]) << '\n';

	// Post searching cleanups that can be done during the opponent's turn
	ageHistoryTable();
}