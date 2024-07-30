//
// Created by Federico Saitta on 13/07/2024.
//

#pragma once

#include "../movegen/movegen.h"
#include "../include/types.h"
#include <vector>

int scoreMove(Move move);

void giveScores(MoveList& moveList, Move bestMove);

Move pickBestMove(MoveList& moveList, int start);