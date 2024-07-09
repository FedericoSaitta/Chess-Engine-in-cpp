#include "globals.h"
#include <fstream>

#include "evaluation.h"
#include "tests.h"

// #define Perft_TESTS
// #define Move_TESTS
// #define Puzzle_TESTS
// #define Init_TESTS


int main() {
    // startpos is rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
    // kiwipete is r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1
    // cmk is  r2q1rk1/ppp2ppp/2n1bn2/2b1p3/3pP3/3P1NPP/PPP1NPB1/R1BQ1RK1 b - - 0 9
    // killer is 6k1/3q1pp1/pp5p/1r5n/8/1P3PP1/PQ4BP/2R3K1 w - - 0 1
    const std::string logFilePath{ "/Users/federicosaitta/CLionProjects/ChessEngine/logfile.txt" };
  //  logFile.open(logFilePath, std::ios::app);

    initAll(); // Done at compile time :)
    clearTranspositionTable();


    UCI();


    // need to press quit to properly save this file
//    logFile.close();

    // nice article: https://web.archive.org/web/20071030220825/http://www.brucemo.com/compchess/programming/pvs.htm
    // nice transposition table: https://web.archive.org/web/20071031100051/http://www.brucemo.com/compchess/programming/hashing.htm
    // you can use transposition table for pruning the tree, try and do this in the late future when engine is already very strong
    // maybe also do a dynamic allocated version

#ifdef Perft_TESTS
    Test::standardizedPerft();
#endif

#ifdef Move_TESTS
    Test::moveSorting();
#endif

#ifdef Puzzle_TESTS
    Test::matingPuzzles();
#endif

#ifdef Init_TESTS
    Test::countPawnAttacks();
    Test::countKnightMoves();
    Test::countKingMoves();

    Test::countBishopMoves_noEdges();
    Test::countRookMoves_noEdges();

    Test::moveEncodingAndDecoding();
#endif
}

