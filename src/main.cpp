#include <fstream>
#include <iostream>

#include "uci.h"
#include "init.h"

// #define DEBUG
// #define BENCHMARK

#include "benchmark_tests.h"
#include "debug_tests.h"
#include "movegen/movegen.h"

int main(int argc, char** argv) {
    const std::string logFilePath{ "/Users/federicosaitta/CLionProjects/ChessEngine/logfile.txt" };
    logFile.open(logFilePath, std::ios::app);

    initAll(256); // Done at compile time :), using 256 MB size hash
    UCI();
    Test::BenchMark::standardPerft();


    logFile.close();

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