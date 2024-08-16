//
// Created by Federico Saitta on 10/07/2024.
//

#pragma once

#include "macros.h"
#include "init.h"


// there are more complicated bit counting techniques but this actually all gets
// optimized by the compiler
inline int countBits(U64 b) {
    // to quickly count the number of bits on a bitboard use the bit hacK board &= (board - 1)
    int count{};
    while (b) { b &= (b - 1); count++; }
    return count;
}


constexpr int DEBRUIJN64[64] = {
    0, 47,  1, 56, 48, 27,  2, 60,
   57, 49, 41, 37, 28, 16,  3, 61,
   54, 58, 35, 52, 50, 42, 21, 44,
   38, 32, 29, 23, 17, 11,  4, 62,
   46, 55, 26, 59, 40, 36, 15, 53,
   34, 51, 20, 43, 31, 22, 10, 45,
   25, 39, 14, 33, 19, 30,  9, 24,
   13, 18,  8, 12,  7,  6,  5, 63
};

constexpr U64 MAGIC = 0x03f79d71b4cb0a89;

//Returns the index of the least significant bit in the bitboard
constexpr int bsf(const U64 b) {
    return DEBRUIJN64[MAGIC * (b ^ (b - 1)) >> 58];
}

//Returns the index of the least significant bit in the bitboard, and removes the bit from the bitboard
inline int pop_lsb(U64* b) {
    const int lsb = bsf(*b);
    *b &= *b - 1;
    return lsb;
}

// should write a test for these but it looks good for now
inline U64 shiftLeft(const U64 b) {
    return b >> 1 & notHFile;
}

inline U64 shiftRight(const U64 b) {
    return b << 1 & notAFile;
}


inline U64 getBishopAttacks(const int square, U64 occupancy) {
    // get bishop attacks assuming current board occupancy
    occupancy &= bitBishopAttacks[square];
    occupancy *= bishopMagics[square];
    occupancy >>= (64 - bishopRelevantBits[square]);

    return bitBishopAttacksTable[square][occupancy];
}
inline U64 getBishopXrayAttacks(const int square, const U64 occupancy, U64 blockers) {
    const U64 attacks { getBishopAttacks(square, occupancy) };
    blockers &= attacks;
    return attacks ^ getBishopAttacks(square, occupancy ^ blockers);
}

inline U64 getRookAttacks(const int square, U64 occupancy) {
    // get bishop attacks assuming current board occupancy
    occupancy &= bitRookAttacks[square];
    occupancy *= rookMagics[square];
    occupancy >>= (64 - rookRelevantBits[square]);

    return bitRookAttacksTable[square][occupancy];
}
inline U64 getRookXrayAttacks(const int square, const U64 occupancy, U64 blockers) {
    const U64 attacks { getRookAttacks(square, occupancy) };
    blockers &= attacks;
    return attacks ^ getRookAttacks(square, occupancy ^ blockers);
}

inline U64 getQueenAttacks(const int square, U64 occupancy) {

    U64 bishopOccupancy{ occupancy };
    U64 rookOccupancy{ occupancy };

    bishopOccupancy &= bitBishopAttacks[square];
    bishopOccupancy *= bishopMagics[square];
    bishopOccupancy >>= 64 - bishopRelevantBits[square];

    rookOccupancy &= bitRookAttacks[square];
    rookOccupancy *= rookMagics[square];
    rookOccupancy >>= 64 - rookRelevantBits[square];

    return (bitBishopAttacksTable[square][bishopOccupancy] | bitRookAttacksTable[square][rookOccupancy]);
}