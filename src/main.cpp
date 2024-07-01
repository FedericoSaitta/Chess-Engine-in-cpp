#include <iostream>
#include <chrono>

#include "globals.h"
#include "constants.h"
#include "inline_functions.h"
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
    printBitBoard(getRookAttacks(C4, occupancy));


    Test::countPawnAttacks();
    Test::countKnightMoves();
    Test::countKingMoves();

    Test::countBishopMoves();
    Test::countRookMoves();

    // so mask only is true at the index in the list, not mask is the opposide
    // maskCols has ones in the column of the square chosen
    printBitBoard(maskCols[3]);

    printBitBoard(maskKnightMoves(B3));
    printBitBoard(maskKingMoves(H3));
    printBitBoard(maskPawnAttacks(D3, 0));
    printBitBoard(maskPawnAttacks(D3, 1));
}
// This should contain functions for starting the UCI protocol