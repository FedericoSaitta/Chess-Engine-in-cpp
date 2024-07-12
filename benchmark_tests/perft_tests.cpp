//
// Created by Federico Saitta on 03/07/2024.
//
#include "benchmark_tests.h"

#include <iostream>
#include <random>
#include <cmath>
#include <chrono>

#include "update.h"
#include "macros.h"
#include "hashtable.h"
#include "board.h"
#include "movegen.h"

static const std::string testFEN[] {
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
    "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",
    "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1",
    "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1",
    "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8",
    "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10 "};

static constexpr std::uint32_t testNodes[] {
    4'865'609, 193'690'690, 674'624, 15'833'292, 89'941'194, 164'075'551};

namespace Test::BenchMark{

    static std::uint32_t nodes{};

    void perftDriver(const int depth) {

        if (depth == 0) {
            nodes++;
            return;
        }

        MoveList moveList;
        generateMoves(moveList);

        for (int moveCount = 0; moveCount < moveList.count; moveCount++)
        {
            COPY_BOARD();

            if (!makeMove(moveList.moves[moveCount], 0)) continue;

            perftDriver(depth - 1);

            RESTORE_BOARD();
        }
    }

    std::uint32_t perft(const int depth) {
        nodes = 0;

        const auto start = std::chrono::high_resolution_clock::now();

        MoveList moveList;
        generateMoves(moveList);

        for (int moveCount = 0;  moveCount < moveList.count; moveCount++) {
            COPY_BOARD();

            if (!makeMove(moveList.moves[moveCount], 0)) continue;

            //const std::uint32_t cumulativeNodes {nodes};

            perftDriver(depth - 1);

            RESTORE_BOARD();
            // Print parent moves for debugging purposes
            //const std::uint32_t oldNodes {nodes - cumulativeNodes};
            //printf("     move: %s%s%c  nodes: %ld\n", chessBoard[getMoveStartSQ(moveList.moves[moveCount])],
            // chessBoard[getMoveTargettSQ(moveList.moves[moveCount])],
            // promotedPieces[getMovePromPiece(moveList.moves[moveCount])],
            // oldNodes);
        }

        const std::chrono::duration<float> duration = std::chrono::high_resolution_clock::now() - start;

        // print results
        printf("Depth: %d", depth);
        std::cout << " Nodes: " << nodes;
        std::cout << " Time: " << duration;
        std::cout << " MNodes/s: " << nodes / (duration.count() * 1'000'000);

        return nodes;
    }

    // prints in red benchmark_tests that have not passed
    void standardizedPerft() {

        const auto start = std::chrono::high_resolution_clock::now();
        for (int i=0; i < 6; i++) {

            parseFEN(testFEN[i]);
            if ( perft(5) == testNodes[i] ) {
                std::cout << " FEN: " << testFEN[i] << '\n';

            } else {
                std::cerr << " FEN: " << testFEN[i] << '\n';
            }
        }
        const std::chrono::duration<float> duration = std::chrono::high_resolution_clock::now() - start;
        std::cout << "Test suite took: " << duration.count() << "s\n\n";

    }
}