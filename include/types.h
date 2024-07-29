//
// Created by Federico Saitta on 18/07/2024.
//

#pragma once

#include "macros.h"
#include <cstring>
#include <cstdint>

enum Color { WHITE = 0, BLACK = 1 };
enum PieceType { PAWN = 0, KNIGHT = 1, BISHOP = 2, ROOK = 3, QUEEN = 4, KING = 5 };

enum Piece {
    WHITE_PAWN=0, WHITE_KNIGHT, WHITE_BISHOP, WHITE_ROOK, WHITE_QUEEN, WHITE_KING,
    BLACK_PAWN, BLACK_KNIGHT, BLACK_BISHOP, BLACK_ROOK, BLACK_QUEEN, BLACK_KING, EMPTY,
};

enum Occupancies {
    WHITE_OCC=12, BLACK_OCC, BOTH_OCC
};

// these return ints because they'll be directly used to index the piece bitboards
inline int pieceType(const Piece pc){ return pc % 6; }
inline int color(const Piece pc){ return pc / 6; }


struct UndoInfo {
    //The bitboard of squares on which pieces have either moved from, or have been moved to. Used for castling
    //legality checks
    U64 entry;

    //The piece that was captured on the last move
    Piece captured;

    //The en passant square. This is the square which pawns can move to in order to en passant capture an enemy pawn that has
    //double pushed on the previous move
    int enPassSq;

    constexpr UndoInfo() : entry(0), captured(EMPTY), enPassSq(64) {}

    //This preserves the entry bitboard across moves
    UndoInfo(const UndoInfo& prev) :
        entry(prev.entry), captured(EMPTY), enPassSq(64) {}
};

enum MoveFlags : int {
    QUIET = 0b0000'0000,

    W_PR_KNIGHT = 0b0000'0001, W_PR_BISHOP = 0b0000'0010, W_PR_ROOK = 0b0000'0011, W_PR_QUEEN = 0b0000'0100,
    B_PR_KNIGHT = 0b0000'0111, B_PR_BISHOP = 0b0000'1000, B_PR_ROOK = 0b0000'1001, B_PR_QUEEN = 0b0000'1010,

    CAPTURE = 0b0001'0000,

    W_PC_KNIGHT = 0b0001'0001, W_PC_BISHOP = 0b0001'0010, W_PC_ROOK = 0b0001'0011, W_PC_QUEEN = 0b0001'0100,
    B_PC_KNIGHT = 0b0001'0111, B_PC_BISHOP = 0b0001'1000, B_PC_ROOK = 0b0001'1001, B_PC_QUEEN = 0b0001'1010,

    DOUBLE_PUSH = 0b0010'0000,
    EN_PASSANT = 0b0100'0000,
    OO = 0b1000'0000,
};





class Board {

public:
    UndoInfo history[256];

    U64 bitboards[15]; // last 3 represent the occupancies

    int side{ WHITE };
    int enPassantSq{ 64 };
    int castle { 0 };


    inline U64 getBitboard(const Piece pc) const { return bitboards[pc]; }
    inline U64 getBitboard(const Occupancies occ)  const { return bitboards[occ]; }

    inline U64 getBitboard(const PieceType pc, const Color c) const { return bitboards[pc + 6 * c]; }


    inline void resetBoard() {
        memset(bitboards, 0ULL, sizeof(bitboards));
        side = WHITE;
        enPassantSq = 64;
        castle = 0;
    }
    inline void resetOcc() {
        bitboards[WHITE_OCC] = 0ULL;
        bitboards[BLACK_OCC] = 0ULL;
        bitboards[BOTH_OCC] = 0ULL;
    }

};