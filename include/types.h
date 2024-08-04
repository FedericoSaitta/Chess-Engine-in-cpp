//
// Created by Federico Saitta on 18/07/2024.
//

#pragma once

#include "macros.h"
#include <cstring>
#include <cstdint>
#include <bit>
#include <iostream>
#include "init.h"
#include <cmath>

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



enum MoveFlags : int {
    QUIET = 0b0000,
    DOUBLE_PUSH = 0b0001,
    OO = 0b0010, OOO = 0b0011,

    PR_KNIGHT = 0b0100, PR_BISHOP = 0b0101, PR_ROOK = 0b0110, PR_QUEEN = 0b0111,

    EN_PASSANT = 0b1010,

    PC_KNIGHT = 0b1100, PC_BISHOP = 0b1101, PC_ROOK = 0b1110, PC_QUEEN = 0b1111,

    CAPTURE = 0b1000,   // All captures (en-passant too) have the fourth bit set
    PROMOTION = 0b0100, // All promotions have the third bit set

    PROMOTIONS = 0b0111,
    //  CAPTURES = 0b1111,
 //   PROMOTION_CAPTURES = 0b1100,

};

class Move {
private:
    uint16_t move;
public:
    inline Move() : move(0) {}

    inline Move(uint16_t m) { move = m; }

    inline Move(const uint16_t from, const uint16_t to) : move(0) {
        move = (from << 6) | to;
    }

    inline Move(const uint16_t from, const uint16_t to, const MoveFlags flags) : move(0) {
        move = (flags << 12) | (from << 6) | to;
    }

    inline int to() const { return int(move & 0x3f); }
    inline int from() const { return int((move >> 6) & 0x3f); }
    inline int to_from() const { return move & 0xffff; }
    inline MoveFlags flags() const { return MoveFlags((move >> 12) & 0xf); }

    inline bool isCapture() const {
        return flags() & CAPTURE;
    }

    // this is also quite sketchy
    inline bool isPromotion() const {
        return flags() & PROMOTION;
    }

    inline bool isEnPassant() const {
        return flags() == EN_PASSANT;
    }

    // Noisy moves are: captures, en-passant and queen promotions
    // Non-Noisy moves are: undepromtions( even capture promotions ) and quiet moves (castling etc)
    inline bool isNoisy() const {
        switch(flags()) {
            case (CAPTURE): return true;
            case (EN_PASSANT): return true;
            case (PC_QUEEN): return true;
            case (PR_QUEEN): return true;
            default: return false;
        }
    }

    // this can be written so much better
    inline PieceType promotionPiece() const {
        return PieceType( std::max( ( flags() & PROMOTIONS) - 3, 0) );
    }

    bool operator==(Move a) const { return to_from() == a.to_from(); }
    bool operator!=(Move a) const { return to_from() != a.to_from(); }
};

