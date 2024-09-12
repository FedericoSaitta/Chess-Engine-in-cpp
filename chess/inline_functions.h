#pragma once

#include "macros.h"
#include "../include/init.h"

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

inline U64 getQueenAttacks(const int square, const U64 occupancy) {

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