//
// Created by Federico Saitta on 29/06/2024.
//

#include "globals.h"

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

BITBOARD pawnAttacks[2][64];


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



// Write tests for these two functions


// This generates relevant bishop occupancy squares
// in the mask we do not consider the edge landing squares
// not storing this anywhere at the moment
BITBOARD maskBishopMoves(const int square) {
    U64 attacks{};

    // initial rank and files
    int r{}, f{};

    const int tr{ square / 8 }; //target row
    const int tf{ square % 8 }; //target file

    for (r = tr + 1, f = tf + 1; r < 7 && f < 7; r++, f++) { attacks |= (1ULL << (r * 8 + f)); }

    for (r = tr - 1, f = tf + 1; r > 0 && f < 7; r--, f++) { attacks |= (1ULL << (r * 8 + f)); }
    for (r = tr + 1, f = tf - 1; r < 7 && f > 0; r++, f--) { attacks |= (1ULL << (r * 8 + f)); }

    for (r = tr - 1, f = tf - 1; r > 0 && f > 0; r--, f--) { attacks |= (1ULL << (r * 8 + f)); }

    return attacks;
}

// need to implement this
BITBOARD maskRookMoves(const int square) {
    U64 attacks{};

    // initial rank and files
    int r{}, f{};

    const int tr{ square / 8 }; //target row
    const int tf{ square % 8 }; //target file

    for (r = tr + 1; r < 7; r++) { attacks |= (1ULL << (r * 8 + tf)); }
    for (r = tr - 1; r > 0; r--) { attacks |= (1ULL << (r * 8 + tf)); }

    for (f = tf + 1; f < 7; f++) { attacks |= (1ULL << (tr * 8 + f)); }
    for (f = tf - 1; f > 0; f--) { attacks |= (1ULL << (tr * 8 + f)); }

    return attacks;
}


void initLeaperPiecesAttacks() {

    for (int i = 0; i < 64; i ++) {
        pawnAttacks[0][i] =  maskPawnAttacks(i, 0);
        pawnAttacks[1][i] =  maskPawnAttacks(i, 1);

        bitKnightMoves[i] =  maskKnightMoves(i);
        bitKingMoves[i] =  maskKingMoves(i);
    }
}


// In this function we want to hit the board edges unlike before
// at the moements lets assume we can eat the blockers
// movegenerators will deal with this difference
BITBOARD bishopAttacksOnTheFly(const int square, const U64 blocker) {
    U64 attacks{};

    // initial rank and files
    int r{}, f{};

    const int tr{ square / 8 }; //target row
    const int tf{ square % 8 }; //target file

    for (r = tr + 1, f = tf + 1; r <= 7 && f <= 7; r++, f++) {
        attacks |= (1ULL << (r * 8 + f));
        if ( (1ULL << (r * 8 + f)) & blocker) break;
    }

    for (r = tr - 1, f = tf + 1; r >= 0 && f <= 7; r--, f++) {
        attacks |= (1ULL << (r * 8 + f));
        if ( (1ULL << (r * 8 + f)) & blocker) break;
    }
    for (r = tr + 1, f = tf - 1; r <= 7 && f >= 0; r++, f--) {
        attacks |= (1ULL << (r * 8 + f));
        if ( (1ULL << (r * 8 + f)) & blocker) break;
    }

    for (r = tr - 1, f = tf - 1; r >= 0 && f >= 0; r--, f--) {
        attacks |= (1ULL << (r * 8 + f));
        if ( (1ULL << (r * 8 + f)) & blocker) break;
    }

    return attacks;
}

BITBOARD rookAttacksOnTheFly(const int square, const U64 blocker) {
    U64 attacks{};

    // initial rank and files
    int r{}, f{};

    const int tr{ square / 8 }; //target row
    const int tf{ square % 8 }; //target file

    for (r = tr + 1; r <= 7; r++) {
        attacks |= (1ULL << (r * 8 + tf));
        if ( (1ULL << (r * 8 + tf)) & blocker) break;
    }
    for (r = tr - 1; r >= 0; r--) {
        attacks |= (1ULL << (r * 8 + tf));
        if ( (1ULL << (r * 8 + tf)) & blocker) break;
    }

    for (f = tf + 1; f <= 7; f++) {
        attacks |= (1ULL << (tr * 8 + f));
        if ( (1ULL << (tr * 8 + f)) & blocker) break;
    }
    for (f = tf - 1; f >= 0; f--) {
        attacks |= (1ULL << (tr * 8 + f));
        if ( (1ULL << (tr * 8 + f)) & blocker) break;
    }

    return attacks;
}


BITBOARD setOccupancies(const int index, const int bitInMask, U64 attackMask) {
    U64 occupancy{};

    for (int i=0; i < bitInMask; i++) {

        const int square {getLeastSigBitIndex(attackMask)};
        setBitFalse(attackMask, square);

        if (index & (1ULL << i)) {
            occupancy |= (1ULL << square);
        }

    }
    return occupancy;
}


// from this https://www.reddit.com/r/chessprogramming/comments/wsrf3s/is_there_any_place_i_can_copy_fancy_magic_numbers/

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


void initSliderAttacks(const int bishop) {

    for (int square=0; square < 64; square++) {
        bitBishopMoves[square] = maskBishopMoves(square);
        bitRookMoves[square] = maskRookMoves(square);

        const U64 attackMask { bishop ? bitBishopMoves[square] : bitRookMoves[square]};

        const int relevantBitsCount{ countBits(attackMask)};
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
