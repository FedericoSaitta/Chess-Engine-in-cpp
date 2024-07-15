//
// Created by Federico Saitta on 10/07/2024.
//

#pragma once

#include "macros.h"
#include "board.h"
#include "init.h"

inline int countBits(U64 board) {
    // to quickly count the number of bits on a bitboard use the bit hacK board &= (board - 1)
    int count{};
    while (board) { board &= (board - 1); count++; }
    return count;
}

inline int getLeastSigBitIndex(const U64 board) {
    // this returned the index of the right most bit in a Bitboard
    if (board) {
        return countBits((board & -board) - 1);
    }
    return -1; // in case the bitboard is initially empty
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

inline int isSqAttacked(const int square, const int side) {

    // attacked by pawns
    if ( (side == White) && (bitPawnAttacks[Black][square] & bitboards[Pawn]) ) return 1;
    if ( (side == Black) && (bitPawnAttacks[White][square] & bitboards[Pawn + 6]) ) return 1;

    // attacked by knight
    if ( bitKnightAttacks[square] & ((side == White) ? bitboards[Knight] : bitboards[Knight + 6] ) ) return 1;

    // attacked by bishop
    if ( (getBishopAttacks(square, occupancies[2]) & ((side == White) ? bitboards[Bishop] : bitboards[Bishop+6])) ) return 1;

    // attacked by rook
    if ( (getRookAttacks(square, occupancies[2]) & ((side == White) ? bitboards[Rook] : bitboards[Rook+6])) ) return 1;

    // attacked by queen
    if ( (getQueenAttacks(square, occupancies[2]) & ((side == White) ? bitboards[Queen] : bitboards[Queen+6])) ) return 1;

    // attacked by king
    if ( bitKingAttacks[square] & ((side == White) ? bitboards[King] : bitboards[King + 6] ) ) return 1;

    return 0;
}

inline int getCapturedPiece(const int targetSquare) {
    int startPiece, endPiece;
    int targetPiece{};

    if (side == White ) { startPiece = Pawn + 6; endPiece = King + 6; }
    else { startPiece = Pawn; endPiece = King; }

    for (int bbPiece=startPiece; bbPiece <= endPiece; bbPiece++) {
        if ( GET_BIT(bitboards[bbPiece], targetSquare) ) {
            targetPiece = bbPiece;
            break;
        }
    }
    return targetPiece;
}
