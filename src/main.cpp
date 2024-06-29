#include <iostream>

#include "globals.h"
#include "misc.h"

#include "/Users/federicosaitta/CLionProjects/ChessEngine/tests/bitmask_tests.h"


int main() {

    setBits();

    BITBOARD a = maskKnightMoves(E4);

    Test::countPawnAttacks();
    Test::countKnightMoves();
}
// This should contain functions for starting the UCI protocol