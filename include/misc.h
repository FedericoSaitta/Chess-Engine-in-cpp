//
// Created by Federico Saitta on 29/06/2024.
//

#ifndef MISC_H
#define MISC_H

#include <iostream>

#include "macros.h"
#include "movegen.h"


void printBitBoard(U64 bb, bool mirrored=false);
void printBoardFancy();

void printAttackedSquares(int side);
void printMove(int move);
void printMovesList(const MoveList& moveList);

extern const char promotedPieces[];

std::string algebraicNotation(int move);


#endif //MISC_H
