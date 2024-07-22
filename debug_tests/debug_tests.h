//
// Created by Federico Saitta on 12/07/2024.
//

#ifndef DEBUG_TESTS_H
#define DEBUG_TESTS_H

namespace Test::Debug {
    void countPawnAttacks();
    void countKnightMoves();
    void countKingMoves();
    void countBishopMoves_noEdges();
    void countRookMoves_noEdges();

    void moveEncodingAndDecoding();
    void moveSorting();

    void historyScores();

    void mirrorEval();
    void equalEvalTuner();
}

#endif //DEBUG_TESTS_H
