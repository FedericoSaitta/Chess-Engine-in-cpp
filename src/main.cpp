#include <iostream>
#include <chrono>
#include <cstring>

#include "globals.h"
#include "constants.h"
#include "inline_functions.h"
#include "misc.h"

#include "tests.h"

// #define RUN_TESTS


int main() {
    // make sure to call this otherwise tests dont run
    initAll(); // Done at compile time :)

    // remember en-passant square must be lowercase
    const std::string starpos = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    /*parseFEN(fenString);
    nodes = 0;
    auto start = std::chrono::high_resolution_clock::now();
    MoveList moveList{ };
    generateMoves(moveList, 0);


    for (int moveCount = 0;  moveCount < moveList.count; moveCount++) {
    // preserve board state
    copyBoard();


    // make move
    if (!makeMove(moveList.moves[moveCount], 0)) {
    std::cout << "Move illegal," << '\n';
    continue;
    }

    printBoardFancy();
    */

    Test::perft(starpos, 2);


#ifdef RUN_TESTS
    Test::countPawnAttacks();
    Test::countKnightMoves();
    Test::countKingMoves();

    Test::countBishopMoves_noEdges();
    Test::countRookMoves_noEdges();

    Test::moveEncodingAndDecoding();
#endif
}