//
// Tests are designed to be fully contained so that user only needs to call the test from the main
// function without the need of any helper functions or previous input

#include "../benchmark_tests/benchmark_tests.h"

#include <bit>
#include <iostream>

#include "debug_tests.h"
#include "../include/init.h"
#include "../../chess/misc.h"


namespace Test::Debug{

    void countPawnAttacks() {

        bool testPassed{ true };
        // Range is restricted as pawns can only captures between rank 1 and 7 inclusive
        for(int i=8; i < 56; i++) {

            const int whiteCount{ std::popcount(bitPawnAttacks[0][i]) };
            const int blackCount{ std::popcount(bitPawnAttacks[1][i]) };

            // checks that pawn is not on A or H file
            if ( (i % 8 == 0) | (i % 8 == 7)) {
                if ( (whiteCount != blackCount) || (whiteCount != 1) ) {
                    testPassed = false;
                }

            } else if ( (whiteCount != blackCount) || (whiteCount != 2) ){
                testPassed = false;
            }
        }

        printTestOutput(testPassed, "countPawnAttacks");
    }

    void countKnightMoves() {

        bool testPassed{ true };
        if ( ( std::popcount(bitKnightAttacks[A1]) != std::popcount(bitKnightAttacks[A8]) )
          || ( std::popcount(bitKnightAttacks[A8]) != 2 ) ){
            testPassed = false;
        }

        if ( ( std::popcount(bitKnightAttacks[H8]) != std::popcount(bitKnightAttacks[H1]) )
          || ( std::popcount(bitKnightAttacks[H8]) != 2) ) {
            testPassed = false;
        }

        if (std::popcount(bitKnightAttacks[E4]) != 8) {
            testPassed = false;
        }

        // Total number of moves of a knight is 336:
        // https://chess.stackexchange.com/questions/33329/how-many-possible-chess-moves-are-there
        int totalMoves{};
        for (int i=0; i < 64; i++) { totalMoves += (std::popcount(bitKnightAttacks[i])); }

        if (totalMoves != 336) {
            testPassed = false;
        }

        printTestOutput(testPassed, "Count Knight Moves");
    }

    void countKingMoves() {
        bool testPassed{ true };
        if ( ( std::popcount(bitKingAttacks[A1]) != std::popcount(bitKingAttacks[A8]) )
          || ( std::popcount(bitKingAttacks[A8]) != 3 ) ){
            testPassed = false;
          }

        if ( ( std::popcount(bitKingAttacks[H8]) != std::popcount(bitKingAttacks[H1]) )
          || ( std::popcount(bitKingAttacks[H8]) != 3) ) {
            testPassed = false;
          }

        if (std::popcount(bitKingAttacks[E4]) != 8) {
            testPassed = false;
        }

        // Total number of moves of a knight is 336:
        // https://chess.stackexchange.com/questions/33329/how-many-possible-chess-moves-are-there
        int totalMoves{};
        for (int i=0; i < 64; i++) { totalMoves += (std::popcount(bitKingAttacks[i])); }

        if (totalMoves != 420) {
            testPassed = false;
        }

        printTestOutput(testPassed, "Count King Moves");
    } // This doesnt consider castling

}