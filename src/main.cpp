#include <fstream>
#include <iostream>

#include "uci.h"
#include "init.h"

#include "benchmark_tests.h"
#include "debug_tests.h"

// #define DEBUG
// #define BENCHMARK

int main() {

    initAll(256); // Done at compile time :), using 256 MB size hash
    UCI();


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