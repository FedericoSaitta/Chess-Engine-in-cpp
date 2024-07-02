//
// Created by Federico Saitta on 29/06/2024.
//
#include "constants.h"
#include "globals.h"
#include "inline_functions.h"


const int bishopRelevantBits[64]{
    6, 5, 5, 5, 5, 5, 5, 6,
    5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5,
    6, 5, 5, 5, 5, 5, 5, 6
};

const int rookRelevantBits[64] {
    12, 11, 11, 11, 11, 11, 11, 12,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    12, 11, 11, 11, 11, 11, 11, 12
};

// from this https://www.reddit.com/row/chessprogramming/comments/wsrf3s/is_there_any_place_i_can_copy_fancy_magic_numbers/
const U64 bishopMagics[64] {
    0xC00204004A0449ULL, 0x3020A20A02202000ULL, 0x4282881002004ULL, 0x8244250200140020ULL,
    0x2442021008840010ULL, 0x822020080004ULL, 0x2010801042040D0ULL, 0x2020110311104006ULL,
    0x1020041022880104ULL, 0x12204104008C8EULL, 0x2002044C04005100ULL, 0x2084180600445C80ULL,
    0x10C020210100000ULL, 0x82219200400ULL, 0x800220210240482ULL, 0x8140104508019010ULL,
    0x401910840800C8ULL, 0x2000A4C1022A10ULL, 0x42001008901100ULL, 0x100800802044000ULL,
    0x40A001401210800ULL, 0x41A0200210100804ULL, 0x40040108084CULL, 0x100810202012102ULL,
    0x11084004200D4D02ULL, 0x4410302002A43516ULL, 0x880010122120ULL, 0x801040008020820ULL,
    0x4108808050082000ULL, 0x148060000220901ULL, 0x54040020820140ULL, 0x4610000828800ULL,
    0x85804000A2021ULL, 0x108021000080120ULL, 0xC000248840100401ULL, 0x20080880080ULL,
    0x1000404040440100ULL, 0x2010101000E0248ULL, 0x120A821400804412ULL, 0x2001004082010400ULL,
    0x52080240000818ULL, 0x68420101C8002000ULL, 0x42020044000848ULL, 0x4002053008803ULL,
    0x2640812001012ULL, 0x2040108808800041ULL, 0xB8022082100400ULL, 0x2402809200890600ULL,
    0x40C0C84440200ULL, 0x4100212108200114ULL, 0x4202284108008ULL, 0x422042022000ULL,
    0x910001002022100ULL, 0x9074203232221000ULL, 0x40334802018000ULL, 0x4502042124030040ULL,
    0x24110801042000ULL, 0x1E4628A4042000ULL, 0xD010142104014420ULL, 0x1080002411090ULL,
    0x240003206020410DULL, 0x3C008200202020CULL, 0x51100310010204ULL, 0x220200882008620ULL
};

const U64 rookMagics[64] {
    0x2880002040001080ULL, 0xC0012000401002ULL, 0x2C80100080600148ULL, 0x480048008001000ULL,
    0x6000C10202A0008ULL, 0x200041001020008ULL, 0x700009402002100ULL, 0x18000446A800500ULL,
    0x25800240008430ULL, 0x10440002000500AULL, 0x8208801000200380ULL, 0x4800800800801001ULL,
    0x405000800041101ULL, 0x400A000402000850ULL, 0x140400083A1001A4ULL, 0x40A00045100840AULL,
    0x440108000804022ULL, 0x1040028020004480ULL, 0x1101010040102000ULL, 0x80101001000200CULL,
    0x8008008040080ULL, 0x2008002800400ULL, 0x1000010100040200ULL, 0x30020000840041ULL,
    0x1400980008060ULL, 0x40004000C0201001ULL, 0x9040100080802000ULL, 0x8140100080080080ULL,
    0x488A002200041008ULL, 0x2040080800200ULL, 0x1028400083041ULL, 0x811020000A044ULL,
    0x80002010400040ULL, 0x1090002008404002ULL, 0x802004801000ULL, 0x100201001000D00ULL,
    0x10080082800400ULL, 0x9C02001002000804ULL, 0x8100804008201ULL, 0x100880DE000401ULL,
    0x188008600251C000ULL, 0x2500020014000ULL, 0x60014021030012ULL, 0x8010200101000ULL,
    0x41000800110006ULL, 0x24000402008080ULL, 0x804300102240088ULL, 0x84002100408A0024ULL,
    0x60C0002040800080ULL, 0xD0004000200040ULL, 0x806000900280ULL, 0x30010220100900ULL,
    0x20800400080080ULL, 0x5004802440100ULL, 0x4022800200010080ULL, 0xA286008044210200ULL,
    0x4000204100958001ULL, 0x3000950040832202ULL, 0x200008401501ULL, 0x82022100C810000DULL,
    0x401001002080005ULL, 0x1A01001A04002829ULL, 0x5000210842100084ULL, 0x310810C240142ULL
};

BITBOARD maskCols[64]{};

