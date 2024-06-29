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
        setBits(); // initializes the notA and notHFile ints
        maskPawnAttacksArray(); // initializes the 2D array containing pawn attacks

        // Range is restricted as pawns can only captures between rank 1 and 7 inclusive
        for(int i=8; i < 56; i++) {

            const int whiteCount{ std::popcount(pawnAttacks[0][i]) };
            const int blackCount{ std::popcount(pawnAttacks[1][i]) };

            if ( (col[i] == 0) | (col[i] == 7)) {
                if ( (whiteCount == blackCount) && (whiteCount == 1) ) {
                    std::cout << "Index " << i << ": correct \n";
                }
                else { std::cout << "**Index " << i << ": wrong** \n"; }

            } else if ( (whiteCount == blackCount) && (whiteCount == 2) ){
                std::cout << "Index " << i << ": correct \n";
            } else { std::cout << "**Index " << i << ": wrong** \n"; }
        }
    }

}