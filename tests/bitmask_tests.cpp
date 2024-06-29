//
// Tests are designed to be fully contained so that user only needs to call the test from the main
// function without the need of any helper functions or previous input

#include "bitmask_tests.h"

#include <bit>
#include <iostream>
#include "/Users/federicosaitta/CLionProjects/ChessEngine/src/globals.h"
#include "/Users/federicosaitta/CLionProjects/ChessEngine/src/misc.h"


namespace Test{

    void countPawnAttacks() {
        setBits();
        maskPawnAttacksArray(); // initializes the 2D array containing pawn attacks
        bool testPassed{true};

        // Range is restricted as pawns can only captures between rank 1 and 7 inclusive
        for(int i=8; i < 56; i++) {

            const int whiteCount{ std::popcount(pawnAttacks[0][i]) };
            const int blackCount{ std::popcount(pawnAttacks[1][i]) };

            if ( (col[i] == 0) | (col[i] == 7)) {
                if ( (whiteCount != blackCount) || (whiteCount != 1) ) {
                    std::cout << "**Index " << i << ": wrong** \n";
                    testPassed = false;
                }

            } else if ( (whiteCount != blackCount) || (whiteCount != 2) ){
                std::cout << "**Index " << i << ": wrong** \n";
            testPassed = false;
            }
        }

        std::cout << std::boolalpha;
        std::cout << "countPawnAttacks Test successfull: " << testPassed << '\n';

    }

    void countKnightMoves() {
        setBits();
        maskKnightMovesArray();
        bool testPassed{true};

        if ( ( std::popcount(bitKnightMoves[A1]) != std::popcount(bitKnightMoves[A8]) )
          || ( std::popcount(bitKnightMoves[A8]) != 2 ) ){
            std::cout << "**Index " << A1 << " or " << A8 << ": wrong** \n";
        }

        if ( ( std::popcount(bitKnightMoves[H8]) != std::popcount(bitKnightMoves[H1]) )
          || ( std::popcount(bitKnightMoves[A8]) != 2) ) {
            std::cout << "**Index " << H8 << " or " << H1 << ": wrong** \n";
        }

        if (std::popcount(bitKnightMoves[E4]) != 8){ std::cout << "**Index " << E4 << ": wrong** \n"; }

        // Total number of moves of a knight is 336:
        // https://math.stackexchange.com/questions/2204736/how-many-different-knights-moves-are-there-on-an-n-times-n-chessboard
        int totalMoves{};
        for (int i=0; i < 64; i++) { totalMoves += (std::popcount(bitKnightMoves[i])); }

        if (totalMoves != 336) { std::cout << "**Total number of moves is wrong** \n"; }

        std::cout << std::boolalpha;
        std::cout << "countKnightMoves Test successfull: " << testPassed << '\n';
    }
}