//
// Created by Federico Saitta on 29/06/2024.
//
#include "macros.h"
#include "globals.h"
#include "inline_functions.h"


// *** NON-MAGIC PRE-COMPUTED TABLES *** //
static BITBOARD maskPawnAttacks(const int square, const int side) {

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

static BITBOARD maskKnightMoves(const int square) {

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

static BITBOARD maskKingMoves(const int square) {
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

// *** PLAIN-MAGIC PRE-COMPUTED TABLES *** //
// https://chess.stackexchange.com/questions/40051/border-pieces-in-magic-bitboards#:~:text=The%20reason%20the%20borders%20are,rook%20can%20attack%20other%20squares.
static BITBOARD maskBishopMoves(const int square) {
    U64 attacks{};

    const int targetRow{ square / 8 }; // target row
    const int targetFile{ square % 8 }; // target file

    for (int row = targetRow + 1, file = targetFile + 1; row < 7 && file < 7; row++, file++) { setBit(attacks, row * 8 + file); }

    for (int row = targetRow - 1, file = targetFile + 1; row > 0 && file < 7; row--, file++) { setBit(attacks, row * 8 + file); }
    for (int row = targetRow + 1, file = targetFile - 1; row < 7 && file > 0; row++, file--) { setBit(attacks, row * 8 + file); }

    for (int row = targetRow - 1, file = targetFile - 1; row > 0 && file > 0; row--, file--) { setBit(attacks, row * 8 + file); }

    return attacks;
}

static BITBOARD maskRookMoves(const int square) {
    U64 attacks{};

    const int targetRow{ square / 8 }; // target row
    const int targetFile{ square % 8 }; // target file

    for (int row = targetRow + 1; row < 7; row++) { setBit(attacks, row * 8 + targetFile); }
    for (int row = targetRow - 1; row > 0; row--) { setBit(attacks, row * 8 + targetFile); }

    for (int file = targetFile + 1; file < 7; file++) { setBit(attacks, targetRow * 8 + file); }
    for (int file = targetFile - 1; file > 0; file--) { setBit(attacks, targetRow * 8 + file); }

    return attacks;
}

static BITBOARD bishopAttacksOnTheFly(const int square, const U64 blocker) {
    U64 attacks{};

    const int targetRow{ square / 8 }; // target row
    const int targetFile{ square % 8 }; // target file

    for (int row = targetRow + 1, file = targetFile + 1; row <= 7 && file <= 7; row++, file++) {
        setBit(attacks, row * 8 + file);
        if ( (1ULL << (row * 8 + file)) & blocker) break;
    }

    for (int row = targetRow - 1, file = targetFile + 1; row >= 0 && file <= 7; row--, file++) {
        setBit(attacks, row * 8 + file);
        if ( (1ULL << (row * 8 + file)) & blocker) break;
    }
    for (int row = targetRow + 1, file = targetFile - 1; row <= 7 && file >= 0; row++, file--) {
        setBit(attacks, row * 8 + file);
        if ( (1ULL << (row * 8 + file)) & blocker) break;
    }

    for (int row = targetRow - 1, file = targetFile - 1; row >= 0 && file >= 0; row--, file--) {
        setBit(attacks, row * 8 + file);
        if ( (1ULL << (row * 8 + file)) & blocker) break;
    }

    return attacks;
}

static BITBOARD rookAttacksOnTheFly(const int square, const U64 blocker) {
    // generates an attack Bitboard for each square on the board, here we loop till the end of the baord
    U64 attacks{};

    const int targetRow{ square / 8 }; // target row
    const int targetFile{ square % 8 }; // target file

    for (int row = targetRow + 1; row <= 7; row++) {
        setBit(attacks, row * 8 + targetFile);
        if ( (1ULL << (row * 8 + targetFile)) & blocker) break;
    }
    for (int row = targetRow - 1; row >= 0; row--) {
        setBit(attacks, row * 8 + targetFile);
        if ( (1ULL << (row * 8 + targetFile)) & blocker) break;
    }

    for (int file = targetFile + 1; file <= 7; file++) {
        setBit(attacks, targetRow * 8 + file);
        if ( (1ULL << (targetRow * 8 + file)) & blocker) break;
    }
    for (int file = targetFile - 1; file >= 0; file--) {
        setBit(attacks, targetRow * 8 + file);
        if ( (1ULL << (targetRow * 8 + file)) & blocker) break;
    }

    return attacks;
}

static BITBOARD setOccupancies(const int index, const int bitInMask, U64 attackMask) {
    U64 occupancy{};

    for (int i=0; i < bitInMask; i++) {

        const int square {getLeastSigBitIndex(attackMask)};
        setBitFalse(attackMask, square);

        if (index & (1ULL << i)) { setBit(occupancy, square); }
    }
    return occupancy;
}

// *** INITIALIZING BITBOARD ARRAYS *** //
static void initLeaperPiecesAttacks() {
    for (int square=0; square < 64; square ++) {
        pawnAttacks[White][square] =  maskPawnAttacks(square, 0); // white pawn captures (no en-passant)
        pawnAttacks[Black][square] =  maskPawnAttacks(square, 1); // black pawn captures (no en-passant)

        bitKnightAttacks[square] =  maskKnightMoves(square);
        bitKingAttacks[square] =  maskKingMoves(square);
    }
}

static void initSliderAttacks(const int bishop) {

    for (int square=0; square < 64; square++) {
        bitBishopAttacks[square] = maskBishopMoves(square);
        bitRookAttacks[square] = maskRookMoves(square);

        const U64 attackMask { bishop ? bitBishopAttacks[square] : bitRookAttacks[square]};

        const int relevantBitsCount{ countBits(attackMask) };
        const int occupacyIndex { (1 << relevantBitsCount) };

        for (int index=0; index < occupacyIndex; index++) {
            if (bishop) {
                const U64 occupancy { setOccupancies(index, relevantBitsCount, attackMask) };

                // for correctness this should be static casted
                const int magicIndex = (occupancy * bishopMagics[square]) >> (64 - bishopRelevantBits[square]);
                bitBishopAttacksTable[square][magicIndex] = bishopAttacksOnTheFly(square, occupancy);

            } else {
                const U64 occupancy { setOccupancies(index, relevantBitsCount, attackMask) };

                // for correctness this should be static casted
                const int magicIndex = (occupancy * rookMagics[square]) >> (64 - rookRelevantBits[square]);
                bitRookAttacksTable[square][magicIndex] = rookAttacksOnTheFly(square, occupancy);
            }
        }
    }
}

void initAll() {
    initLeaperPiecesAttacks();

    initSliderAttacks(1);
    initSliderAttacks(0);

    initRandomKeys();

    clearTranspositionTable();
} // this only takes 100 ms at startup in DEBUG mode
