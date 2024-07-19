#include <fstream>
#include <iostream>

#include "uci.h"
#include "init.h"

// #define DEBUG
 #define BENCHMARK


#include "benchmark_tests.h"
#include "debug_tests.h"
#include "movegen.h"
#include "eval/evalparams.h"

int main() {
    const std::string logFilePath{ "/Users/federicosaitta/CLionProjects/ChessEngine/logfile.txt" };
    logFile.open(logFilePath, std::ios::app);

    initAll(256); // Done at compile time :), using 256 MB size hash

   // UCI();

    logFile.close();

    // nice article: https://web.archive.org/web/20071030220825/http://www.brucemo.com/compchess/programming/pvs.htm
    // nice transposition table: https://web.archive.org/web/20071031100051/http://www.brucemo.com/compchess/programming/hashing.htm

    // good chess article:, really nice article for move generation
    // https://peterellisjones.com/posts/generating-legal-chess-moves-efficiently/


#ifdef BENCHMARK
    Test::BenchMark::staticSearch();
    Test::BenchMark::matingPuzzles();
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
/*
 *V3 values for static search test
 *Nodes: 7.69035 Million
AVG Branching Ratio: 3.17528
Time taken: 8.29306s
info score cp -54 depth 1 nodes 74 nps 1430311 time 0 pv f6d5
info score cp -54 depth 2 nodes 196 nps 2578099 time 0 pv f6d5  f5d5
info score cp -189 depth 3 nodes 1079 nps 7180026 time 0 pv f6d5  f5f2  h2h1  e6d5
info score cp -174 depth 4 nodes 1563 nps 4255064 time 0 pv f6d5  f5f2  h2h1  e6d5  h4d8
bestmove f6d5
Mating Move: h4h6 Time taken: 0.00103028ss

info score cp -243 depth 1 nodes 235 nps 1588814 time 0 pv d2d8
info score cp -243 depth 2 nodes 715 nps 1995306 time 0 pv d2d8  c8d8  d1d8
info score cp -202 depth 3 nodes 829 nps 10137945 time 0 pv d2d8  c8d8  d1d8  g8g7  h5h6
info score mate 3 depth 4 nodes 970 nps 15166204 time 0 pv d2d8  c8d8  d1d8  g8g7  h5h6
info score mate 3 depth 5 nodes 1114 nps 16375610 time 0 pv d2d8  c8d8  d1d8  g8g7  h5h6
info score mate 3 depth 6 nodes 1478 nps 10059144 time 0 pv d2d8  c8d8  d1d8  g8g7  h5h6
bestmove d2d8
Mating Move: d2d8 Time taken: 0.000934757ss

info score cp -629 depth 1 nodes 284 nps 1488703 time 0 pv e5d6
info score cp -618 depth 2 nodes 723 nps 2343666 time 0 pv h7h8  f8e7  e5d6
info score cp -544 depth 3 nodes 1247 nps 8196291 time 0 pv h7h8  f8e7  h8f6  e7e8  e5d6
info score cp -449 depth 4 nodes 1765 nps 10301577 time 0 pv h7h8  f8e7  h8f6  e7e8  h4h8  e8d7  f6f7
info score mate 4 depth 5 nodes 2232 nps 14428578 time 0 pv h7h8  f8e7  h8f6  e7e8  h4h8  e8d7  f6d6
info score mate 4 depth 6 nodes 2818 nps 11373084 time 0 pv h7h8  f8e7  h8f6  e7e8  h4h8  e8d7  f6d6
info score mate 4 depth 7 nodes 3494 nps 12605983 time 0 pv h7h8  f8e7  h8f6  e7e8  h4h8  e8d7  f6d6
info score mate 4 depth 8 nodes 4859 nps 7668368 time 0 pv h7h8  f8e7  h8f6  e7e8  h4h8  e8d7  f6d6
bestmove h7h8
Mating Move: h7h8 Time taken: 0.00267066ss

info score cp -101 depth 1 nodes 73 nps 985980 time 0 pv h4g3
info score cp -114 depth 2 nodes 433 nps 1394659 time 0 pv h4g3  e5d5  d3c4
info score cp -112 depth 3 nodes 720 nps 2833374 time 0 pv h4g3  e5f5  e2f3  f5g6  d3e4
info score cp -112 depth 4 nodes 1148 nps 2221998 time 0 pv h4g3  e5f5  e2f3  f5g6  d3e4  f6e4  f3e4
info score cp -80 depth 5 nodes 2429 nps 1576408 time 1 pv h4g3  e5f5  e2f3  f5g5  d3e4  f6e4  f3e4
info score cp -31 depth 6 nodes 8478 nps 1749708 time 4 pv f2f4  e5f5  f1g3  f5g6  g3e4  f6e4  d3e4  f7f5  e2g4
info score cp -36 depth 7 nodes 11882 nps 4344170 time 2 pv f2f4  e5f5  f1g3  f5g6  g3e4  f6e4  d3e4  f7f5  h4e7  d8e7
info score cp -33 depth 8 nodes 27182 nps 2397528 time 11 pv f2f4  e5f5  f1g3  f5g6  g3e4  f6e4  d3e4  f7f5  h4e7  d8e7  e2g4
bestmove f2f4
Mating Move: e2f3 Time taken: 0.0217497ss

info score cp 1155 depth 1 nodes 461 nps 1377665 time 0 pv g8f7
info score cp 1178 depth 2 nodes 1982 nps 1506578 time 1 pv c4d5  e4d5
info score cp 1178 depth 3 nodes 2182 nps 10614390 time 0 pv c4d5  e4d5  g8d5
info score cp 1182 depth 4 nodes 2841 nps 4766539 time 0 pv c4d5  e4d5  g8d5  h1g1
info score cp 1226 depth 5 nodes 3930 nps 4041260 time 0 pv c4d5  e4d5  g8d5  e3f4  b8e8
info score cp 1260 depth 6 nodes 7315 nps 1972946 time 3 pv c4d5  e4d5  g8d5  f2f3  d5f5  b1a2  b8e8
info score cp 1257 depth 7 nodes 15202 nps 1653876 time 9 pv c4d5  e4d5  g8d5  g2g4  d5e4  b1a2  e4d5  a2b1  b8e8
info score mate 5 depth 8 nodes 23354 nps 115251000 time 0 pv c4a2  b1a1  d4b3  a1a2  b3c1  a2b1  g8a2  b1c1  a2b2
info score mate 5 depth 9 nodes 24338 nps 41193460 time 0 pv c4a2  b1a1  d4b3  a1a2  b3c1  a2b1  g8a2  b1c1  a2b2
info score mate 5 depth 10 nodes 26624 nps 19413978 time 1 pv c4a2  b1a1  d4b3  a1a2  b3c1  a2b1  g8a2  b1c1  a2b2
bestmove c4a2
Mating Move: c4a2 Time taken: 0.0298854ss

info score cp -718 depth 1 nodes 166 nps 1346146 time 0 pv h4e1
info score cp -718 depth 2 nodes 372 nps 2158311 time 0 pv h4e1  h1e1
info score cp -718 depth 3 nodes 499 nps 5488100 time 0 pv h4e1  h1e1  d4c2
info score cp -736 depth 4 nodes 1068 nps 2593032 time 0 pv h4e1  h1e1  d4c2  e1c1
info score cp -688 depth 5 nodes 1587 nps 4095114 time 0 pv h4e1  h1e1  d4c2  e1c1  g4e3
info score cp -569 depth 6 nodes 5246 nps 2900121 time 1 pv h4e1  h1e1  d4c2  e1e2  g4e3  g2f3  c8h3
info score cp -517 depth 7 nodes 7716 nps 7298835 time 1 pv h4e1  h1e1  d4c2  e1g1  g4e3  g2h1  e3d5  e4d5
info score mate 5 depth 8 nodes 17752 nps 4490083 time 3 pv h4h3  g2h3  g4e3  d5e6  c8e6  h3h4  d4f3  h4h5  e6g4
info score mate 5 depth 9 nodes 18024 nps 59322456 time 0 pv h4h3  g2h3  g4e3  d5e6  c8e6  h3h4  d4f3  h4h5  e6g4
info score mate 5 depth 10 nodes 32074 nps 3350455 time 9 pv h4h3  g2h3  g4e3  d5e6  c8e6  h3h4  d4f3  h4h5  e6g4
bestmove h4h3
Mating Move: h4h3 Time taken: 0.0223306ss


 */