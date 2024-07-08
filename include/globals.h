//
// In this file we declare the variables and functions that are used across multiple files
// these function and variables will have their own definition in their own .cpp file

#ifndef GLOBALS_H
#define GLOBALS_H

#include "macros.h"
#include <iostream>
#include <fstream>

// //////////////// //
//**** init.cpp ****//
// //////////////// //
extern const int bishopRelevantBits[64];
extern const int rookRelevantBits[64];

extern const U64 bishopMagics[64];
extern const U64 rookMagics[64];

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

void initAll();
// //////////////// //
//**** board.cpp ****//
// //////////////// //
extern const char* chessBoard[65];

void parseFEN(const std::string& fenString);

extern BITBOARD bitboards[12]; // Pawn, Knight, Bishop, Rook, Queen, King, p, n, b, r, q, k
extern BITBOARD occupancies[3]; // white, black, both sides combined

extern int side; // side to move
extern int enPassantSQ; // index from 0 to 64, 64 represents no en-passant is on the board
extern int castle; // 4-bit integer of the form qkQK, if bit is turned on then castling is available


// /////////////////// //
//**** movegen.cpp ****//
struct MoveList {
    int moves[218];
    int count;
};

void generateMoves(MoveList& moveList);

// /////////////////// //
//**** update.cpp ****//
// /////////////////// //
int makeMove(int move, int onlyCaptures);

// ///////////////// //
// **** misc.cpp ****//
// ///////////////// //
extern const char promotedPieces[];
extern const char* unicodePieces[];


// ////////////////////// //
// **** hashtable.cpp ****//
// ////////////////////// //

extern U64 randomPieceKeys[12][64];
extern U64 randomEnPassantKeys[64];
extern U64 randomCastlingKeys[16];
extern U64 sideKey;

extern U64 hashKey; // of the position

void initRandomKeys();
U64 generateHashKey();


// ///////////////// //
// **** search.cpp ****//
// ///////////////// //
int scoreMove(int move, int ply);
void sortMoves(MoveList& moveList, int ply);

void iterativeDeepening(int depth, bool timeConstraint=false);

// ///////////////// //
// **** uci.cpp ****//
// ///////////////// //
void UCI();
extern std::ofstream logFile;

// both are in milliseconds
extern int gameLengthTime;
extern int whiteClockTime;
extern int blackClockTime;
extern int whiteIncrementTime;
extern int blackIncrementTime;



#endif //GLOBALS_H
