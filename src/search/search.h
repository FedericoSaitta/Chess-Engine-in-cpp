//
// Created by Federico Saitta on 10/07/2024.
//
#pragma once

#include "../../include/hashtable.h"
#include "../include/types.h"

enum NodeType {
    DONT_NULL=0,
    DO_NULL,
};


#define MAX_PLY 64

#define INF 50'000
#define MATE_VALUE 49'000
#define MATE_SCORE 48'000 // this is more like a boundary to the scores

extern int searchPly;
extern U64 repetitionTable[1'000];
extern int repetitionIndex;

extern int scorePV;
extern Move pvTable[64][64];

extern Move killerMoves[2][128];
extern int historyScores[12][64];

void initSearchTables();
void clearHistoryTable();
void iterativeDeepening(int depth, bool timeConstraint=false);

