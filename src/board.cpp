#include "globals.h"
#include "constants.h"
#include "inline_functions.h"

BITBOARD bitMoves[6][64]{}; // the indices are P: 0, N: 1, B: 2, R: 3, Q: 4, K: 5

BITBOARD maskCols[64]{};

BITBOARD notAFile{18374403900871474942ULL};
BITBOARD notABFile{18229723555195321596ULL};
BITBOARD notHFile{9187201950435737471ULL};
BITBOARD notHGFile{4557430888798830399ULL};


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

void setBits() {
    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 64; j++) {
            // checking that i and j are the same column
            if ((i % 8) == (j % 8)) {
                setBit(maskCols[i], j);
            }
        }
    }
}





