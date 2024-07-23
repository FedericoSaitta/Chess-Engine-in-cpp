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
    BLACK_PAWN, BLACK_KNIGHT, BLACK_BISHOP, BLACK_ROOK, BLACK_QUEEN, BLACK_KING,
};

enum Occupancies {
    WHITE_OCC=12, BLACK_OCC, BOTH_OCC
};

// these return ints because they'll be directly used to index the piece bitboards
inline int pieceType(const Piece pc){ return pc % 6; }
inline int color(const Piece pc){ return pc / 6; }


class Board {

public:
    U64 bitboards[15]; // last 3 represent the occupancies

    int side{ WHITE };
    int enPassantSq{ 64 };
    int castle { 0 };


    inline int getBitboard(const Piece pc) const { return bitboards[pc]; }
    inline int getBitboard(const Occupancies occ)  const { return bitboards[occ]; }

    inline int getBitboard(const PieceType pc, const Color c) const { return bitboards[pc + 6 * c]; }


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