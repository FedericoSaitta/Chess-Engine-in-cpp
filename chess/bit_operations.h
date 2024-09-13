#pragma once

#include <assert.h>


inline bool getBit(const U64 bb, const int square) {
    assert(A1 <= square && square < NO_SQ);
    return bb & (1ull << square);
}

inline void setBit(U64& board, const int square) {
    assert(!getBit(board, square));
    board |= (1ULL << square);
}

inline void setBitFalse(U64& board, const int square) {
    assert(getBit(board, square));
    board &= ~(1ULL << square);
}


inline int countBits(const U64 b) {
    return __builtin_popcountll(b);
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
inline int popLSB(U64* b) {
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


inline bool several(const U64 bb) {
    return bb & (bb - 1);
}

inline bool onlyOne(const U64 bb) {
    return bb && !several(bb);
}
