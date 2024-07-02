#include <iostream>
#include <chrono>

#include "globals.h"
#include "constants.h"
#include "inline_functions.h"
#include "misc.h"

#include "bitmask_tests.h"

// #define RUN_TESTS


int main() {
    // make sure to call this otherwise tests dont run
    initAll(); // Done at compile time :)

    // remember en-passant square must be lowercase
    const std::string starpos = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 ";
    parseFEN(starpos);

    printBoardFancy();

   // printAttackedSquares(Black);

    // testing the time it takes
    auto start = std::chrono::high_resolution_clock::now();

    generateMoves();


    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed_ms = end - start;
    std::cout << "Elapsed time: " << elapsed_ms.count() << " milliseconds\n";



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