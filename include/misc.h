//
// Created by Federico Saitta on 29/06/2024.
//

#ifndef MISC_H
#define MISC_H

#include "macros.h"
#include "globals.h"


BITBOARD mirrorHorizontal (BITBOARD bb);

void printBitBoard(BITBOARD bb, bool mirrored=false);
void printBoardFancy();

void printAttackedSquares(int side);
void printMove(int move);
void printMovesList(const MoveList& moveList);

std::string algebraicNotation(int move);


#endif //MISC_H
