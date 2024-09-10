#pragma once

#include "macros.h"
#include "types.h"
#include "move.h"
#include "board.h"

extern U64 randomPieceKeys[12][64];
extern U64 randomEnPassantKeys[64];
extern U64 randomCastlingKeys[16];
extern U64 sideKey;

extern U64 hashKey; // of the position

// https://web.archive.org/web/20071031100051/http://www.brucemo.com/compchess/programming/hashing.htm
#define    HASH_FLAG_EXACT   0 // evaluation from evaluation function
#define    HASH_FLAG_ALPHA   1 // evaluation from alpha
#define    HASH_FLAG_BETA    2 // evaluation from beta

// transposition table data structure
struct tt {
    U64 hashKey;
    int depth;
    int flag;
    int score;
    Move bestMove;
};

#define NO_HASH_ENTRY 100'000 // large enough to make sure it goes outside alpha beta window size

extern tt* transpositionTable;
extern std::int64_t transpotitionTableEntries;

void initTranspositionTable(int megaBytes);
void clearTranspositionTable();
int probeHash(int alpha, int beta, Move* best_move, int depth, int searchPly);
void recordHash(int score, Move best_move, int flag, int depth, int searchPly);

int checkHashOccupancy();

U64 generateHashKey(const Board& board);
