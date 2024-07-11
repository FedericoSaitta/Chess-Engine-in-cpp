//
// Created by Federico Saitta on 10/07/2024.
//

#ifndef INIT_H
#define INIT_H

#include "macros.h"
extern const int bishopRelevantBits[64];
extern const int rookRelevantBits[64];

extern const U64 bishopMagics[64];
extern const U64 rookMagics[64];

extern U64 notAFile;
extern U64 notABFile;
extern U64 notHFile;
extern U64 notHGFile;

extern U64 bitPawnAttacks[2][64];
extern U64 bitKnightAttacks[64];
extern U64 bitKingAttacks[64];

extern U64 bitBishopAttacks[64];
extern U64 bitRookAttacks[64];
extern U64 bitQueenAttacks[64];

extern U64 bitBishopAttacksTable[64][512];
extern U64 bitRookAttacksTable[64][4096];

extern U64 fileMasks[64];
extern U64 rankMasks[64];
extern U64 isolatedPawnMasks[64];
extern U64 white_passedPawnMasks[64];
extern U64 black_passedPawnMasks[64];

extern const int getRankFromSquare[64];

// THESE SHOULD BE TUNED!!!!!!! //
inline extern const int doublePawnPenalty = -10;
inline extern const int isolatedPawnPenalty = -10;
inline extern const int passedPawnBonus[8] {0, 5, 10, 20, 35, 60, 100, 200};

void initAll();


#endif //INIT_H
