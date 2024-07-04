//
// Created by Federico Saitta on 04/07/2024.
//
#include <iostream>
#include <fstream>
#include <string>

#include "globals.h"
#include "constants.h"
#include "inline_functions.h"
#include "evaluation.h"

static int ply{}; //half move counter
static int bestMove{};

static int quiescenceSearch(int alpha, const int beta) {

    const int staticEval{ evaluate() };

    if (staticEval >= beta) return beta; // known as node that fails high

    // found a better move
    if (staticEval > alpha) { // Known as PV node (principal variatio)
        alpha = staticEval;
    }

    MoveList moveList;
    generateMoves(moveList, 0); // second parameter not acc used

    for (int count=0; count < moveList.count; count++) {
        copyBoard()
        ply++;

        // makeMove returns 1 for legal moves, we only want to look at captures
        if( !makeMove(moveList.moves[count], 1) ) { // meaning its illegal or its not a capture
            ply--;
            continue;
        }

        const int score = -quiescenceSearch(-beta, -alpha);
        ply--;
        restoreBoard()

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

static int negamax(int alpha, const int beta, const int depth) {

    if (depth == 0) return quiescenceSearch(alpha, beta);

    nodes++;

    int inCheck{ isSqAttacked( (side == White) ? getLeastSigBitIndex(bitboards[King]) : getLeastSigBitIndex(bitboards[King + 6]), side^1) };
    int legalMoves{};

    int bestMoveSoFar{};
    const int oldAlpha{ alpha };

    MoveList moveList;
    generateMoves(moveList, 0);

    for (int count=0; count < moveList.count; count++) {

        copyBoard()
        ply++;

        // makeMove returns 1 for legal moves
        if( !makeMove(moveList.moves[count], 0) ) { // meaning its illegal
            ply--;
            continue;
        }
        // increment legalMoves
        legalMoves++;

        int score = -negamax(-beta, -alpha, depth-1);
        ply--;
        restoreBoard()


        // fail-hard beta cut off
        if (score >= beta) return beta; // known as node that fails high

        // found a better move
        if (score > alpha) { // Known as PV node (principal variatio)
            alpha = score;

            // ply is used to know if we are at the parent node (root node)
            if (ply == 0) bestMoveSoFar = moveList.moves[count];
        }
    }
    if (legalMoves == 0) {
        if (inCheck) {
            return -49'000 + ply; // we want to return the mating score, (slightly above negative infinity, +ply scores faster mates as better moves)
        }
        return 0; // we are in stalemate
    }

    if (oldAlpha != alpha) bestMove = bestMoveSoFar;

    return alpha; // known as fail-low node
}


void searchPosition(const int depth){

    nodes = 0; // the global variable, similar to perft

    const auto start = std::chrono::high_resolution_clock::now();

    int score = negamax(-50'000, 50'000, depth);

    const std::chrono::duration<float> duration = std::chrono::high_resolution_clock::now() - start;

    std::string move { chessBoard[getMoveStartSQ(bestMove)] };
    std::string movetwo { chessBoard[getMoveTargettSQ(bestMove)]};
    std::string promotedPiece{ promotedPieces[getMovePromPiece(bestMove)]};

    std::ofstream logFile("/Users/federicosaitta/CLionProjects/ChessEngine/logfile.txt", std::ios::app);

    logFile << "info score cp " << score << " depth " << depth << " nodes " << nodes << " MNodes/s " << (nodes / (duration.count() * 1'000) ) << '\n';
    std::cout << "bestmove " + move + movetwo + promotedPiece << '\n';

    logFile.close();

  //   std::cout << "Info MNodes/s: " << nodes / (duration.count() * 1'000'000) << '\n';
}
