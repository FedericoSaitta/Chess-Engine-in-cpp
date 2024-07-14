//
// Created by Federico Saitta on 10/07/2024.
//
#pragma once

#include "hashtable.h"

#define MAX_PLY 64

#define INF 50'000
#define MATE_VALUE 49'000
#define MATE_SCORE 48'000 // this is more like a boundary to the scores

extern int ply;
extern U64 repetitionTable[1'000];
extern int repetitionIndex;

extern int scorePV;
extern int pvTable[64][64];

extern int killerMoves[2][128];
extern int historyMoves[12][64];

void initSearchTables();

void iterativeDeepening(int depth, bool timeConstraint=false);

