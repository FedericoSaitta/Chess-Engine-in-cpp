//
// Created by Federico Saitta on 29/06/2024.
//

#ifndef BITMASK_TESTS_H
#define BITMASK_TESTS_H

#include <bit>
#include <iostream>

#include "globals.h"
#include "misc.h"
#include "constants.h"

namespace Test{
    void countPawnAttacks();
    void countKnightMoves();
    void countKingMoves();
    void countBishopMoves_noEdges();
    void countRookMoves_noEdges();

    // ultimately the perft function will decide if the magic bitboards are correct
}


#endif //BITMASK_TESTS_H
