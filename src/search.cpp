//
// Created by Federico Saitta on 04/07/2024.
//
#include <iostream>
#include <string>

#include "globals.h"
#include "constants.h"
#include "inline_functions.h"
#include "evaluation.h"

static int ply{}; //half move counter
static int bestMove{};

// can it even be inline if its recursive???
static int negamax(int alpha, int beta, const int depth) {

    if (depth == 0) {
        nodes++;
        return evaluate();
    }


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

        const int score = -negamax(-beta, -alpha, depth-1);
        restoreBoard()
        ply--;

        // fail-hard beta cut off
        if (score >= beta) return beta; // known as node that fails high

        // found a better move
        if (score > alpha) { // Known as PV node (principal variatio)
            alpha = score;

            if (ply == 0) bestMoveSoFar = moveList.moves[count];
        }
    }

    if (oldAlpha != alpha) bestMove = bestMoveSoFar;

    return alpha; // known as fail-low node
}


void searchPosition(const int depth) {
    constexpr int alpha{-50'000};
    constexpr int beta{50'000};

    nodes = 0; // the global variable, similar to perft

    const auto start = std::chrono::high_resolution_clock::now();

    int score = negamax(alpha, beta, depth);

    const std::chrono::duration<float> duration = std::chrono::high_resolution_clock::now() - start;

    std::string move { chessBoard[getMoveStartSQ(bestMove)] };
    std::string movetwo { chessBoard[getMoveTargettSQ(bestMove)]};
    std::string promotedPiece{ promotedPieces[getMovePromPiece(bestMove)]};
    std::cout << "bestmove " + move + movetwo + promotedPiece << '\n';
    std::cout << "Eval: " << static_cast<float>(score) / 100 << '\n';
    std::cout << "Info Nodes: " << nodes << '\n';
    std::cout << "Info MNodes/s: " << nodes / (duration.count() * 1'000'000) << '\n';
}
