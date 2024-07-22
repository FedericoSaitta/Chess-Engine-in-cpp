#include <fstream>
#include <iostream>

#include "uci.h"
#include "init.h"

// #define DEBUG
// #define BENCHMARK

#include "benchmark_tests.h"
#include "debug_tests.h"
#include "movegen/movegen.h"


int main() {
    const std::string logFilePath{ "/Users/federicosaitta/CLionProjects/ChessEngine/logfile.txt" };
    logFile.open(logFilePath, std::ios::app);

    initAll(256); // Done at compile time :), using 256 MB size hash

    UCI();

    logFile.close();

    // nice article: https://web.archive.org/web/20071030220825/http://www.brucemo.com/compchess/programming/pvs.htm
    // nice transposition table: https://web.archive.org/web/20071031100051/http://www.brucemo.com/compchess/programming/hashing.htm

    // good chess article:, really nice article for move generation
    // https://peterellisjones.com/posts/generating-legal-chess-moves-efficiently/


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