//
// Created by Federico Saitta on 29/06/2024.
//

#ifndef TESTS_H
#define TESTS_H

#include <bit>
#include <iostream>

#include "globals.h"
#include "misc.h"
#include "macros.h"

namespace Test{
    void countPawnAttacks();
    void countKnightMoves();
    void countKingMoves();
    void countBishopMoves_noEdges();
    void countRookMoves_noEdges();

    void moveEncodingAndDecoding();
    void moveSorting();

    void perftDriver(int depth);
    std::uint32_t perft(int depth);
    void standardizedPerft();

}


#endif //TESTS_H
