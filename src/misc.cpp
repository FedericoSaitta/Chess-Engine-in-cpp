//
// Created by Federico Saitta on 29/06/2024.
//

#include "misc.h"

#include "globals.h"
#include <iostream>
// File used for miscellanous functions such as a simple GUI and printing out bitboards etc,
// these could go under tests but there we will put more rigorous testing functions

void printBitBoard(BITBOARD& bb) {

    std::cout << '\n';
    for (int i = 63; i >= 0; --i) {

        std::cout << ((bb >> i) & 1);

        if (i % 8 == 0){ std::cout << '\n'; }
    }
}