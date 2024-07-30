//
// Created by Federico Saitta on 29/06/2024.
//

#pragma once

#include "../src/movegen/movegen.h"
#include "../include/types.h"
#include <iostream>
#include <vector>
#include "macros.h"


void printBitBoard(U64 bb, bool mirrored=false);
void printBoardFancy();

void printAttackedSquares(int side);
void printMove(Move move);
void printMovesList(const MoveList& moveList);

extern const char promotedPieces[];
extern const char* unicodePieces[];

std::string algebraicNotation(Move move);

std::vector<std::string> split(const std::string& str);
