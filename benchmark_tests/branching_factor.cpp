//
// Created by Federico Saitta on 12/07/2024.
//
#include "benchmark_tests.h"

#include <iostream>
#include <random>
#include <vector>
#include <sstream>
#include <cmath>

#include "update.h"
#include "macros.h"
#include "hashtable.h"
#include "board.h"
#include "movegen.h"
#include "search.h"
#include "misc.h"
#include "uci.h"

namespace Test::BenchMark {

    // these are mostly middle game positions
    static const std::string testFEN[] {
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
        "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",
        "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1",
        "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1",
        "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8",
        "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10 ",
        "r1bq1rk1/ppp1bppp/2n1pn2/3p4/2PP4/2N2N2/PPQ1PPPP/R1B1KB1R w KQ - 2 8",
        "r2q1rk1/pp1nbppp/3ppn2/8/2PNP3/2N1B3/PP2BPPP/R2QK2R w KQ - 0 10",
        "r1bq1rk1/pp3ppp/2n1pn2/3p4/2PP4/2NBPN2/PP3PPP/R1BQ1RK1 w - - 0 9",
        "r1bq1rk1/pp1n1ppp/2n1p3/1BppP3/3P4/2N2N2/PP3PPP/R1BQ1RK1 w - - 0 10",
        "r2q1rk1/pp1nbppp/3ppn2/2p5/2P1P3/2N1BN2/PP1QBPPP/R3K2R w KQ - 1 11"};



    void branchingFactor() {
        constexpr float maxDepth { 10 };

        int numEntries { static_cast<int>((maxDepth - 5) * sizeof(testFEN) / sizeof(std::string)) };
        double averageBranchingRatio{};

        for (std::string FEN: testFEN) {
            resetGameVariables();
            parseFEN(FEN);

            std::streambuf* originalCoutBuffer = std::cout.rdbuf();
            std::ostringstream output;
            std::cout.rdbuf(output.rdbuf());

            iterativeDeepening(maxDepth);

            std::cout.rdbuf(originalCoutBuffer);
            std::istringstream outputStream(output.str());
            std::string line;

            while (std::getline(outputStream, line)) {
                std::vector<std::string> tokens = split(line);

                if ( tokens[6] == "nodes" ) {
                    if ( std::stoi(tokens[5]) > 5 ) {
                        // the formula is pow(nodes at depth N, 1 / N)
                        const float branchingRatio { static_cast<float>(std::pow( std::stoi(tokens[7]), 1 / std::stod(tokens[5]) )) };

                        averageBranchingRatio += (branchingRatio / numEntries);
                        //std::cout << "At depth: " << tokens[5] << " Branching Ratio: " << branchingRatio << '\n';
                    }
                }
                else break;
            }
        }

        std::cout << "AVG Branching Ratio: " << averageBranchingRatio << '\n';
    }
}
