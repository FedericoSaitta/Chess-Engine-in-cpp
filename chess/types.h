#pragma once

#include <cstdint>
#define U64 std::uint64_t

// Board Square are in Little-Endian format
enum Square{
    A1=0,  B1,  C1,  D1,  E1,  F1,  G1,  H1,
    A2,  B2,  C2,  D2,  E2,  F2,  G2,  H2,
    A3,  B3,  C3,  D3,  E3,  F3,  G3,  H3,
    A4,  B4,  C4,  D4,  E4,  F4,  G4,  H4,
    A5,  B5,  C5,  D5,  E5,  F5,  G5,  H5,
    A6,  B6,  C6,  D6,  E6,  F6,  G6,  H6,
    A7,  B7,  C7,  D7,  E7,  F7,  G7,  H7,
    A8,  B8,  C8,  D8,  E8,  F8,  G8,  H8, NO_SQ
};

enum CastlingRights {

    WK = 1,
    WQ = 2,
    BK = 4,
    BQ = 8,
};

enum Color { WHITE = 0, BLACK = 1 };

constexpr Color operator~(const Color c) {
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

// these return ints because they'll be directly used to index the piece bitboards
inline int pieceType(const Piece pc){ return pc % 6; }
inline int color(const Piece pc){ return pc / 6; }

enum Occupancies {
    WHITE_OCC=12, BLACK_OCC, BOTH_OCC
};


struct UndoInfo {

    U64 castle;
    Piece captured;
    int enPassSq;

    constexpr UndoInfo() : castle(0ULL), captured(NO_PIECE), enPassSq(NO_SQ) {}

    UndoInfo(const UndoInfo& prev) :
        castle(prev.castle), captured(NO_PIECE), enPassSq(NO_SQ) {}

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

