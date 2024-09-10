#include "macros.h"
#include "types.h"
#include "move.h"
#include <random>

#include "inline_functions.h"
#include "hashtable.h"
#include "eval/evaluation.h"
#include "search/search.h"


const Move Move::Null = Move();

// ARRAYS NEEDED FOR MAGIC U64 APPROACH //
constexpr int bishopRelevantBits[64]{
    6, 5, 5, 5, 5, 5, 5, 6,
    5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 9, 9, 7, 5, 5,
    5, 5, 7, 7, 7, 7, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5,
    6, 5, 5, 5, 5, 5, 5, 6
};


constexpr int getRankFromSquare[64]{
    0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 1,
    2, 2, 2, 2, 2, 2, 2, 2,
    3, 3, 3, 3, 3, 3, 3, 3,
    4, 4, 4, 4, 4, 4, 4, 4,
    5, 5, 5, 5, 5, 5, 5, 5,
    6, 6, 6, 6, 6, 6, 6, 6,
    7, 7, 7, 7, 7, 7, 7, 7
};

constexpr int rookRelevantBits[64] {
    12, 11, 11, 11, 11, 11, 11, 12,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11,
    12, 11, 11, 11, 11, 11, 11, 12
};

constexpr int getFileFromSquare[64]{
    0, 1, 2, 3, 4, 5, 6, 7, // Rank 0
    0, 1, 2, 3, 4, 5, 6, 7, // Rank 1
    0, 1, 2, 3, 4, 5, 6, 7, // Rank 2
    0, 1, 2, 3, 4, 5, 6, 7, // Rank 3
    0, 1, 2, 3, 4, 5, 6, 7, // Rank 4
    0, 1, 2, 3, 4, 5, 6, 7, // Rank 5
    0, 1, 2, 3, 4, 5, 6, 7, // Rank 6
    0, 1, 2, 3, 4, 5, 6, 7  // Rank 7
};

