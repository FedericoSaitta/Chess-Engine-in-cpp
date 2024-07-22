//
// Created by Federico Saitta on 03/07/2024.
//
#include "benchmark_tests.h"

#include <iostream>
#include <random>
#include <cmath>
#include <chrono>
#include <fstream>
#include "../src/movegen/update.h"
#include "macros.h"
#include "hashtable.h"
#include "board.h"
#include "../src/movegen/movegen.h"



static std::vector<std::string> split(const std::string &str, const char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}


namespace Test::BenchMark {
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

    std::uint32_t perft(const int depth, const bool printInfo) {
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

        if (printInfo) {
            // print results
            printf("Depth: %d", depth);
            std::cout << " Nodes: " << nodes;
            std::cout << " Time: " << duration;
            std::cout << " MNodes/s: " << nodes / (duration.count() * 1'000'000);
        }

        return nodes;
    }

    // prints in red benchmark_tests that have not passed
    void standardPerft() {

        std::ifstream file("/Users/federicosaitta/CLionProjects/ChessEngine/benchmark_tests/resources/standard.epd"); // Replace with the actual file path
        if (!file.is_open()) {
            std::cerr << "Failed to open the file." << std::endl;
        }

        const auto start = std::chrono::high_resolution_clock::now();

        std::string line{};
        while (std::getline(file, line)) {

            std::vector<std::string> tokens = split(line, ';');

            if (tokens.empty()) continue;

            parseFEN(tokens[0]);

            const int startDepth { std::stoi( &tokens[1][1] ) };
            const int maxDepth = (tokens.size() - 1) + startDepth - 1;

            for (int depth=startDepth; depth <= maxDepth; depth++) {
                const int nodeCount = std::stoi( (split(tokens[depth - startDepth + 1], ' '))[1]);

                if ( perft(depth, false) != nodeCount ){
                    std::cerr << " Error in FEN: " << tokens[0]
                    << " at depth: " << depth << std::endl;
                }

            }

        }

        const std::chrono::duration<float> duration = std::chrono::high_resolution_clock::now() - start;
        std::cout << "Perft suite took: " << duration.count() << "s\n";

        file.close();
    }
}