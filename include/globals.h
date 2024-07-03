//
// In this file we declare the variables and functions that are used across multiple files
// these function and variables will have their own definition in their own .cpp file


#ifndef GLOBALS_H
#define GLOBALS_H

#include "constants.h"
#include <iostream>

// //////////////// //
//**** init.cpp ****//
// //////////////// //
extern const int bishopRelevantBits[64];
extern const int rookRelevantBits[64];

extern const U64 bishopMagics[64];
extern const U64 rookMagics[64];

extern BITBOARD maskCols[64];

extern BITBOARD notAFile;
extern BITBOARD notABFile;
extern BITBOARD notHFile;
extern BITBOARD notHGFile;

extern BITBOARD pawnAttacks[2][64];
extern BITBOARD bitKnightAttacks[64];
extern BITBOARD bitKingAttacks[64];

extern BITBOARD bitBishopAttacks[64];
extern BITBOARD bitRookAttacks[64];
extern BITBOARD bitQueenAttacks[64];

extern BITBOARD bitBishopAttacksTable[64][512];
extern BITBOARD bitRookAttacksTable[64][4096];

void initBits();
void initAll();


// //////////////// //
//**** board.cpp ****//
// //////////////// //
extern const char* chessBoard[65];

extern BITBOARD bitboards[12]; // Pawn, Knight, Bishop, Rook, Queen, King, p, n, b, r, q, k
extern BITBOARD occupancies[3]; // white, black, both sides combined

extern int side; // side to move
extern int enPassantSQ; // index from 0 to 64, 64 represents no en-passant is on the board
extern int castle; // 4-bit integer of the form qkQK, if bit is turned on then castling is available

void parseFEN(const std::string& fenString);


// /////////////////// //
//**** movegen.cpp ****//
// /////////////////// //
void generateMoves();

struct MoveList {
    int moves[256];
    int count;
};

extern MoveList moveList;

// ///////////////// //
// **** misc.cpp ****//
// ///////////////// //
extern const char asciiPieces[];
extern const int charPieces[];
extern const char promotedPieces[];
extern const char* unicodePieces[];


#endif //GLOBALS_H
