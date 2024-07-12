//
// Created by Federico Saitta on 10/07/2024.
//

#pragma once

#include "macros.h"
extern U64 randomPieceKeys[12][64];
extern U64 randomEnPassantKeys[64];
extern U64 randomCastlingKeys[16];
extern U64 sideKey;

extern U64 hashKey; // of the position

#define HASH_SIZE 2'666'666 // 64 Mega Byte transposition table, statically allocated

// https://web.archive.org/web/20071031100051/http://www.brucemo.com/compchess/programming/hashing.htm
#define    HASH_FLAG_EXACT   0 // evaluation from evaluation function
#define    HASH_FLAG_ALPHA   1 // evaluation from alpha
#define    HASH_FLAG_BETA    2 // evaluation from beta

// transposition table data structure
typedef struct tagHASHE {
    U64 hashKey;
    int depth;
    int flag;
    int score;
    int bestMove;
}   tt;

#define NO_HASH_ENTRY 100'000 // large enough to make sure it goes outside alpha beta window size
#define MAX_VALUE 50'000
#define MATE_VALUE 49'000
#define MATE_SCORE 48'000 // this is more like a boundary to the scores

extern tt transpositionTable[HASH_SIZE];

void clearTranspositionTable();
int probeHash(int alpha, int beta, int* best_move, int depth);
void recordHash(int score, int best_move, int flag, int depth);

int checkHashOccupancy();

U64 generateHashKey();

