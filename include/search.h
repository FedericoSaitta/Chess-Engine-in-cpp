//
// Created by Federico Saitta on 10/07/2024.
//

#ifndef SEARCH_H
#define SEARCH_H

#include "movegen.h"
#include "hashtable.h"

extern int ply;
extern int hashFull;
extern U64 repetitionTable[1'000];
extern int repetitionIndex;

int scoreMove(int move, int ply);
void sortMoves(MoveList& moveList, int ply, int bestMove);

void iterativeDeepening(int depth, bool timeConstraint=false);


#endif //SEARCH_H
