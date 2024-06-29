//
// Created by Federico Saitta on 29/06/2024.
//

#include "misc.h"

#include "globals.h"
#include <iostream>
// File used for miscellanous functions such as a simple GUI and printing out bitboards etc,
// these could go under tests but there we will put more rigorous testing functions

// prints the bitboard, run bb through mirrorHorizontal to obtain normal view of the board (A1: bottom left)
void printBitBoard(BITBOARD& bb) {

    std::cout << '\n';
    for (int i = 63; i >= 0; --i) {
        std::cout << ((bb >> i) & 1);

        if (i % 8 == 0){ std::cout << '\n'; }
    }
}

// from https://www.chessprogramming.org/Flipping_Mirroring_and_Rotating#Mirror_Horizontally
void mirrorHorizontal (BITBOARD& bb) {
    constexpr U64 k1 = 0x5555555555555555;
    constexpr U64 k2 = 0x3333333333333333;
    constexpr U64 k4 = 0x0f0f0f0f0f0f0f0f;
    bb = ((bb >> 1) & k1) | ((bb & k1) << 1);
    bb = ((bb >> 2) & k2) | ((bb & k2) << 2);
    bb = ((bb >> 4) & k4) | ((bb & k4) << 4);
}