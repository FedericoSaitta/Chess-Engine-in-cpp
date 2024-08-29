#pragma once

#include "../src/movegen/movegen.h"
#include "../include/types.h"
#include <vector>
#include "macros.h"
#include "board.h"


void printBitBoard(U64 bb, bool mirrored=false);
void printBoardFancy();

void printAttackedSquares(int side);
void printMove(Move move);
void printMovesList(const MoveList& moveList);

extern const char promotedPieces[];
extern const char* unicodePieces[];

std::string algebraicNotation(Move move);
std::string getStringFlags(MoveFlags flag);

Move parseMove(std::string_view move, const Board& board);
void printHistoryInfo();

std::vector<std::string> split(const std::string& str);
