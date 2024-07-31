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
    //The bitboard of squares on which pieces have either moved from, or have been moved to. Used for castling
    //legality checks
    U64 castle;

    //The piece that was captured on the last move
    Piece captured;

    //The en passant square. This is the square which pawns can move to in order to en passant capture an enemy pawn that has
    //double pushed on the previous move
    int enPassSq;

    constexpr UndoInfo() : castle(0), captured(NO_PIECE), enPassSq(64) {}

    //This preserves the entry bitboard across moves
    UndoInfo(const UndoInfo& prev) :
        castle(prev.castle), captured(NO_PIECE), enPassSq(64) {}
};

enum MoveFlags : int {
    QUIET = 0b0000, DOUBLE_PUSH = 0b0001,
    OO = 0b0010, OOO = 0b0011,
    CAPTURE = 0b1000,

  //  CAPTURES = 0b1111,
    EN_PASSANT = 0b1010,
 //   PROMOTIONS = 0b0111,
 //   PROMOTION_CAPTURES = 0b1100,
    PR_KNIGHT = 0b0100, PR_BISHOP = 0b0101, PR_ROOK = 0b0110, PR_QUEEN = 0b0111,
    PC_KNIGHT = 0b1100, PC_BISHOP = 0b1101, PC_ROOK = 0b1110, PC_QUEEN = 0b1111,
};

class Move {
private:
    //The internal representation of the move
    uint16_t move;
public:
    //Defaults to a null move (a1a1)
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

    inline bool is_capture() const {
        return (move >> 12) & CAPTURE;
    }

    // this is also quite sketchy
    inline bool is_promotion() const {
        return (move >> 12) > 3;
    }

    // this can be written so much better
    inline PieceType promotionPiece() const {
        switch(move >> 12) {
            case(PR_KNIGHT): return KNIGHT;
            case(PC_KNIGHT): return KNIGHT;

            case(PR_BISHOP): return BISHOP;
            case(PC_BISHOP): return BISHOP;

            case(PR_ROOK): return ROOK;
            case(PC_ROOK): return ROOK;

            case(PR_QUEEN): return QUEEN;
            case(PC_QUEEN): return QUEEN;
            default: ;
        }
        return PAWN;
    }

    bool operator==(Move a) const { return to_from() == a.to_from(); }
    bool operator!=(Move a) const { return to_from() != a.to_from(); }
};


class Board {

public:
    UndoInfo history[256];
    U64 bitboards[15]; // last 3 represent the occupancies

    Piece mailbox[64];

    int gamePly{};

    int side{ WHITE };

    inline U64 getBitboard(const Piece pc) const { return bitboards[pc]; }
    inline U64 getBitboard(const Occupancies occ)  const { return bitboards[occ]; }

    inline U64 getBitboard(const PieceType pc, const Color c) const { return bitboards[pc + 6 * c]; }

    inline void resetBoard() {
        memset(bitboards, 0ULL, sizeof(bitboards));

        for (int i=0; i<64; i++) mailbox[i] = NO_PIECE;

        side = WHITE;
        history[gamePly].enPassSq = 64;
        history[gamePly].castle = 0;
    }
    inline void resetOcc() {
        bitboards[WHITE_OCC] = 0ULL;
        bitboards[BLACK_OCC] = 0ULL;
        bitboards[BOTH_OCC] = 0ULL;
    }

    inline void resetAll() {
        resetBoard();
        resetOcc();
    }

    inline Piece getMovePiece(Move move) {
        return mailbox[move.from()];
    }


    void move_piece(const int from, const int to);
    void move_piece_quiet(const int from, const int to);

    void put_piece(Piece pc, int s);
    void remove_piece(const int s);

    void undo(const Move move);
    int makeMove(const Move move, const int onlyCaptures);

    inline int checkParallel() {
        int countBitboards{};
        int countMailBox{};

        for (int i=0; i < 12; i++) countBitboards += std::popcount(bitboards[i]);

        for (int sq=0; sq<64; sq++) {
            if (mailbox[sq] != NO_PIECE) countMailBox++;
        }

        if (countBitboards != countMailBox) {
            std::cerr << "Counts differ" << std::endl;
            return 1;
        }
        return 0;

    }

};