// from this https://www.reddit.com/row/chessprogramming/comments/wsrf3s/is_there_any_place_i_can_copy_fancy_magic_numbers/
constexpr U64 bishopMagics[64] {
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

constexpr U64 rookMagics[64] {
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

U64 notAFile{18374403900871474942ULL};
U64 notABFile{18229723555195321596ULL};
U64 notHFile{9187201950435737471ULL};
U64 notHGFile{4557430888798830399ULL};

U64 bitPawnAttacks[2][64];
U64 bitKnightAttacks[64];
U64 bitKingAttacks[64];

U64 bitBishopAttacks[64];
U64 bitRookAttacks[64];
U64 bitQueenAttacks[64];

U64 bitBishopAttacksTable[64][512];
U64 bitRookAttacksTable[64][4096];

U64 fileMasks[64]{};
U64 rankMasks[64]{};
U64 isolatedPawnMasks[64]{};
U64 white_passedPawnMasks[64]{};
U64 black_passedPawnMasks[64]{};


// *** NON-MAGIC PRE-COMPUTED TABLES *** //
static U64 maskPawnAttacks(const int square, const int side) {

    U64 attacks{};
    U64 b{};

    SET_BIT(b, square);

    if (!side) { // for white
        if (notHFile & (b << 7)){ attacks |= (b << 7); }
        if (notAFile & (b << 9)){ attacks |= (b << 9); }
    }
    else { // for black
        if (notAFile & (b >> 7)){ attacks |= (b >> 7); }
        if (notHFile & (b >> 9)){ attacks |= (b >> 9); }
    }

    return attacks;
}

static U64 maskKnightMoves(const int square) {

    U64 attacks{};
    U64 b{};

    SET_BIT(b, square);

    if ((b << 17) & notAFile) attacks |= b << 17;
    if ((b << 15) & notHFile) attacks |= b << 15;
    if ((b << 10) & notABFile) attacks |= b << 10;
    if ((b << 6) & notHGFile) attacks |= b << 6;

    if ((b >> 17) & notHFile) attacks |= b >> 17;
    if ((b >> 15) & notAFile) attacks |= b >> 15;
    if ((b >> 10) & notHGFile) attacks |= b >> 10;
    if ((b >> 6) & notABFile) attacks |= b >> 6;

    return attacks;
}

static U64 maskKingMoves(const int square) {
    U64 attacks{};
    U64 b{};

    SET_BIT(b, square);

    if (b << 8) attacks |= (b << 8);
    if (b >> 8) attacks |= (b >> 8);
    if ((b << 1) & notAFile) {
        attacks |= (b << 1) | (b << 9) | (b >> 7);
    }
    if ((b >> 1) & notHFile) {
        attacks |= (b >> 1) | (b >> 9) | (b << 7);
    }

    return attacks;
}

// *** PLAIN-MAGIC PRE-COMPUTED TABLES *** //
// https://chess.stackexchange.com/questions/40051/border-pieces-in-magic-bitboards#:~:text=The%20reason%20the%20borders%20are,rook%20can%20attack%20other%20squares.
static U64 maskBishopMoves(const int square) {
    U64 attacks{};

    const int targetRow{ square / 8 }; // target row
    const int targetFile{ square % 8 }; // target file

    for (int row = targetRow + 1, file = targetFile + 1; row < 7 && file < 7; row++, file++) { SET_BIT(attacks, row * 8 + file); }

    for (int row = targetRow - 1, file = targetFile + 1; row > 0 && file < 7; row--, file++) { SET_BIT(attacks, row * 8 + file); }
    for (int row = targetRow + 1, file = targetFile - 1; row < 7 && file > 0; row++, file--) { SET_BIT(attacks, row * 8 + file); }

    for (int row = targetRow - 1, file = targetFile - 1; row > 0 && file > 0; row--, file--) { SET_BIT(attacks, row * 8 + file); }

    return attacks;
}

static U64 maskRookMoves(const int square) {
    U64 attacks{};

    const int targetRow{ square / 8 }; // target row
    const int targetFile{ square % 8 }; // target file

    for (int row = targetRow + 1; row < 7; row++) { SET_BIT(attacks, row * 8 + targetFile); }
    for (int row = targetRow - 1; row > 0; row--) { SET_BIT(attacks, row * 8 + targetFile); }

    for (int file = targetFile + 1; file < 7; file++) { SET_BIT(attacks, targetRow * 8 + file); }
    for (int file = targetFile - 1; file > 0; file--) { SET_BIT(attacks, targetRow * 8 + file); }

    return attacks;
}

static U64 bishopAttacksOnTheFly(const int square, const U64 blocker) {
    U64 attacks{};

    const int targetRow{ square / 8 }; // target row
    const int targetFile{ square % 8 }; // target file

    for (int row = targetRow + 1, file = targetFile + 1; row <= 7 && file <= 7; row++, file++) {
        SET_BIT(attacks, row * 8 + file);
        if ( (1ULL << (row * 8 + file)) & blocker) break;
    }

    for (int row = targetRow - 1, file = targetFile + 1; row >= 0 && file <= 7; row--, file++) {
        SET_BIT(attacks, row * 8 + file);
        if ( (1ULL << (row * 8 + file)) & blocker) break;
    }
    for (int row = targetRow + 1, file = targetFile - 1; row <= 7 && file >= 0; row++, file--) {
        SET_BIT(attacks, row * 8 + file);
        if ( (1ULL << (row * 8 + file)) & blocker) break;
    }

    for (int row = targetRow - 1, file = targetFile - 1; row >= 0 && file >= 0; row--, file--) {
        SET_BIT(attacks, row * 8 + file);
        if ( (1ULL << (row * 8 + file)) & blocker) break;
    }

    return attacks;
}

static U64 rookAttacksOnTheFly(const int square, const U64 blocker) {
    // generates an attack Bitboard for each square on the board, here we loop till the end of the baord
    U64 attacks{};

    const int targetRow{ square / 8 }; // target row
    const int targetFile{ square % 8 }; // target file

    for (int row = targetRow + 1; row <= 7; row++) {
        SET_BIT(attacks, row * 8 + targetFile);
        if ( (1ULL << (row * 8 + targetFile)) & blocker) break;
    }
    for (int row = targetRow - 1; row >= 0; row--) {
        SET_BIT(attacks, row * 8 + targetFile);
        if ( (1ULL << (row * 8 + targetFile)) & blocker) break;
    }

    for (int file = targetFile + 1; file <= 7; file++) {
        SET_BIT(attacks, targetRow * 8 + file);
        if ( (1ULL << (targetRow * 8 + file)) & blocker) break;
    }
    for (int file = targetFile - 1; file >= 0; file--) {
        SET_BIT(attacks, targetRow * 8 + file);
        if ( (1ULL << (targetRow * 8 + file)) & blocker) break;
    }

    return attacks;
}

static U64 setOccupancies(const int index, const int bitInMask, U64 attackMask) {
    U64 occupancy{};

    for (int i=0; i < bitInMask; i++) {

        const int square {pop_lsb(&attackMask)};

        if (index & (1ULL << i)) { SET_BIT(occupancy, square); }
    }
    return occupancy;
}

// *** INITIALIZING U64 ARRAYS *** //
static void initLeaperPiecesAttacks() {
    for (int square=0; square < 64; square ++) {
        bitPawnAttacks[WHITE][square] =  maskPawnAttacks(square, 0); // white pawn captures (no en-passant)
        bitPawnAttacks[BLACK][square] =  maskPawnAttacks(square, 1); // black pawn captures (no en-passant)

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
                const U64 magicIndex = (occupancy * bishopMagics[square]) >> (64 - bishopRelevantBits[square]);
                bitBishopAttacksTable[square][magicIndex] = bishopAttacksOnTheFly(square, occupancy);

            } else {
                const U64 occupancy { setOccupancies(index, relevantBitsCount, attackMask) };

                // for correctness this should be static casted
                const U64 magicIndex = (occupancy * rookMagics[square]) >> (64 - rookRelevantBits[square]);
                bitRookAttacksTable[square][magicIndex] = rookAttacksOnTheFly(square, occupancy);
            }
        }
    }
}


