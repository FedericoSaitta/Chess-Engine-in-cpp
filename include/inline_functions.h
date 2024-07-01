//
// Created by Federico Saitta on 01/07/2024.
//

#ifndef INLINEFUNCTIONS_H
#define INLINEFUNCTIONS_H

#include "constants.h"
#include "globals.h"


inline void setBit(BITBOARD& board, const int square) {
    board |= (1ULL << square);
}

inline void setBitFalse(BITBOARD& board, const int square) {
    board &= ~(1ULL << square);
}

// this resets the Leftmostbit, in this case it is the most significant bit
// to quickly count the number of bits on a bitboard use the bit hacK
// board &= (board - 1), this is the function to count bits on a bitboard
// inline basically inserts your code inside the block instead of making an actuall "call", can cause code bloating
// use inline for functions that will be called a lot

inline int countBits(BITBOARD board) {
    int count{};
    while (board) { board &= (board - 1); count++; }
    return count;
}

// visually for you gets the rightmost bit, in the tutorial it is the leftmost
// of course this only works for boards that have at least one bit set on them
inline int getLeastSigBitIndex(const BITBOARD board) {

    if (board) {
        return countBits((board & -board) - 1);
    }
    return -1; // in case the bitboard is initially empty
}


inline U64 getBishopAttacks(const int square, U64 occupancy) {
    // get bishop attacks assuming current board occupancy
    occupancy &= bitBishopMoves[square];
    occupancy *= bishopMagics[square];
    occupancy >>= 64 - bishopRelevantBits[square];

    return bitBishopMovesTable[square][occupancy];
}


inline U64 getRookAttacks(const int square, U64 occupancy) {
    // get bishop attacks assuming current board occupancy
    occupancy &= bitRookMoves[square];
    occupancy *= rookMagics[square];
    occupancy >>= 64 - rookRelevantBits[square];

    return bitRookMovesTable[square][occupancy];
}


#endif //INLINEFUNCTIONS_H
