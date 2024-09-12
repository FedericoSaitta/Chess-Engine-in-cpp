#pragma once

#include "types.h"
#include <cstdint>
#include <algorithm>
#include <iostream>

#include <assert.h>

enum MoveFlags : int {
    QUIET = 0b0000,
    DOUBLE_PUSH = 0b0001,
    OO = 0b0010, OOO = 0b0011,

    PROMOTION = 0b0100, // All promotions have the third bit set

    PR_KNIGHT = 0b0100, PR_BISHOP = 0b0101, PR_ROOK = 0b0110, PR_QUEEN = 0b0111,

    CAPTURE = 0b1000,   // All captures (en-passant too) have the fourth bit set

    EN_PASSANT = 0b1010,
    PC_KNIGHT = 0b1100, PC_BISHOP = 0b1101, PC_ROOK = 0b1110, PC_QUEEN = 0b1111,

    PROMOTION_MASK = 0b0111,
};

class Move {
private:
    uint16_t move;

public:
    Move() : move(0) {}

    Move(uint16_t m) { move = m; }

    Move(const int from, const int to) : move(0) {
        move = static_cast<std::uint16_t>( (from << 6) | to );
    }

    Move(const int from, const int to, const MoveFlags flags) : move(0) {
        move = static_cast<std::uint16_t>( (flags << 12) | (from << 6) | to);
    }

    int to() const { return int(move & 0x3f); }
    int from() const { return int((move >> 6) & 0x3f); }
    int to_from() const { return move & 0xffff; }
    MoveFlags flags() const { return MoveFlags((move >> 12) & 0xf); }

    bool isCapture() const {
        return flags() & CAPTURE;
    }

    bool isPromotion() const { return flags() & PROMOTION; }
    bool isQueenPromotion() const { return (flags() & PROMOTION_MASK) == 0b0111; }
    bool isEnPassant() const { return flags() == EN_PASSANT; }
    bool isCastling() const { return (flags() == OO) || (flags() == OOO); }

    // Noisy moves are: captures, en-passant and queen promotions
    // Non-Noisy moves are: undepromtions( even capture promotions ) and quiet moves (castling etc)
    bool isNoisy() const {
        return isCapture() || isPromotion();
    }

    // this can be written so much better
    PieceType promotionPiece() const {
        assert( (std::max( ( flags() & PROMOTION_MASK) - 3, 0) < KING) && "promotionPiece: is wrong");

        return PieceType( std::max( ( flags() & PROMOTION_MASK) - 3, 0) );
    }

    bool isNone() const { return (to_from() == 0); }
    bool operator==(const Move a) const { return to_from() == a.to_from(); }
    bool operator!=(const Move a) const { return to_from() != a.to_from(); }


    friend std::ostream& operator<<(std::ostream& os, const Move& move);

    // Static member declaration
    static const Move Null;
};


