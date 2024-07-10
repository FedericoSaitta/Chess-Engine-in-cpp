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

void initAll();


#endif //INIT_H
