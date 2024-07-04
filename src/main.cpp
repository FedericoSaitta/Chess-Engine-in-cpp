#include <iostream>
#include <chrono>
#include <cstring>

#include "globals.h"
#include "constants.h"
#include "inline_functions.h"
#include "misc.h"

#include "tests.h"

// #define Perft_TESTS
// #define RUN_TESTS

int main() {
    // make sure to call this otherwise tests dont run
    initAll(); // Done at compile time :)

    // remember en-passant square must be lowercase
    const std::string position = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
//    parseFEN(starpos);

    Test::perft(position, 7);




#ifdef Perft_TESTS
    Test::bulkPerft();
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