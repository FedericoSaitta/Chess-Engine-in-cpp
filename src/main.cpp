#include <fstream>
#include <iostream>

#include "uci.h"
#include "init.h"

#include "benchmark_tests.h"
#include "debug_tests.h"

// #define DEBUG
// #define BENCHMARK

#include <iostream>
#include <random>
#include <cmath>

#include "uci.h"
#include "../src/search/movesort.h"
#include "../include/board.h"
#include "../include/macros.h"
#include "../include/misc.h"
#include "../src/search/search.h"

#include "search/search.h"
#include "board.h"
int main() {

    initAll(256); // Done at compile time :), using 256 MB size hash

  //  UCI();
    Test::BenchMark::standardPerft();

    /*
    parseFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -");

    printBitBoard(board.bitboards[PAWN]);

    SET_BIT_FALSE(board.bitboards[PAWN], A2);
    SET_BIT(board.bitboards[PAWN], A3);

    printBitBoard(board.bitboards[PAWN]);

    parseFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -");
    printBitBoard(board.bitboards[PAWN]);

    board.bitboards[PAWN] ^= ( (1ULL << A2) | (1ULL << A3) );

    printBitBoard(board.bitboards[PAWN]);
    */


#ifdef BENCHMARK
    Test::BenchMark::staticSearch();
    Test::BenchMark::matingPuzzles();
    Test::BenchMark::standardPerft();
#endif


#ifdef DEBUG
    Test::Debug::countPawnAttacks();
    Test::Debug::countKnightMoves();
    Test::Debug::countKingMoves();

    Test::Debug::countBishopMoves_noEdges();
    Test::Debug::countRookMoves_noEdges();

    Test::Debug::moveEncodingAndDecoding();
 //   Test::Debug::moveSorting();

    Test::Debug::mirrorEval();
   // Test::Debug::historyScores();
#endif

}