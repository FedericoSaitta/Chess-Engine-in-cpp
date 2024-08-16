//
// Created by Federico Saitta on 10/07/2024.
//
#pragma once

#include "../../include/hashtable.h"
#include "../include/types.h"
#include "board.h"

enum NodeType {
    DONT_NULL=0,
    DO_NULL,
};


#define MAX_PLY 64

#define INF 50'000
#define MATE_VALUE 49'000
#define MATE_SCORE 48'000 // this is more like a boundary to the scores

extern int searchPly;
extern U64 repetitionTable[512];
extern int repetitionIndex;

extern int scorePV;
extern Move pvTable[MAX_PLY][MAX_PLY];

extern Move killerMoves[2][MAX_PLY];
extern int historyScores[64][64];

void initSearchTables();
void clearHistoryTable();

class Searcher {
private:


public:
    Board pos{};
    int quiescenceSearch(int alpha, const int beta);
    int aspirationWindow(int currentDepth, const int previousScore);
    int negamax(int alpha, const int beta, int depth, const NodeType canNull);
    void iterativeDeepening(int depth, bool timeConstraint=false);

};
