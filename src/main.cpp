#include <fstream>
#include <iostream>

#include "uci.h"
#include "init.h"

//#define DEBUG
// #define BENCHMARK


#include "benchmark_tests.h"
#include "debug_tests.h"
#include "movegen.h"

int main() {
    const std::string logFilePath{ "/Users/federicosaitta/CLionProjects/ChessEngine/logfile.txt" };
    logFile.open(logFilePath, std::ios::app);

    initAll(256); // Done at compile time :), using 256 MB size hash

    UCI();

    // need to press quit to properly save this file
    logFile.close();

    // nice article: https://web.archive.org/web/20071030220825/http://www.brucemo.com/compchess/programming/pvs.htm
    // nice transposition table: https://web.archive.org/web/20071031100051/http://www.brucemo.com/compchess/programming/hashing.htm
    // you can use transposition table for pruning the tree, try and do this in the late future when engine is already very strong
    // maybe also do a dynamic allocated version

    // good chess article:, really nice article for move generation
    // https://peterellisjones.com/posts/generating-legal-chess-moves-efficiently/


#ifdef BENCHMARK
    Test::BenchMark::staticSearch();
    Test::BenchMark::matingPuzzles();
#endif

    /*
#ifdef DEBUG
    Test::countPawnAttacks();
    Test::countKnightMoves();
    Test::countKingMoves();

    Test::countBishopMoves_noEdges();
    Test::countRookMoves_noEdges();

    Test::moveEncodingAndDecoding();
    Test::moveSorting();

    Test::mirrorEval();
    Test::Debug::historyScores();
#endif
*/
}

