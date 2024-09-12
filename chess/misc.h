#pragma once

#include "movegen/movegen.h"
#include "types.h"
#include <vector>
#include "board.h"


void printBitBoard(U64 bb, bool mirrored=false);
void printBoardFancy();

void printAttackedSquares(int side);
void printMovesList(const MoveList& moveList);

extern const char promotedPieces[];
extern const char* unicodePieces[];

std::string algebraicNotation(Move move);
std::string getStringFlags(MoveFlags flag);

Move parseMove(std::string_view move, const Board& board);
void printHistoryInfo();

std::vector<std::string> split(const std::string& str);