static void initRandomKeys() {
    constexpr unsigned int seed = 12345;

    std::mt19937 gen(seed);
    std::uniform_int_distribution<U64> dis(0, UINT64_MAX);
    
    for (int piece=0; piece < 12; piece++) {
        for(int square=0; square < 64; square++) {
            randomPieceKeys[piece][square] = dis(gen);
        }
    }

    for(int square=0; square < 64; square++) { randomEnPassantKeys[square] = dis(gen); }
    for(int bit=0; bit < 16; bit++) { randomCastlingKeys[bit] = dis(gen); }

    sideKey = dis(gen);
}


static U64 setFileAndRankMask(const int file, const int rank) {
    U64 mask{};
    for(int rankIndex=0; rankIndex < 8; rankIndex++) {
        for(int fileIndex=0; fileIndex < 8; fileIndex++) {
            const int square { rankIndex * 8 + fileIndex };

            if (file != -1 && file != 8) {
                if (fileIndex == file) SET_BIT(mask, square);
            } else if (rank != -1 && rank != 8) {
                if (rankIndex == rank) SET_BIT(mask, square);
            }
        }
    }
    return mask;
}


static void initEvaluationMasks() {
    for (int square=0; square < 64; square++) {
        const int file { square % 8 };
        const int rank { square / 8 };
        fileMasks[square] = setFileAndRankMask(file, -1);
        rankMasks[square] = setFileAndRankMask(-1, rank);
    }

    // now we do this for isolated masks
    for (int square=0; square < 64; square++) {
        const int file { square % 8 };
        isolatedPawnMasks[square] = setFileAndRankMask(file - 1, -1) | setFileAndRankMask(file + 1, -1);
    }

    // now we do this for passed masks
    // for white first
    for (int square=0; square < 64; square++) {
        const int file { square % 8 };
        const int rank { square / 8 };
        white_passedPawnMasks[square] = setFileAndRankMask(file - 1, -1) | setFileAndRankMask(file + 1, -1) | setFileAndRankMask(file, -1);
        black_passedPawnMasks[square] = setFileAndRankMask(file - 1, -1) | setFileAndRankMask(file + 1, -1) | setFileAndRankMask(file, -1);

        for (int i=0; i <= rank; i++) {
            white_passedPawnMasks[square] &= ~rankMasks[i * 8 + file];
        }
        for (int i=rank; i < 8; i++) {
            black_passedPawnMasks[square] &= ~rankMasks[i * 8 + file];
        }
    }
}


U64 squaresBetween[64][64]{};
U64 squaresLine[64][64]{};

// these functions now work :)
static int diagonal_of(const int s){
    return 7 + (s / 8) - (s % 8);
}
static int anti_diagonal_of(const int s) {
    return (s / 8) + (s % 8);
}
static void initialiseSquaresBetween() {

    // we do 1ULL << sq1 to get the board with a bit set on sq1 position
    for (int sq1 = 0; sq1 <= 63; ++sq1)
        for (int sq2 = 0; sq2 <= 63; ++sq2) {
            const U64 sqs = (1ULL << sq1) | (1ULL << sq2);

            if ( (fileMasks[sq1] == fileMasks[sq2]) || (rankMasks[sq1] == rankMasks[sq2]) ) {
                squaresBetween[sq1][sq2] = ( getRookAttacks(sq1, sqs) & getRookAttacks(sq2, sqs) );
            }

            else if ( (diagonal_of(sq1) == diagonal_of(sq2)) || (anti_diagonal_of(sq1) == anti_diagonal_of(sq2)) ) {
                squaresBetween[sq1][sq2] = getBishopAttacks(sq1, sqs) & getBishopAttacks(sq2, sqs);
            }
        }
}
static void initialiseLine() {
    for (int sq1 = 0; sq1 <= 63; ++sq1)
        for (int sq2 = 0; sq2 <= 63; ++sq2) {
            if (fileMasks[sq1] == fileMasks[sq2] || rankMasks[sq1] == rankMasks[sq2])
                squaresLine[sq1][sq2] = ( getRookAttacks(sq1, 0) & getRookAttacks(sq2, 0) ) | 1ULL << sq1 | 1ULL << sq2;

            else if (diagonal_of(sq1) == diagonal_of(sq2) || anti_diagonal_of(sq1) == anti_diagonal_of(sq2))
                squaresLine[sq1][sq2] = (getBishopAttacks(sq1, 0) & getBishopAttacks(sq2, 0) ) | 1ULL << sq1 | 1ULL << sq2;
        }
}


void initAll(const int ttSize) {

    initLeaperPiecesAttacks();

    initSliderAttacks(1);
    initSliderAttacks(0);

    initEvaluationMasks();

    initialiseSquaresBetween();
    initialiseLine();

    initRandomKeys();

    init_tables();


    initTranspositionTable(ttSize); // with 64 megabytes

    // these are the search boards
    initSearchTables();
}
