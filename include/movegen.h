//
// Created by Federico Saitta on 10/07/2024.
//

#pragma once
#include "movegen.h"


struct MoveList {
    int moves[218];
    int count;
};

inline void addMove(MoveList& moveList, const int move) {
    moveList.moves[moveList.count] = move;
    moveList.count++;
}

void generateMoves(MoveList& moveList);

