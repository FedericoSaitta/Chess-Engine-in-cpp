//
// In this file we declare the variables and functions that are used across multiple files
// these function and variables will have their own definition in their own .cpp file

// Here is the board representation, A1 (the right most bit) is at index 0, this results in the
// board being mirrored vertically if the bitboard is printed out directly
// This is a convention which might have possible mathematical advantages
#ifndef GLOBALS_H
#define GLOBALS_H

#include "constants.h"

// //////////////// //
//**** board.cpp ****//
// //////////////// //
extern BITBOARD mask[64];
extern BITBOARD notMask[64];
extern BITBOARD maskCols[64];

extern BITBOARD notAFile;
extern BITBOARD notABFile;
extern BITBOARD notHFile;
extern BITBOARD notHGFile;

const extern char piece_char[6];
const extern int piece_value[6];
const extern int init_color[64];
const extern int init_board[64];

extern BITBOARD bitMoves[6][64]; // the indices are P: 0, N: 1, B: 2, R: 3, Q: 4, K: 5

void setBits();



// //////////////// //
//**** init.cpp ****//
// //////////////// //
extern const int bishopRelevantBits[64];
extern const int rookRelevantBits[64];

extern const U64 bishopMagics[64];
extern const U64 rookMagics[64];

extern BITBOARD pawnAttacks[2][64];
extern BITBOARD bitKnightMoves[64];
extern BITBOARD bitKingMoves[64];

extern BITBOARD bitBishopMoves[64];
extern BITBOARD bitRookMoves[64];
extern BITBOARD bitQueenMoves[64];

extern BITBOARD bitBishopMovesTable[64][512];
extern BITBOARD bitRookMovesTable[64][4096];

// maybe some of these should not be at the global level, can see later
// anything that is not at the global level should be static
BITBOARD maskPawnAttacks(int square, int side);
BITBOARD maskKnightMoves(int square);
BITBOARD maskKingMoves(int square);
BITBOARD maskBishopMoves(int square);
BITBOARD maskRookMoves(int square);

BITBOARD bishopAttacksOnTheFly(int square, U64 blocker);
BITBOARD rookAttacksOnTheFly(int square, U64 blocker);

BITBOARD setOccupancies(int index, int bitInMask, U64 attackMask);

void initLeaperPiecesAttacks();
void initSliderAttacks(int bishop);


#endif //GLOBALS_H
