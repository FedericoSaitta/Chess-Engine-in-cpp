#include <iostream>

#include "globals.h"
#include "misc.h"

#include "/Users/federicosaitta/CLionProjects/ChessEngine/tests/bitmask_tests.h"


int main() {

    setBits();
    maskLeaperPiecesArrays();

    // now the rook seems to be running okay, make sure to run test for it

    Test::countPawnAttacks();
    Test::countKnightMoves();
    Test::countKingMoves();

    Test::countBishopMoves();
    Test::countRookMoves();
}
// This should contain functions for starting the UCI protocol