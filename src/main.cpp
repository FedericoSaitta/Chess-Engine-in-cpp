#include <iostream>
#include <chrono>


#include "globals.h"
#include "macros.h"
#include "inline_functions.h"
#include "misc.h"

#include "evaluation.h"
#include "tests.h"

// #define Perft_TESTS
// #define RUN_TESTS


int main() {
    // startpos is rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
    // kiwipete is r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1
    // cmk is  r2q1rk1/ppp2ppp/2n1bn2/2b1p3/3pP3/3P1NPP/PPP1NPB1/R1BQ1RK1 b - - 0 9


    initAll(); // Done at compile time :)
    UCI();



#ifdef Perft_TESTS
    Test::standardizedPerft();
   // Test::standardizedPerft();
 //   Test::standardizedPerft();
  //  Test::standardizedPerft();
  //  Test::standardizedPerft();
#endif


#ifdef RUN_TESTS
    Test::countPawnAttacks();
    Test::countKnightMoves();
    Test::countKingMoves();

    Test::countBishopMoves_noEdges();
    Test::countRookMoves_noEdges();

    Test::moveEncodingAndDecoding();
#endif
}