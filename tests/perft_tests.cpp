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

namespace Test{

    long long int nodes{};

    void perftDriver(int depth) {

        if (depth == 0) {
            nodes++;
            return;
        }

        MoveList moveList{};
        generateMoves(moveList, 0);

        for (int moveCount = 0; moveCount < moveList.count; moveCount++)
        {
            // preserve board state
            copyBoard();

            if (!makeMove(moveList.moves[moveCount], 0)) continue;

            perftDriver(depth - 1);

            restoreBoard();
        }
    }


    void perft(const std::string& fenString, const int depth) {
        parseFEN(fenString);
        nodes = 0;
        auto start = std::chrono::high_resolution_clock::now();
        MoveList moveList{ };
        generateMoves(moveList, 0);


        for (int moveCount = 0;  moveCount < moveList.count; moveCount++) {
            // preserve board state
            copyBoard();


            // make move
            if (!makeMove(moveList.moves[moveCount], 0)) {
                std::cout << "Move illegal," << '\n';
                continue;
            }

            printBoardFancy();


            long cummulative_nodes = nodes;

            perftDriver(depth - 1);

            long old_nodes = nodes - cummulative_nodes;

            // take back
            restoreBoard();

            // print move
            printf("     move: %s%s%c  nodes: %ld\n", chessBoard[getMoveStartSQ(moveList.moves[moveCount])],
                                                     chessBoard[getMoveTargettSQ(moveList.moves[moveCount])],
                                                     promotedPieces[getMovePromPiece(moveList.moves[moveCount])],
                                                     old_nodes);

        }


        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);

        // print results
        printf("\n   Depth: %d\n", depth);
        std::cout << "   Nodes: " << nodes << '\n';
        std::cout << "   Time: " << duration << '\n';



    }
}