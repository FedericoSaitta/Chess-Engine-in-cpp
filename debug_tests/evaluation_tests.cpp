//
// Created by Federico Saitta on 12/07/2024.
//
#include "benchmark_tests.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

#include "../src/eval/evaluation.h"
#include "board.h"
#include "misc.h"

U64 flipVertical(U64 x) {
    return  ( (x << 56)                           ) |
            ( (x << 40) & (0x00ff000000000000) ) |
            ( (x << 24) & (0x0000ff0000000000) ) |
            ( (x <<  8) & (0x000000ff00000000) ) |
            ( (x >>  8) & (0x00000000ff000000) ) |
            ( (x >> 24) & (0x0000000000ff0000) ) |
            ( (x >> 40) & (0x000000000000ff00) ) |
            ( (x >> 56) );
}

static void mirrorBitboards() {
    for (int piece=0; piece < 12; piece++) {
        bitboards[piece] = flipVertical(bitboards[piece]);
    }
}

static void swapColours() {
    for (int piece=0; piece < 6; piece++) {
        const U64 tempCopy{ bitboards[piece] };

        bitboards[piece] = bitboards[piece + 6];
        bitboards[piece + 6] = tempCopy;
    }

    memset(occupancies, 0ULL, sizeof(occupancies));

    for (int bbPiece=0; bbPiece < 6; bbPiece++) {
        occupancies[0] |= bitboards[bbPiece]; // for white
        occupancies[1] |= bitboards[bbPiece + 6]; // for black
        occupancies[2] |= (bitboards[bbPiece] | bitboards[bbPiece + 6]); // for both
    }
}



namespace Test::Debug {

    // (a) normal board (b) reflect by swapping ranks and changing the color of all pieces; (c)
    void mirrorEval() {

        const std::string fileName = "/Users/federicosaitta/CLionProjects/ChessEngine/debug_tests/resources/random_positions.epd";
        std::ifstream file(fileName);

        if (!file.is_open()) { std::cerr << "Error opening file: " << fileName << std::endl; }

        std::string line;
        while (std::getline(file, line)) {
            std::vector<std::string> tokens = split(line);

            std::string FEN{ tokens[0] + ' ' + tokens[1] + ' ' + tokens[2] + ' ' + tokens[3] };

            parseFEN(FEN);
            int whiteEval { evaluate() };
            side ^= 1;
            int blackEval { evaluate() };
            side ^= 1;

            // White and black should return a zero-sum eval, if they dont we are overcounting white or black pieces
            if ( (whiteEval + blackEval) != 0) { std::cerr << "Mismatch between eval of the same position with switched sides\n"; }

            // Now we flip ranks and change colours
         //   printBoardFancy();
            mirrorBitboards();
            swapColours();
          //  printBoardFancy();

            int mirroredWhiteEval { evaluate() };
            side ^= 1;
            int mirroredBlackEval { evaluate() };
            side ^= 1;

         //   std::cout << whiteEval << ' ' << blackEval << '\n';
        //    std::cout << mirroredWhiteEval << ' ' << mirroredBlackEval << '\n';

            if ( (whiteEval != mirroredBlackEval) || (blackEval != mirroredWhiteEval)) {
                std::cerr << "Mismatch between eval of the same position with mirrored boards\n";
                std::cout << FEN << '\n';
            }

        }
        std::cout << "mirrorEval test completed\n";
        file.close();

    }
}