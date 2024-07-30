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
    std::pair<Move, int> moves[256];
    int count;
};

inline void addMove(MoveList& moveList, const Move move) {
    moveList.moves[moveList.count].first = move;
    moveList.count++;
}

void generateMoves(MoveList& moveList);