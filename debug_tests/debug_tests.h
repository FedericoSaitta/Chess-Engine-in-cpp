//
// Created by Federico Saitta on 12/07/2024.
//

#ifndef DEBUG_TESTS_H
#define DEBUG_TESTS_H


#include <bit>
#include <iostream>

#include "misc.h"
#include "macros.h"

namespace Test::Debug {
    void countPawnAttacks();
    void countKnightMoves();
    void countKingMoves();
    void countBishopMoves_noEdges();
    void countRookMoves_noEdges();

    void moveEncodingAndDecoding();
    void moveSorting();

    void mirrorEval();
}

#endif //DEBUG_TESTS_H
