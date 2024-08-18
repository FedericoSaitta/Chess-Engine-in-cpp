#include "uci.h"
#include "init.h"

#include "benchmark_tests.h"
#include "debug_tests.h"

// #define DEBUG_TEST
// #define BENCHMARK_TEST
//#define TUNE_ARAMIS


int main() {
// for now lets do 64
    initAll(64); // Done at compile time :), using 256 MB size hash
    UCI();

    // some sigsevfault is happening if we run uci before this
   // Test::Debug::gameScenario();

   // Test::BenchMark::staticSearch();
  //  parseFEN("1k1r4/pp1b1R2/3q2pp/4p3/2B5/4Q3/PPP2B2/2K5 b - -");
   // iterativeDeepening(5);

    // killer position 1k1r4/pp1b1R2/3q2pp/4p3/2B5/4Q3/PPP2B2/2K5 b - -
   // Test::BenchMark::standardPerft();

#ifdef TUNE_ARAMIS
    // to avoid including unwanted macros etc.
    #include "init_tuner.h"
    tune();
#endif


#ifdef BENCHMARK_TEST
    Test::BenchMark::staticSearch();
    Test::BenchMark::staticEval();
    Test::BenchMark::matingPuzzles();
    Test::BenchMark::standardPerft();
#endif


#ifdef DEBUG_TEST
    Test::Debug::countPawnAttacks();
    Test::Debug::countKnightMoves();
    Test::Debug::countKingMoves();

    Test::Debug::countBishopMoves_noEdges();
    Test::Debug::countRookMoves_noEdges();

    Test::Debug::mirrorEval();

    Test::Debug::boardInCheck();
    Test::Debug::boardNonPawnMat();
#endif

}
