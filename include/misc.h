//
// Created by Federico Saitta on 29/06/2024.
//

#ifndef MISC_H
#define MISC_H

#include <iostream>

#include "macros.h"
#include "movegen.h"

U64 mirrorHorizontal (U64 bb);

void printBitBoard(U64 bb, bool mirrored=false);
void printBoardFancy();

void printAttackedSquares(int side);
void printMove(int move);
void printMovesList(const MoveList& moveList);

extern const char promotedPieces[];
extern const char* unicodePieces[];

std::string algebraicNotation(int move);


#endif //MISC_H
