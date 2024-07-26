//
// Created by Federico Saitta on 13/07/2024.
//

#pragma once

#include "../movegen/movegen.h"
#include <vector>

int scoreMove(int move);

void giveScores(MoveList& moveList, int bestMove);

int pickBestMove(MoveList& moveList, int start);