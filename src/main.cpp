#include <fstream>
#include <iostream>

#include "uci.h"
#include "init.h"

#include "benchmark_tests.h"
#include "debug_tests.h"

// #define DEBUG
// #define BENCHMARK

#include "search/search.h"
#include "board.h"
int main() {

    initAll(256); // Done at compile time :), using 256 MB size hash

   // Test::Debug::moveSorting();
    UCI();

    // r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1
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