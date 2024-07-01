//
// In this file we declare the variables and functions that are used across multiple files
// these function and variables will have their own definition in their own .cpp file

// Here is the board representation, A1 (the right most bit) is at index 0, this results in the
// board being mirrored vertically if the bitboard is printed out directly
// This is a convention which might have possible mathematical advantages

// A8  B8  C8  D8  E8  F8  G8  H8
// A7  B7  C7  D7  E7  F7  G7  H7
// A6  B6  C6  D6  E6  F6  G6  H6
// A5  B5  C5  D5  E5  F5  G5  H5
// A4  B4  C4  D4  E4  F4  G4  H4
// A3  B3  C3  D3  E3  F3  G3  H3
// A2  B2  C2  D2  E2  F2  G2  H2
// A1  B1  C1  D1  E1  F1  G1  H1

// Bitboard Indices
// 56  57  58  59  60  61  62  63
// 48  49  50  51  52  53  54  55
// 40  41  42  43  44  45  46  47
// 32  33  34  35  36  37  38  39
// 24  25  26  27  28  29  30  31
// 16  17  18  19  20  21  22  23
// 8   9   10  11  12  13  14  15
// 0   1   2   3   4   5   6   7

#ifndef GLOBALS_H
#define GLOBALS_H

#include <cstdint>
#define U64 std::uint64_t
#define BITBOARD std::uint64_t

#define A1  0
#define B1  1
#define C1  2
#define D1  3
#define E1  4
#define F1  5
#define G1  6
#define H1  7

#define A2  8
#define B2  9
#define C2  10
#define D2  11
#define E2  12
#define F2  13
#define G2  14
#define H2  15

#define A3  16
#define B3  17
#define C3  18
#define D3  19
#define E3  20
#define F3  21
#define G3  22
#define H3  23

#define A4  24
#define B4  25
#define C4  26
#define D4  27
#define E4  28
#define F4  29
#define G4  30
#define H4  31

#define A5  32
#define B5  33
#define C5  34
#define D5  35
#define E5  36
#define F5  37
#define G5  38
#define H5  39

#define A6  40
#define B6  41
#define C6  42
#define D6  43
#define E6  44
#define F6  45
#define G6  46
#define H6  47

#define A7  48
#define B7  49
#define C7  50
#define D7  51
#define E7  52
#define F7  53
#define G7  54
#define H7  55

#define A8  56
#define B8  57
#define C8  58
#define D8  59
#define E8  60
#define F8  61
#define G8  62
#define H8  63



// Move directions as seen from white, north is up the board
#define NORTH 0
#define NE 1
#define EAST 2
#define SE 3
#define SOUTH 4
#define SW 5
#define WEST 6
#define NW 7

// Piece values
#define P 0
#define N 1
#define B 2
#define R 3
#define Q 4
#define K 5
#define EMPTY 6

#define White 0
#define Black 1


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
const extern int col[64];
const extern int row[64];
const extern int nwdiag[64];
const extern int nediag[64];

BITBOARD maskPawnAttacks(int square, int side); // Maybe i dont need this at the global level
BITBOARD maskKnightMoves(int square);  // these are mostly for debugging
BITBOARD maskKingMoves(int square);
BITBOARD maskBishopMoves(int square);
BITBOARD maskRookMoves(int square);

BITBOARD bishopAttacksOnTheFly(int square, U64 blocker);
BITBOARD rookAttacksOnTheFly(int square, U64 blocker);

void initLeaperPiecesArrays();
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
