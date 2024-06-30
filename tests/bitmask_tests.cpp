//
// Tests are designed to be fully contained so that user only needs to call the test from the main
// function without the need of any helper functions or previous input

#include "bitmask_tests.h"

#include <bit>
#include <iostream>
#include "/Users/federicosaitta/CLionProjects/ChessEngine/src/globals.h"
#include "/Users/federicosaitta/CLionProjects/ChessEngine/src/misc.h"



// Code is quite repetitive could you use some sort of templates???

namespace Test{

    void countPawnAttacks() {
        setBits();
        maskLeaperPiecesArrays(); // initializes the 2D array containing pawn attacks
        bool testPassed{true};

        // Range is restricted as pawns can only captures between rank 1 and 7 inclusive
        for(int i=8; i < 56; i++) {

            const int whiteCount{ std::popcount(pawnAttacks[0][i]) };
            const int blackCount{ std::popcount(pawnAttacks[1][i]) };

            if ( (col[i] == 0) | (col[i] == 7)) {
                if ( (whiteCount != blackCount) || (whiteCount != 1) ) {
                    std::cerr << "**Index " << i << ": wrong** \n";
                    testPassed = false;
                }

            } else if ( (whiteCount != blackCount) || (whiteCount != 2) ){
                std::cerr << "**Index " << i << ": wrong** \n";
                testPassed = false;
            }
        }

        std::cout << std::boolalpha;
        std::cout << "countPawnAttacks Test successfull: " << testPassed << '\n';

    }
    void countKnightMoves() {
        setBits();
        maskLeaperPiecesArrays();
        bool testPassed{true};

        if ( ( std::popcount(bitKnightMoves[A1]) != std::popcount(bitKnightMoves[A8]) )
          || ( std::popcount(bitKnightMoves[A8]) != 2 ) ){
            std::cerr << "**Index " << A1 << " or " << A8 << ": wrong** \n";
            testPassed = false;
        }

        if ( ( std::popcount(bitKnightMoves[H8]) != std::popcount(bitKnightMoves[H1]) )
          || ( std::popcount(bitKnightMoves[H8]) != 2) ) {
            std::cerr << "**Index " << H8 << " or " << H1 << ": wrong** \n";
            testPassed = false;
        }

        if (std::popcount(bitKnightMoves[E4]) != 8) {
            std::cerr << "**Index " << E4 << ": wrong** \n";
            testPassed = false;
        }

        // Total number of moves of a knight is 336:
        // https://chess.stackexchange.com/questions/33329/how-many-possible-chess-moves-are-there
        int totalMoves{};
        for (int i=0; i < 64; i++) { totalMoves += (std::popcount(bitKnightMoves[i])); }

        if (totalMoves != 336) {
            std::cerr << "**Total number of moves is wrong** \n";
            testPassed = false;
        }

        std::cout << std::boolalpha;
        std::cout << "countKnightMoves Test successfull: " << testPassed << '\n';
    }
    void countKingMoves() {
        setBits();
        maskLeaperPiecesArrays();
        bool testPassed{true};

        if ( ( std::popcount(bitKingMoves[A1]) != std::popcount(bitKingMoves[A8]) )
          || ( std::popcount(bitKingMoves[A8]) != 3 ) ){
            std::cerr << "**Index " << A1 << " or " << A8 << ": wrong** \n";
            testPassed = false;
          }

        if ( ( std::popcount(bitKingMoves[H8]) != std::popcount(bitKingMoves[H1]) )
          || ( std::popcount(bitKingMoves[H8]) != 3) ) {
            std::cerr << "**Index " << H8 << " or " << H1 << ": wrong** \n";
            testPassed = false;
          }

        if (std::popcount(bitKingMoves[E4]) != 8) {
            std::cerr << "**Index " << E4 << ": wrong** \n";
            testPassed = false;
        }

        // Total number of moves of a knight is 336:
        // https://chess.stackexchange.com/questions/33329/how-many-possible-chess-moves-are-there
        int totalMoves{};
        for (int i=0; i < 64; i++) { totalMoves += (std::popcount(bitKingMoves[i])); }

        if (totalMoves != 420) {
            std::cerr << "**Total number of moves is wrong** \n";
            testPassed = false;
        }

        std::cout << std::boolalpha;
        std::cout << "countKingMoves Test successfull: " << testPassed << '\n';
    } // This doesnt consider castling
}