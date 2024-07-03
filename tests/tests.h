//
// Created by Federico Saitta on 29/06/2024.
//

#ifndef TESTS_H
#define TESTS_H

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

    void moveEncodingAndDecoding();
}


#endif //TESTS_H
