//
// Created by Federico Saitta on 13/07/2024.
//

#pragma once

#include "movegen.h"

int scoreMove(int move, int ply);
void sortMoves(MoveList& moveList, int ply, int bestMove);
