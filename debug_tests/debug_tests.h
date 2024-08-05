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

    void moveSorting();

    void printMoveOrdering();


    void mirrorEval();
    void equalEvalTuner();

    void gameScenario();
}

#endif //DEBUG_TESTS_H
