//
// Created by Federico Saitta on 13/07/2024.
//

#pragma once

#include "../movegen/movegen.h"
#include "../include/types.h"
#include "../include/board.h"

int scoreMove(Move move, const Board& board);

void giveScores(MoveList& moveList, Move bestMove, const Board& board);

std::pair<Move, int> pickBestMove(MoveList& moveList, int start);