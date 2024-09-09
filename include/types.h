#pragma once

#include "macros.h"
#include <cstdint>
#include <bit>
#include <iostream>
#include <assert.h>

enum Color { WHITE = 0, BLACK = 1 };

constexpr Color operator~(Color c) {
    return Color(c ^ 1);
}

enum PieceType { PAWN = 0, KNIGHT = 1, BISHOP = 2, ROOK = 3, QUEEN = 4, KING = 5 };

enum Piece {
    WHITE_PAWN=0, WHITE_KNIGHT, WHITE_BISHOP, WHITE_ROOK, WHITE_QUEEN, WHITE_KING,
    BLACK_PAWN, BLACK_KNIGHT, BLACK_BISHOP, BLACK_ROOK, BLACK_QUEEN, BLACK_KING, NO_PIECE,
};

constexpr Piece make_piece(Color c, PieceType pt) {
    return Piece(6 * c + pt);
}

enum Occupancies {
    WHITE_OCC=12, BLACK_OCC, BOTH_OCC
};

// these return ints because they'll be directly used to index the piece bitboards
inline int pieceType(const Piece pc){ return pc % 6; }
inline int color(const Piece pc){ return pc / 6; }

// adding a copy assignment to allow for memset to know what to do would remove warnings
struct UndoInfo {

    U64 castle;
    Piece captured;
    int enPassSq;

    constexpr UndoInfo() : castle(0ULL), captured(NO_PIECE), enPassSq(64) {}

    //This preserves the entry bitboard across moves
    UndoInfo(const UndoInfo& prev) :
        castle(prev.castle), captured(NO_PIECE), enPassSq(64) {}

    // Copy assignment operator
    UndoInfo& operator=(const UndoInfo& other) {
        if (this != &other) {
            castle = other.castle;
            captured = other.captured;
            enPassSq = other.enPassSq;
        }
        return *this;
    }

    void resetUndoInfo() {
        castle = 0ULL;
        captured = NO_PIECE;
        enPassSq = 64;
    }

};