#include <iostream>
#include <chrono>

#include "globals.h"
#include "misc.h"

#include "/Users/federicosaitta/CLionProjects/ChessEngine/tests/bitmask_tests.h"


int main() {

    setBits();
    initLeaperPiecesAttacks();

    initSliderAttacks(1);
    initSliderAttacks(0);

    U64 occupancy { 0ULL };
    setBit(occupancy, B2);

    printBitBoard(occupancy);

    printBitBoard(getRookAttacks(D4, occupancy));


    //BITBOARD a{ rookAttacksOnTheFly(E4, blocker) };
    //printBitBoard(blocker);
    //printBitBoard(a);

    // now the rook seems to be running okay, make sure to run test for it

    /* maybe you could rewrite some of the tests, they seem like overkill for that they do and a bit repetitive
    Test::countPawnAttacks();
    Test::countKnightMoves();
    Test::countKingMoves();

    Test::countBishopMoves();
    Test::countRookMoves();
    */



}
// This should contain functions for starting the UCI protocol