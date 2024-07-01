//
// In this file we declare the variables and functions that are used across multiple files
// these function and variables will have their own definition in their own .cpp file

// Here is the board representation, A1 (the right most bit) is at index 0, this results in the
// board being mirrored vertically if the bitboard is printed out directly
// This is a convention which might have possible mathematical advantages



#ifndef GLOBALS_H
#define GLOBALS_H


#include "constants.h"


//**** board.cpp ****//
extern BITBOARD mask[64];
extern BITBOARD notMask[64];
extern BITBOARD maskCols[64];

extern BITBOARD notAFile;
extern BITBOARD notABFile;
extern BITBOARD notHFile;
extern BITBOARD notHGFile;

// relevant occupancy bit count for every square on the board
extern const int bishopRelevantBits[64];
extern const int rookRelevantBits[64];


extern BITBOARD bitMoves[6][64]; // the indices are P: 0, N: 1, B: 2, R: 3, Q: 4, K: 5

extern BITBOARD pawnAttacks[2][64];

extern BITBOARD bitKnightMoves[64];
extern BITBOARD bitQueenMoves[64];
extern BITBOARD bitKingMoves[64];

extern BITBOARD bitBishopMoves[64];
extern BITBOARD bitRookMoves[64];

extern BITBOARD bitBishopMovesTable[64][512];
extern BITBOARD bitRookMovesTable[64][4096];


void setBit(BITBOARD& bb, int square);
void setBitFalse(BITBOARD& bb, int square);
void setBits();

//**** init.cpp ****//
const extern char piece_char[6];
const extern int piece_value[6];
const extern int init_color[64];
const extern int init_board[64];

BITBOARD maskPawnAttacks(int square, int side); // Maybe i dont need this at the global level
BITBOARD maskKnightMoves(int square);  // these are mostly for debugging
BITBOARD maskKingMoves(int square);
BITBOARD maskBishopMoves(int square);
BITBOARD maskRookMoves(int square);

BITBOARD bishopAttacksOnTheFly(int square, U64 blocker);
BITBOARD rookAttacksOnTheFly(int square, U64 blocker);

void initLeaperPiecesAttacks();
void initSliderAttacks(int bishop);


// this resets the Leftmostbit, in this case it is the most significant bit
// to quickly count the number of bits on a bitboard use the bit hacK
// board &= (board - 1), this is the function to count bits on a bitboard
// inline basically inserts your code inside the block instead of making an actuall "call", can cause code bloating
// use inline for functions that will be called a lot

inline int countBits(BITBOARD board) {
    int count{};
    while (board) { board &= (board - 1); count++; }
    return count;
}

// visually for you gets the rightmost bit, in the tutorial it is the leftmost
// of course this only works for boards that have at least one bit set on them
inline int getLeastSigBitIndex(const BITBOARD board) {

    if (board) {
        return countBits((board & -board) - 1);
    }
    return -1; // in case the bitboard is initially empty
}


BITBOARD setOccupancies(int index, int bitInMask, U64 attackMask);

extern const U64 bishopMagics[64];
extern const U64 rookMagics[64];


inline U64 getBishopAttacks(const int square, U64 occupancy) {
    // get bishop attacks assuming current board occupancy
    occupancy &= bitBishopMoves[square];
    occupancy *= bishopMagics[square];
    occupancy >>= 64 - bishopRelevantBits[square];

    return bitBishopMovesTable[square][occupancy];
}

inline U64 getRookAttacks(const int square, U64 occupancy) {
    // get bishop attacks assuming current board occupancy
    occupancy &= bitRookMoves[square];
    occupancy *= rookMagics[square];
    occupancy >>= 64 - rookRelevantBits[square];

    return bitRookMovesTable[square][occupancy];
}


#endif //GLOBALS_H
