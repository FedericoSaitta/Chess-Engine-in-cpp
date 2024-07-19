//
// Created by Federico Saitta on 05/07/2024.
//
#include "benchmark_tests.h"

#include <iostream>
#include <random>
#include <cmath>
#include <chrono>

#include "hashtable.h"
#include "board.h"
#include "../src/search/search.h"
#include "macros.h"

namespace Test::BenchMark {
    // this test can be very time consuming
    std::vector<Puzzle> matePuzzles {
    {"7k/5pp1/p3pN1p/1p1r1q2/2p4Q/2Pn2RP/PP3P1K/8 w - - 0 1", "h4h6", 2},
    {"2r3k1/2r2p1p/p3p1p1/1p5P/2b2NN1/2b5/P2R1PP1/3R2K1 w - - 0 1", "d2d8", 3},

    {"2rr1k2/1b3p1Q/p2p2p1/4P3/2PnP2R/3q3P/P5P1/R5K1 w - - 0 1", "h7h8", 4},
    {"2rq1r2/pb2bpp1/1pnppn1p/2p1k3/4p2B/1PPBP3/PP2QPPP/R2NRNK1 w - - 0 1", "e2f3", 4},

    {"rr4qk/pp5p/5b2/2p5/2bnB3/P3B2P/1P3PP1/1K1R3R b - - 0 1", "c4a2", 5},
    {"N1bk3r/P5pp/3b1p2/3B4/R2nP1nq/3P3N/1BP3KP/4Q2R b - - 0 1", "h4h3", 5}
    };

    void matingPuzzles() {

        for (const auto& puzzle : matePuzzles) {

            clearTranspositionTable();
            parseFEN(puzzle.FEN);

            const auto start = std::chrono::high_resolution_clock::now();
            iterativeDeepening(puzzle.depth * 2); // as we are considering half moves
            const std::chrono::duration<float> duration = std::chrono::high_resolution_clock::now() - start;

            std::cout << "Mating Move: " << puzzle.matingMove << " Time taken: " << duration << "s\n\n";
        }
    }
}