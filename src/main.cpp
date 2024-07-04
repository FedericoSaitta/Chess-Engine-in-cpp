#include <iostream>
#include <chrono>


#include "globals.h"
#include "constants.h"
#include "inline_functions.h"
#include "misc.h"

#include "tests.h"

// #define Perft_TESTS
// #define RUN_TESTS

int main() {
    initAll(); // Done at compile time :)

    UCI();











#ifdef Perft_TESTS
    Test::standardizedPerft();
    Test::standardizedPerft();
    Test::standardizedPerft();
    Test::standardizedPerft();
    Test::standardizedPerft();
#endif


#ifdef RUN_TESTS
    Test::countPawnAttacks();
    Test::countKnightMoves();
    Test::countKingMoves();

    Test::countBishopMoves_noEdges();
    Test::countRookMoves_noEdges();

    Test::moveEncodingAndDecoding();
#endif
}