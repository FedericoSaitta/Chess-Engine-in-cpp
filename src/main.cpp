#include <iostream>
#include <chrono>
#include <bitset>

#include "globals.h"
#include "constants.h"
#include "inline_functions.h"
#include "misc.h"

#include "bitmask_tests.h"

// #define RUN_TESTS


int main() {

    initAll(); // this is successfully done at compile time :)

    const std::string starpos = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    parseFEN(starpos);

    printBoardFancy();



#ifdef RUN_TESTS
    Test::countPawnAttacks();
    Test::countKnightMoves();
    Test::countKingMoves();

    Test::countBishopMoves_noEdges();
    Test::countRookMoves_noEdges();

    BITBOARD occupancies{ 0ULL };
    setBit(occupancies, A1);
    setBit(occupancies, B3);
    setBit(occupancies, C5);
    setBit(occupancies, D7);
    setBit(occupancies, G2);
    setBit(occupancies, H4);
    setBit(occupancies, B7);

    printBitBoard(getRookAttacks(F3, occupancies));
    printBitBoard(getBishopAttacks(F3, occupancies));
#endif
}
// This should contain functions for starting the UCI protocol