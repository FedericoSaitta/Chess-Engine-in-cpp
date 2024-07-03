#include <iostream>
#include <chrono>

#include "globals.h"
#include "constants.h"
#include "inline_functions.h"
#include "misc.h"

#include "tests.h"

#define RUN_TESTS


int main() {
    // make sure to call this otherwise tests dont run
    initAll(); // Done at compile time :)

    // remember en-passant square must be lowercase
    const std::string starpos = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 ";
    parseFEN(starpos);
    printBoardFancy();

    MoveList moveList{ {0}, 0 };

    // should en-passant be consdired a capture???

    generateMoves(moveList);
    printMovesList(moveList);
    std::cout << moveList.count << '\n';

    // my guess would be around 4 M moves per second with bulk counting


#ifdef RUN_TESTS
    Test::countPawnAttacks();
    Test::countKnightMoves();
    Test::countKingMoves();

    Test::countBishopMoves_noEdges();
    Test::countRookMoves_noEdges();

    Test::moveEncodingAndDecoding();
#endif
}