#include "benchmark_tests.h"
#include "uci.h"
#include "init.h"
#include "search/search.h"
#include "eval/nnue.h"

#include <iostream>
#include <string>

// #define DEBUG_TEST
// #define BENCHMARK_TEST
//#define TUNE_ARAMIS

int main(int argc, char *argv[]) {
    // In sprt testing 8 Mb or smaller should be used
    const bool nnueTest = argc > 1 && std::string(argv[1]) == "nnue-test";
    initAll(nnueTest ? 1 : 256); // NNUE validation does not need a large TT.

    std::string nnueError;
    nnue::loadDefaultNetwork(argv[0], nnueError);

    if (argc > 1) { // argc > 1, since argv[0] is the program name, argv[1] is the first argument
        const std::string arg1 = argv[1];

        if (arg1 == "nnue-test") {
            const std::string path = argc > 2 ? argv[2] : nnue::loadedNetworkPath();
            return nnue::runSelfTests(path, std::cout) ? 0 : 1;
        }

        if (arg1 == "bench") {
            Searcher thread{};
            Test::BenchMark::staticSearch(thread, 10);
        }
    }

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