BITBOARD notAFile{18374403900871474942ULL};
BITBOARD notABFile{18229723555195321596ULL};
BITBOARD notHFile{9187201950435737471ULL};
BITBOARD notHGFile{4557430888798830399ULL};

BITBOARD pawnAttacks[2][64];
BITBOARD bitKnightMoves[64]{};
BITBOARD bitKingMoves[64]{};

BITBOARD bitBishopMoves[64]{};
BITBOARD bitRookMoves[64]{};
BITBOARD bitQueenMoves[64]{};

BITBOARD bitBishopMovesTable[64][512];
BITBOARD bitRookMovesTable[64][4096];

// *** NON-MAGIC PRE-COMPUTED TABLES *** //
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

// *** PLAIN-MAGIC PRE-COMPUTED TABLES *** //
// https://chess.stackexchange.com/questions/40051/border-pieces-in-magic-bitboards#:~:text=The%20reason%20the%20borders%20are,rook%20can%20attack%20other%20squares.
BITBOARD maskBishopMoves(const int square) {
    U64 attacks{};

    const int targetRow{ square / 8 }; // target row
    const int targetFile{ square % 8 }; // target file

    for (int row = targetRow + 1, file = targetFile + 1; row < 7 && file < 7; row++, file++) { setBit(attacks, row * 8 + file); }

    for (int row = targetRow - 1, file = targetFile + 1; row > 0 && file < 7; row--, file++) { setBit(attacks, row * 8 + file); }
    for (int row = targetRow + 1, file = targetFile - 1; row < 7 && file > 0; row++, file--) { setBit(attacks, row * 8 + file); }

    for (int row = targetRow - 1, file = targetFile - 1; row > 0 && file > 0; row--, file--) { setBit(attacks, row * 8 + file); }

    return attacks;
}

BITBOARD maskRookMoves(const int square) {
    U64 attacks{};

    const int targetRow{ square / 8 }; // target row
    const int targetFile{ square % 8 }; // target file

    for (int row = targetRow + 1; row < 7; row++) { setBit(attacks, row * 8 + targetFile); }
    for (int row = targetRow - 1; row > 0; row--) { setBit(attacks, row * 8 + targetFile); }

    for (int file = targetFile + 1; file < 7; file++) { setBit(attacks, targetRow * 8 + file); }
    for (int file = targetFile - 1; file > 0; file--) { setBit(attacks, targetRow * 8 + file); }

    return attacks;
}

BITBOARD bishopAttacksOnTheFly(const int square, const U64 blocker) {
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

BITBOARD rookAttacksOnTheFly(const int square, const U64 blocker) {
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

BITBOARD setOccupancies(const int index, const int bitInMask, U64 attackMask) {
    U64 occupancy{};

    for (int i=0; i < bitInMask; i++) {

        const int square {getLeastSigBitIndex(attackMask)};
        setBitFalse(attackMask, square);

        if (index & (1ULL << i)) { setBit(occupancy, square); }
    }
    return occupancy;
}

// *** INITIALIZING BITBOARD ARRAYS *** //
void initLeaperPiecesAttacks() {
    for (int square=0; square < 64; square ++) {
        pawnAttacks[0][square] =  maskPawnAttacks(square, 0); // white pawn captures (no en-passant)
        pawnAttacks[1][square] =  maskPawnAttacks(square, 1); // black pawn captures (no en-passant)

        bitKnightMoves[square] =  maskKnightMoves(square);
        bitKingMoves[square] =  maskKingMoves(square);
    }
}

void initSliderAttacks(const int bishop) {

    for (int square=0; square < 64; square++) {
        bitBishopMoves[square] = maskBishopMoves(square);
        bitRookMoves[square] = maskRookMoves(square);

        const U64 attackMask { bishop ? bitBishopMoves[square] : bitRookMoves[square]};

        const int relevantBitsCount{ countBits(attackMask) };
        const int occupacyIndex { (1 << relevantBitsCount) };

        for (int index=0; index < occupacyIndex; index++) {
            if (bishop) {
                const U64 occupancy { setOccupancies(index, relevantBitsCount, attackMask) };

                // for correctness this should be static casted
                const int magicIndex = (occupancy * bishopMagics[square]) >> (64 - bishopRelevantBits[square]);
                bitBishopMovesTable[square][magicIndex] = bishopAttacksOnTheFly(square, occupancy);

            } else {
                const U64 occupancy { setOccupancies(index, relevantBitsCount, attackMask) };

                // for correctness this should be static casted
                const int magicIndex = (occupancy * rookMagics[square]) >> (64 - rookRelevantBits[square]);
                bitRookMovesTable[square][magicIndex] = rookAttacksOnTheFly(square, occupancy);
            }
        }
    }
}

void initBits() {
    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 64; j++) {
            // checking that i and j are the same column
            if ((i % 8) == (j % 8)) {
                setBit(maskCols[i], j);
            }
        }
    }
} // maybe you can delete this later

void initAll() {
    initBits();
    initLeaperPiecesAttacks();

    initSliderAttacks(1);
    initSliderAttacks(0);
} // this only takes 100 ms at startup in DEBUG mode
