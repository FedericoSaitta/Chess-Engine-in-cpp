//
// Created by Federico Saitta on 29/06/2024.
//

#ifndef MISC_H
#define MISC_H

#include "constants.h"
#include "globals.h"

extern const char asciiPieces[];
extern const int charPieces[];
extern const char* unicodePieces[];

void printBitBoard(BITBOARD bb, bool mirrored=false);
BITBOARD mirrorHorizontal (BITBOARD bb);  // Returns a new copy of the bitboard that has been mirrored
void printBoardFancy();



#endif //MISC_H
