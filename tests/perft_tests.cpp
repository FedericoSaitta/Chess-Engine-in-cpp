//
// Created by Federico Saitta on 03/07/2024.
//
#include "tests.h"

#include <iostream>
#include <random>
#include <cmath>
#include <chrono>

#include "globals.h"
#include "constants.h"
#include "inline_functions.h"
#include "misc.h"

static const std::string testFEN[] {
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
    "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",
    "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1",
    "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1",
    "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8",
    "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10 "};

static constexpr std::uint32_t testNodes[] {
    4'865'609, 193'690'690, 674'624, 15'833'292, 89'941'194, 164'075'551};

namespace Test{

    void perftDriver(const int depth) {

        if (depth == 0) {
            nodes++;
            return;
        }

        MoveList moveList{};
        generateMoves(moveList, 0);

        for (int moveCount = 0; moveCount < moveList.count; moveCount++)
        {
            copyBoard();

            if (!makeMove(moveList.moves[moveCount], 0)) continue;

            perftDriver(depth - 1);

            restoreBoard();
        }
    }

    std::uint32_t perft(const std::string& fenString, const int depth) {
        parseFEN(fenString);
        nodes = 0;

        const auto start = std::chrono::high_resolution_clock::now();

        MoveList moveList{ };
        generateMoves(moveList, 0);

        for (int moveCount = 0;  moveCount < moveList.count; moveCount++) {
            copyBoard();

            if (!makeMove(moveList.moves[moveCount], 0)) continue;

            const std::uint32_t cumulativeNodes {nodes};

            perftDriver(depth - 1);

            const std::uint32_t oldNodes {nodes - cumulativeNodes};

            // take back
            restoreBoard();

            // Print parent moves for debugging purposes
            //printf("     move: %s%s%c  nodes: %ld\n", chessBoard[getMoveStartSQ(moveList.moves[moveCount])],
            // chessBoard[getMoveTargettSQ(moveList.moves[moveCount])],
            // promotedPieces[getMovePromPiece(moveList.moves[moveCount])],
            // oldNodes);
        }

        const std::chrono::duration<float> duration = std::chrono::high_resolution_clock::now() - start;

        // print results
        printf("\nDepth: %d", depth);
        std::cout << " Nodes: " << nodes;
        std::cout << " Time: " << duration;
        std::cout << " Nodes/s: " << nodes / (duration.count() * 1'000'000) << " MNodes/s";

        return nodes;
    }

    // prints in red tests that have not passed
    void standardizedPerft() {


        for (int i=0; i < 6; i++) {

            if ( perft(testFEN[i], 5) == testNodes[i] ) {
                std::cout << " FEN: " << testFEN[i] << '\n';

            } else {
                std::cerr << " FEN: " << testFEN[i] << '\n';
            }

        }

    }
}