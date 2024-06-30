//
// Created by Federico Saitta on 29/06/2024.
//

#include "globals.h"

const char piece_char[6] =
{
    'P', 'N', 'B', 'R', 'Q', 'K'
};

const int piece_value[6] =
{
    100, 300, 300, 500, 900, 10000
};

const int init_color[64] =
{
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1
};

const int init_board[64] =
{
    3, 1, 2, 4, 5, 2, 1, 3,
    0, 0, 0, 0, 0, 0, 0, 0,
    6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6,
    0, 0, 0, 0, 0, 0, 0, 0,
    3, 1, 2, 4, 5, 2, 1, 3
};

const int col[64]=
{
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 1, 2, 3, 4, 5, 6, 7
};

const int row[64]=
{
    0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 1,
    2, 2, 2, 2, 2, 2, 2, 2,
    3, 3, 3, 3, 3, 3, 3, 3,
    4, 4, 4, 4, 4, 4, 4, 4,
    5, 5, 5, 5, 5, 5, 5, 5,
    6, 6, 6, 6, 6, 6, 6, 6,
    7, 7, 7, 7, 7, 7, 7, 7
};

const int nwdiag[64]=
{
    14,13,12,11,10, 9, 8, 7,
    13,12,11,10, 9, 8, 7, 6,
    12,11,10, 9, 8, 7, 6, 5,
    11,10, 9, 8, 7, 6, 5, 4,
    10, 9, 8, 7, 6, 5, 4, 3,
     9, 8, 7, 6, 5, 4, 3, 2,
     8, 7, 6, 5, 4, 3, 2, 1,
     7, 6, 5, 4, 3, 2, 1, 0
};

const int nediag[64]=
{
    7, 8,9,10,11,12,13,14,
    6, 7,8, 9,10,11,12,13,
    5, 6,7, 8, 9,10,11,12,
    4, 5,6, 7, 8, 9,10,11,
    3, 4,5, 6, 7, 8, 9,10,
    2, 3,4, 5, 6, 7, 8, 9,
    1, 2,3, 4, 5, 6, 7, 8,
    0, 1,2, 3, 4, 5, 6, 7
};

BITBOARD pawnAttacks[2][64];


BITBOARD maskPawnAttacks(const int square, const int side) {

    U64 attacks{};
    U64 board{};

    setBit(board, square);
    if (!side) { // for white
        if (notHFile & (board << 7)){ attacks |= (board << 7); }
        if (notAFile & (board << 9)){ attacks |= (board << 9); }
    }
    else { // for black
        if (notAFile & (board >> 7)){ attacks |= (board >> 7); }
        if (notHFile & (board >> 9)){ attacks |= (board >> 9); }
    }

    return attacks;
}
BITBOARD maskKnightMoves(const int square) {

    U64 attacks{};
    U64 board{};

    setBit(board, square);

    if ((board << 17) & notAFile) attacks |= board << 17;
    if ((board << 15) & notHFile) attacks |= board << 15;
    if ((board << 10) & notABFile) attacks |= board << 10;
    if ((board << 6) & notHGFile) attacks |= board << 6;

    if ((board >> 17) & notHFile) attacks |= board >> 17;
    if ((board >> 15) & notAFile) attacks |= board >> 15;
    if ((board >> 10) & notHGFile) attacks |= board >> 10;
    if ((board >> 6) & notABFile) attacks |= board >> 6;

    return attacks;
}
BITBOARD maskKingMoves(const int square) {
    U64 attacks{};
    U64 board{};

    setBit(board, square);

    if (board << 8) attacks |= (board << 8);
    if (board >> 8) attacks |= (board >> 8);
    if ((board << 1) & notAFile) {
        attacks |= (board << 1) | (board << 9) | (board >> 7);
    }
    if ((board >> 1) & notHFile) {
        attacks |= (board >> 1) | (board >> 9) | (board << 7);
    }

    return attacks;
}



// Write tests for these two functions


// This generates relevant bishop occupancy squares
// in the mask we do not consider the edge landing squares
// not storing this anywhere at the moment
BITBOARD maskBishopMoves(const int square) {
    U64 attacks{};

    // initial rank and files
    int r{}, f{};

    const int tr{ square / 8 }; //target row
    const int tf{ square % 8 }; //target file

    for (r = tr + 1, f = tf + 1; r < 7 && f < 7; r++, f++) { attacks |= (1ULL << (r * 8 + f)); }

    for (r = tr - 1, f = tf + 1; r > 0 && f < 7; r--, f++) { attacks |= (1ULL << (r * 8 + f)); }
    for (r = tr + 1, f = tf - 1; r < 7 && f > 0; r++, f--) { attacks |= (1ULL << (r * 8 + f)); }

    for (r = tr - 1, f = tf - 1; r > 0 && f > 0; r--, f--) { attacks |= (1ULL << (r * 8 + f)); }

    return attacks;
}

// need to implement this
BITBOARD maskRookMoves(const int square) {
    U64 attacks{};

    // initial rank and files
    int r{}, f{};

    const int tr{ square / 8 }; //target row
    const int tf{ square % 8 }; //target file

    for (r = tr + 1; r < 7; r++) { attacks |= (1ULL << (r * 8 + tf)); }
    for (r = tr - 1; r > 0; r--) { attacks |= (1ULL << (r * 8 + tf)); }

    for (f = tf + 1; f < 7; f++) { attacks |= (1ULL << (tr * 8 + f)); }
    for (f = tf - 1; f > 0; f--) { attacks |= (1ULL << (tr * 8 + f)); }

    return attacks;
}




void maskLeaperPiecesArrays() {

    for (int i = 0; i < 64; i ++) {
        pawnAttacks[0][i] =  maskPawnAttacks(i, 0);
        pawnAttacks[1][i] =  maskPawnAttacks(i, 1);

        bitKnightMoves[i] =  maskKnightMoves(i);
        bitKingMoves[i] =  maskKingMoves(i);
    }
}
