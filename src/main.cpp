#include <iostream>
#include <chrono>

#include "globals.h"
#include "constants.h"
#include "misc.h"

#include "bitmask_tests.h"


int main() {

    setBits();
    initLeaperPiecesAttacks();

    initSliderAttacks(1);
    initSliderAttacks(0);

    U64 occupancy { 0ULL };
    setBit(occupancy, B2);
    setBit(occupancy, D7);
    setBit(occupancy, A6);
    setBit(occupancy, B4);
    setBit(occupancy, E4);
    setBit(occupancy, H7);
    setBit(occupancy, C5);


    printBitBoard(occupancy);

    printBitBoard(getBishopAttacks(D4, occupancy));


    //BITBOARD a{ rookAttacksOnTheFly(E4, blocker) };
    //printBitBoard(blocker);
    //printBitBoard(a);

    // now the rook seems to be running okay, make sure to run test for it


    Test::countPawnAttacks();
    Test::countKnightMoves();
    Test::countKingMoves();

    Test::countBishopMoves();
    Test::countRookMoves();

    // so mask only is true at the index in the list, not mask is the opposide
    // maskCols has ones in the column of the square chosen
    printBitBoard(mask[0]);
    printBitBoard(notMask[0]);
    printBitBoard(maskCols[8]);


}
// This should contain functions for starting the UCI protocol