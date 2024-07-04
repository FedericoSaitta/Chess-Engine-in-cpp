// In this file functions that are called very often by the init and movegen modules are inlined to boost performance

#ifndef INLINEFUNCTIONS_H
#define INLINEFUNCTIONS_H

#include <cstring>
#include "constants.h"
#include "globals.h"

// these functions should be tested as passsing by reference might slow them down? Not sure if they are used a lot
// outside of magic number generation which is pre-processor anyway
inline void setBit(BITBOARD& board, const int square) {
    board |= (1ULL << square);
}
inline void setBitFalse(BITBOARD& board, const int square) {
    board &= ~(1ULL << square);
}
inline bool getBit(const BITBOARD& board, const int square) {
    return (board >> square) & 1ULL;
}

inline int countBits(BITBOARD board) {
    // to quickly count the number of bits on a bitboard use the bit hacK board &= (board - 1)
    int count{};
    while (board) { board &= (board - 1); count++; }
    return count;
}
inline int getLeastSigBitIndex(const BITBOARD board) {
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
    occupancy >>= 64 - bishopRelevantBits[square];

    return bitBishopAttacksTable[square][occupancy];
}
inline U64 getRookAttacks(const int square, U64 occupancy) {
    // get bishop attacks assuming current board occupancy
    occupancy &= bitRookAttacks[square];
    occupancy *= rookMagics[square];
    occupancy >>= 64 - rookRelevantBits[square];

    return bitRookAttacksTable[square][occupancy];
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
    if ( (side == White) && (pawnAttacks[Black][square] & bitboards[Pawn]) ) return 1;
    if ( (side == Black) && (pawnAttacks[White][square] & bitboards[Pawn + 6]) ) return 1;

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


inline int encodeMove(const int start, const int target, const int piece, const int promoted,
                      const int capture, const int doublePush, const int enPassant, const int castling) {

    return start | (target << 6) | (piece << 12) | (promoted << 16) | (capture << 20) | (doublePush << 21) | (enPassant << 22) | (castling << 23);
}

inline int getMoveStartSQ(const int move) { return (move & 0x3f); }
inline int getMoveTargettSQ(const int move) { return ((move & 0xfc0) >> 6); }
inline int getMovePiece(const int move) { return ((move & 0xf000) >> 12); }
inline int getMovePromPiece(const int move) { return ((move & 0xf0000) >> 16); }
inline int getMoveCapture(const int move) { return (move & 0x100000); }
inline int getMoveDoublePush(const int move) { return (move & 0x200000); }
inline int getMoveEnPassant(const int move) { return (move & 0x400000); }
inline int getMoveCastling(const int move) { return (move & 0x800000); }

inline void addMove(MoveList& moveList, const int move) {
    moveList.moves[moveList.count] = move;
    moveList.count++;
}

/*
inline void copyBoard() {
    std::memcpy(bitboardsCopy, bitboards, 96); // Copy 96 bytes (bitboards are 8 bytes each)
    std::memcpy(occupanciesCopy, occupancies, 24); // Copy 24 bytes (occupancies are 8 bytes each)
    sideCopy = side;
    enPassantCopy = enPassantSQ;
    castleCopy = castle;
}

inline void restoreBoard() {
    std::memcpy(bitboards, bitboardsCopy, 96); // Copy 96 bytes (bitboards are 8 bytes each)
    std::memcpy(occupancies, occupanciesCopy, 24); // Copy 24 bytes (occupancies are 8 bytes each)
    side = sideCopy;
    enPassantSQ = enPassantCopy;
    castle = castleCopy;
}

*/

#define copyBoard()                                                      \
U64 bitboards_copy[12], occupancies_copy[3];                          \
int side_copy, enpassant_copy, castle_copy;                           \
memcpy(bitboards_copy, bitboards, 96);                                \
memcpy(occupancies_copy, occupancies, 24);                            \
side_copy = side, enpassant_copy = enPassantSQ, castle_copy = castle;   \

// restore board state
#define restoreBoard()                                                       \
memcpy(bitboards, bitboards_copy, 96);                                \
memcpy(occupancies, occupancies_copy, 24);                            \
side = side_copy, enPassantSQ = enpassant_copy, castle = castle_copy;   \



#endif //INLINEFUNCTIONS_H
