//
// Created by Federico Saitta on 10/07/2024.
//

#pragma once



#include "hashtable.h"
#include "board.h"
#include "init.h"
#include "inline_functions.h"
#include "macros.h"


struct MoveList {
    int moves[256];
    int count;
};

inline void addMove(MoveList& moveList, const int move) {
    moveList.moves[moveList.count] = move;
    moveList.count++;
}

void generateMoves(MoveList& moveList);