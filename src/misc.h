//
// Created by Federico Saitta on 29/06/2024.
//

#ifndef MISC_H
#define MISC_H


#include "globals.h"

void printBitBoard(BITBOARD& bb, bool mirrored=false);
BITBOARD mirrorHorizontal (BITBOARD bb);  // Returns a new copy of the bitboard that has been mirrored

#endif //MISC_H
