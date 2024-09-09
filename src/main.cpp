#include "uci.h"
#include "init.h"

// #define DEBUG_TEST
// #define BENCHMARK_TEST
//#define TUNE_ARAMIS

int main() {
    // In sprt testing 8 Mb or smaller should be used
    initAll(256); // Done at compile time :), using 256 MB dynamic allocated hash

    UCI();

#ifdef TUNE_ARAMIS
    // to avoid including unwanted macros etc.
    #include "init_tuner.h"
    tune();
#endif

#ifdef BENCHMARK_TEST
    #include "benchmark_tests.h"

    Test::BenchMark::staticSearch();
    Test::BenchMark::staticEval();
    Test::BenchMark::matingPuzzles();
    Test::BenchMark::standardPerft();
#endif

#ifdef DEBUG_TEST
    #include "debug_tests.h"

    Test::Debug::countPawnAttacks();
    Test::Debug::countKnightMoves();
    Test::Debug::countKingMoves();

    Test::Debug::countBishopMoves_noEdges();
    Test::Debug::countRookMoves_noEdges();

    Test::Debug::mirrorEval();

    Test::Debug::boardInCheck();
    Test::Debug::boardNonPawnMat();
    Test::Debug::see_test();
#endif

}
