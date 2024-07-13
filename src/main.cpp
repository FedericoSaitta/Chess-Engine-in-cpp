
#include <fstream>

#include "uci.h"
#include "init.h"
#include "hashtable.h"
#include "benchmark_tests.h"
#include "debug_tests.h"

//#define DEBUG
//#define BENCHMARK

#include <iostream>
#include "evaluation.h"
#include "board.h"

int main() {
    // startpos is rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
    // kiwipete is r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1
    // cmk is  r2q1rk1/ppp2ppp/2n1bn2/2b1p3/3pP3/3P1NPP/PPP1NPB1/R1BQ1RK1 b - - 0 9
    // killer is 6k1/3q1pp1/pp5p/1r5n/8/1P3PP1/PQ4BP/2R3K1 w - - 0 1
    const std::string logFilePath{ "/Users/federicosaitta/CLionProjects/ChessEngine/logfile.txt" };
    logFile.open(logFilePath, std::ios::app);

    initAll(256); // Done at compile time :)

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
    Test::standardizedPerft();
    Test::matingPuzzles();
#endif

#ifdef DEBUG
    Test::countPawnAttacks();
    Test::countKnightMoves();
    Test::countKingMoves();

    Test::countBishopMoves_noEdges();
    Test::countRookMoves_noEdges();

    Test::moveEncodingAndDecoding();
    Test::moveSorting();

    Test::mirrorEval();
#endif
}

