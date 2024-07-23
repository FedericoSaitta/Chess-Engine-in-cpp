//
// Created by Federico Saitta on 04/07/2024.
//
#include <iostream>
#include <fstream>
#include <string>
#include <variant>

#include "../include/macros.h"
#include "../include/inline_functions.h"

#include "search.h"

#include <assert.h>
#include "board.h"

#include "../movegen/update.h"
#include "../include/hashtable.h"
#include "../include/uci.h"
#include "../include/board.h"
#include "../eval/evaluation.h"
#include "../include/misc.h"
#include "movesort.h"

#define DO_NULL 1
#define NO_NULL 0

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

static int LMR_table[MAX_PLY][MAX_PLY];
static int LMP_table[2][MAX_PLY];

constexpr int fullDepthMoves { 4 }; // searching the first 4 moves at the full depth
constexpr int reductionLimit { 3 };

constexpr int windowWidth{ 50 }; // the aspritation window, the width is 100

static int stopSearch { 0 };
static int timePerMove { 0 };

constexpr int maxHistoryScore{ 1'600 };

auto startSearchTime = std::chrono::high_resolution_clock::now();
std::chrono::duration<float> searchDuration{ 0 };


void initSearchTables() {
	constexpr float base = 75 / 100.0f;
	constexpr float division = 300 / 100.0f;
		for(int depth = 1; depth < MAX_PLY; depth++) {
			for(int played = 1; played < 64; played++) {
				LMR_table[depth][played] = base + log(depth) * log(played) / division; // formula from Berserk engine
			}
		}
	LMR_table[0][0] = LMR_table[1][0] =  LMR_table[0][1] = 0;

	// Implement this once you have got improving heuristic done
	// from Berserk chess engine
	for(int depth = 1; depth < 64; depth++) {
		LMP_table[0][depth] = 2.5 +  2.5 * depth * depth / 4.5;
		LMP_table[1][depth] = 6.0 +  4.0 * depth * depth / 4.5;
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
			historyMoves[a][b] = std::min(maxHistoryScore, historyMoves[a][b] / 8);
		}
	}
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

	// We give 100 millisecond lag compensation
	const int timeAlloted = (board.side == WHITE) ? whiteClockTime - 100 : blackClockTime - 100;
	const int increment = (board.side == WHITE) ? whiteIncrementTime : blackIncrementTime;

	int timePerMove{ timeAlloted / 30 + increment };
	if ( (increment > 0) && (timeAlloted < (5 * increment) ) ) timePerMove = (0.75 * increment);

	return timePerMove;
}
static void isTimeUp() {
	searchDuration = std::chrono::high_resolution_clock::now() - startSearchTime;

	if ( (searchDuration.count() * 1'000) > timePerMove) stopSearch = 1;
}
static int isRepetition() {
	for (int index=repetitionIndex - 1; index >= 0; index-= 2) {
		// looping backwards
		if (repetitionTable[index] == hashKey)
			return 1; // repetition found
	}
	return 0; // no repetition
}

static U64 nonPawnMaterial() {
	return ( board.bitboards[QUEEN + 6 * board.side] | board.bitboards[ROOK + 6 * board.side] | board.bitboards[BISHOP + 6 * board.side] | board.bitboards[KNIGHT + 6 * board.side]);
}

static int givesCheck(const int move) {
	// Returns true if the current move puts the opponent in check
	COPY_BOARD()
	makeMove(move, 0);

	const int opponentInCheck { isSqAttacked( bsf(board.bitboards[KING + 6 * board.side]) , board.side^1 ) };

	RESTORE_BOARD()
	return opponentInCheck;
}


static int quiescenceSearch(int alpha, const int beta) {

	if ((nodes & 4095) == 0) isTimeUp();

	nodes++;

    if ( ply > (MAX_PLY - 1) ) return evaluate();

    const int standPat{ evaluate() };

	// delta pruning
	if (standPat < alpha - 975) return alpha;

	if (standPat >= beta) return standPat; // fail soft

    if (standPat > alpha) {
	    alpha = standPat; // Known as PV node (principal variation)
    }

    MoveList moveList;
    generateMoves(moveList);
    sortMoves(moveList, 0);

	int bestEval { standPat };

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

static void updateKillersAndHistory(const int bestMove, const int depth) {
	killerMoves[1][ply] = killerMoves[0][ply];
	killerMoves[0][ply] = bestMove; // store killer moves

	// can do more sophisticated code tho, not giving maluses for now
	historyMoves[getMovePiece(bestMove)][getMoveTargetSQ(bestMove)] += depth * depth;
}

static void makeNullMove() {
	hashKey ^= sideKey;
	if (board.enPassantSq != 64) hashKey ^= randomEnPassantKeys[board.enPassantSq];

	board.side ^= 1; // make null move
	board.enPassantSq = 64; // resetting en-passant to null-square

	// we change plies so white and black killers remain in sync for negamax search
	ply++;
	repetitionIndex++;
	repetitionTable[repetitionIndex] = hashKey;
}

static void undoNullMove() {
	ply--;
	repetitionIndex--;
}

static int negamax(int alpha, const int beta, int depth, const int canNull) {

	pvLength[ply] = ply;
	int score{};
	int bestMove {};
	int bestEval {-INF - 1};

	if (ply && isRepetition()) return 0; // we return draw score if we detect a three-fold repetition


	// ply && used to ensure we dont read from the transposition table at the root node
	const bool pvNode = (beta - alpha) > 1; // Trick used to find if current node is pvNode

	// reading the TT table, if we the move has already been searched, we return its evaluation
	score = probeHash(alpha, beta, &bestMove, depth);
	const bool ttHit = score != NO_HASH_ENTRY;
	if (ply && ttHit && !pvNode) return score;


	if ((nodes & 4095) == 0) isTimeUp();
	if (ply > MAX_PLY - 1) return evaluate();

	if ( depth < 1 ) return quiescenceSearch(alpha, beta);

	nodes++;

	const int inCheck{ isSqAttacked( bsf(board.bitboards[KING + 6 * board.side]), board.side^1) };

	int legalMoves{};

	// Search extension if board.side is in check
	if (inCheck) depth++;

	// STATIC NULL MOVE PRUNING / REVERSE FUTILITY PRUNING
	if (depth < 3 && !pvNode && !inCheck &&  std::abs(beta - 1) > -INF + 100)
	{
		// new addition avoid having to re-evaluate if we already have a tt eval
		const int staticEval{ (ttHit) ? score : evaluate() };
		// evaluation margin substracted from static evaluation score fails high
		if (staticEval - (120 * depth) >= beta)
			// evaluation margin substracted from static evaluation score
			return staticEval - 120 * depth;
	}

	if (!pvNode && !inCheck && ply) {

		/* WHATEVER THIS IS IM REMOVING IT FOR NOW, DO THIS AS V8 AND TEST VS V7*/
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
			const int R = 3 + depth / 3;
			const int nullMoveScore = -negamax(-beta, -beta + 1, depth - R, NO_NULL);
			undoNullMove();

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

    MoveList moveList;
    generateMoves(moveList);

	// if we are following PV line, we enable scoring
    if (followPV) enablePVscoring(moveList);

    sortMoves(moveList, bestMove);

	int movesSearched{};

	//MoveList quietList;
	int quietMoveCount{};
	bool skipQuietMoves{ false };

	const int originalAlpha {alpha};

    for (int count=0; count < moveList.count; count++) {
    	const int move { moveList.moves[count] };
    	// enPassant is already a capture so this also considers en-Passant as non-quiet moves
    	const bool isQuiet = ( !getMoveCapture(move) && !getMovePromPiece(move));

    	if (isQuiet && skipQuietMoves) continue;

    	if (ply && !inCheck && isQuiet && alpha > -MATE_SCORE) {
    		//Late move pruning (LMP)

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
    	if (isQuiet) quietMoveCount++; // we only add to counter if the quiet move is actually legal


    	// LMR from https://web.archive.org/web/20150212051846/http://www.glaurungchess.com/lmr.html
    	if(movesSearched == 0) {
    		// First move, use full-window search // this is the principal variation move
    		score = -negamax(-beta, -alpha, depth-1, DO_NULL);
    	} else {
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

        		pvTable[ply][ply] = moveList.moves[count];
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

	const int softTimeLimit = timePerMove / 3.0f;
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
    std::cout << "bestmove " + algebraicNotation(pvTable[0][0]) << '\n';

	// Post searching cleanups that can be done during the opponent's turn
	ageHistoryTable();
}