#pragma once

#include "types.h"


struct MoveList {
    std::pair<Move, int> moves[256];
    int count;
};

inline void addMove(MoveList& moveList, const Move move) {
    moveList.moves[moveList.count].first = move;
    moveList.count++;
}