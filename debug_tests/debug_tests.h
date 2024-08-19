//
// Created by Federico Saitta on 12/07/2024.
//

#pragma once

#include "../src/search/search.h"

namespace Test::Debug {
    void countPawnAttacks();
    void countKnightMoves();
    void countKingMoves();
    void countBishopMoves_noEdges();
    void countRookMoves_noEdges();

    void moveSorting();

    void printMoveOrdering(Searcher& thread);

    void mirrorEval();
    void equalEvalTuner();

    void gameScenario();
    void boardInCheck();
    void boardNonPawnMat();

    void see_test();
}
