/*
A1 B1 C1 D1 E1 F1 G1 H1
A2 B2 C2 D2 E2 F2 G2 H2
A3 B3 C3 D3 E3 F3 G3 H3
A4 B4 C4 D4 E4 F4 G4 H4
A5 B5 C5 D5 E5 F5 G5 H5
A6 B6 C6 D6 E6 F6 G6 H6
A7 B7 C7 D7 E7 F7 G7 H7
A8 B8 C8 D8 E8 F8 G8 H8

note that white is at the top here
*/
#include "globals.h"
#include "constants.h"

BITBOARD mask[64]{};
BITBOARD notMask[64]{};

BITBOARD bitMoves[6][64]{}; // the indices are P: 0, N: 1, B: 2, R: 3, Q: 4, K: 5

BITBOARD bitKnightMoves[64]{};
BITBOARD bitBishopMoves[64]{};

BITBOARD bitBishopMovesTable[64][512];
BITBOARD bitRookMovesTable[64][4096];

BITBOARD bitRookMoves[64]{};
BITBOARD bitQueenMoves[64]{};
BITBOARD bitKingMoves[64]{};

BITBOARD maskCols[64]{};
BITBOARD notAFile{18374403900871474942ULL};
BITBOARD notABFile{18229723555195321596ULL};
BITBOARD notHFile{9187201950435737471ULL};
BITBOARD notHGFile{4557430888798830399ULL};


// Functions for setting specific squares on and off
void setBit(BITBOARD& bb, const int square) {
    bb |= (1ULL << square);
}

void setBitFalse(BITBOARD& bb, const int square) {
    bb &= ~mask[square];
}


void setBits() {
    for (int i=0; i < 64; i++) {
        setBit(mask[i], i);
        notMask[i] = ~mask[i];
    }

    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 64; j++) {
            // checking that i and j are the same column
            if ((i % 8) == (j % 8)) {
                setBit(maskCols[i], j);
            }
        }
    }
}





