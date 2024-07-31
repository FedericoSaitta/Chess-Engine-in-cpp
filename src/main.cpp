#include <fstream>
#include <iostream>

#include "uci.h"
#include "init.h"

#include "benchmark_tests.h"
#include "debug_tests.h"

// #define DEBUG
// #define BENCHMARK

#include <fstream>
#include <iostream>
#include <random>
#include <cmath>
#include "uci.h"
#include "init.h"
#include "benchmark_tests.h"
#include "debug_tests.h"
#include "../src/search/movesort.h"
#include "../include/board.h"
#include "../include/macros.h"
#include "../include/misc.h"
#include "../src/search/search.h"
#include "search/search.h"
#include "board.h"
#include "eval/evaluation.h"
#include "logger/logger.h"

int main() {

    logFile.setLoggingLevel(NONE);

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
