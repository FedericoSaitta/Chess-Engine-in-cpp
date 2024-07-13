//
// Created by Federico Saitta on 10/07/2024.
//

#pragma once

#include "movegen.h"
#include "hashtable.h"

// these need to be reset when starting new games or parsing a new FEN
extern int ply;
extern U64 repetitionTable[1'000];
extern int repetitionIndex;

extern int scorePV;
extern int pvTable[64][64];

extern int killerMoves[2][128];
extern int historyMoves[12][64];

void iterativeDeepening(int depth, bool timeConstraint=false);

