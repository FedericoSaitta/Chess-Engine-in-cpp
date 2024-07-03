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

BITBOARD mirrorHorizontal (BITBOARD bb);

void printBitBoard(BITBOARD bb, bool mirrored=false);
void printBoardFancy();

void printAttackedSquares(int side);
void printMove(int move);
void printMovesList(const MoveList& moveList);



#endif //MISC_H
