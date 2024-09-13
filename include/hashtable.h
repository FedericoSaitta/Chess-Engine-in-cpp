#pragma once

#include "../chess/types.h"
#include "../chess/move.h"
#include "../chess/board.h"

extern U64 randomPieceKeys[12][64];
extern U64 randomEnPassantKeys[64];
extern U64 randomCastlingKeys[16];
extern U64 sideKey;

// https://web.archive.org/web/20071031100051/http://www.brucemo.com/compchess/programming/hashing.htm
#define    HASH_FLAG_EXACT   0 // evaluation from evaluation function
#define    HASH_FLAG_ALPHA   1 // evaluation from alpha
#define    HASH_FLAG_BETA    2 // evaluation from beta

#define NO_HASH_ENTRY 100'000 // large enough to make sure it goes outside alpha beta window size

// transposition table data structure
struct tt {
    U64 hashkey;
    int depth;
    int flag;
    int score;
    Move bestMove;
};



extern tt* transpositionTable;
extern std::int64_t transpotitionTableEntries;

void initTranspositionTable(int megaBytes);
void clearTranspositionTable();
int probeHash(int alpha, int beta, Move* best_move, int depth, int searchPly, U64 hashkey);
void recordHash(int score, Move best_move, int flag, int depth, int searchPly, U64 hashkey);

int checkHashOccupancy();

U64 generatehashkey(const Board& board);
