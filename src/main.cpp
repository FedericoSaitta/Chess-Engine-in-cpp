#include <iostream>

#include "globals.h"
#include "misc.h"

#include "/Users/federicosaitta/CLionProjects/ChessEngine/tests/bitmask_tests.h"


int main() {

    setBits();


    Test::countPawnAttacks();
    Test::countKnightMoves();
    Test::countKingMoves();
}
// This should contain functions for starting the UCI protocol