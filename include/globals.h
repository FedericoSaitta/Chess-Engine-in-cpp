//
// In this file we declare the variables and functions that are used across multiple files
// these function and variables will have their own definition in their own .cpp file

// Here is the board representation, A1 (the right most bit) is at index 0, this results in the
// board being mirrored vertically if the bitboard is printed out directly
// This is a convention which might have possible mathematical advantages
#ifndef GLOBALS_H
#define GLOBALS_H

#include "constants.h"

#include <iostream>

// //////////////// //
//**** init.cpp ****//
// //////////////// //
extern BITBOARD maskCols[64];

extern BITBOARD notAFile;
extern BITBOARD notABFile;
extern BITBOARD notHFile;
extern BITBOARD notHGFile;

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
void initBits();
void initAll();



// //////////////// //
//**** board.cpp ****//
// //////////////// //
const extern int piece_value[6];

void initBits();

extern BITBOARD bitboards[12]; // indices from 0 to 11 define by macros
extern BITBOARD occupancies[3]; // white, black pieces, both sides, maybe use enums for this

extern int side; // side to move
extern int enPassantSQ;
extern int castle;

void parseFEN(const std::string& fenString);

extern const char* chessBoard[65];

#endif //GLOBALS_H
