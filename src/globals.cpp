//
// Created by Federico Saitta on 02/07/2024.
//
#include "globals.h"

#include <variant>

#include "macros.h"


// //////////////// //
//**** init.cpp ****//
// //////////////// //


// ARRAYS NEEDED FOR MAGIC BITBOARD APPROACH //
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

BITBOARD notAFile{18374403900871474942ULL};
BITBOARD notABFile{18229723555195321596ULL};
BITBOARD notHFile{9187201950435737471ULL};
BITBOARD notHGFile{4557430888798830399ULL};

BITBOARD pawnAttacks[2][64];
BITBOARD bitKnightAttacks[64];
BITBOARD bitKingAttacks[64];

BITBOARD bitBishopAttacks[64];
BITBOARD bitRookAttacks[64];
BITBOARD bitQueenAttacks[64];

BITBOARD bitBishopAttacksTable[64][512];
BITBOARD bitRookAttacksTable[64][4096];


// //////////////// //
//**** board.cpp ****//
// //////////////// //
const char* chessBoard[65] = {
    "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
    "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
    "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
    "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
    "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
    "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
    "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
    "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8", "--" //represents empty square
};

BITBOARD bitboards[12];
BITBOARD occupancies[3];
int side;
int enPassantSQ;
int castle;


// //////////////// //
//**** misc.cpp ****//
// //////////////// //
const char promotedPieces[] = { [Queen] = 'q', [Rook] = 'r', [Bishop] = 'b', [Knight] = 'n',
                            [Queen + 6] = 'q', [Rook + 6] = 'r', [Bishop + 6] = 'b', [Knight + 6] = 'n',
                            [0] =  ' ' }; // these are always lowercase for both colors

const char* unicodePieces[] { "♟", "♞", "♝", "♜", "♛", "♚", // White
                              "♙", "♘", "♗", "♖", "♕", "♔"}; // Black

int gameLengthTime{};
int whiteClockTime{};
int blackClockTime{};
int whiteIncrementTime{};
int blackIncrementTime{};


void clearTranspositionTable() {
    for (int index=0; index < HASH_SIZE; index++) {
        transpositionTable[index].hashKey=0;
        transpositionTable[index].depth=0;
        transpositionTable[index].flag=0;
        transpositionTable[index].score=0;
    }
}
// value for when no hash is found


int probeHash(const int alpha, const int beta, const int depth)
{
    // creates a pointer to the hash entry
    tt* hashEntry { &transpositionTable[hashKey % HASH_SIZE] };

    // make sure we have the correct hashKey
    if (hashEntry->hashKey == hashKey) {
        if (hashEntry->depth >= depth) { // only look at same or higher depths evaluations

            if (hashEntry->flag == HASH_FLAG_EXACT)
                return hashEntry->score;

            // do some reading on why we are returning alpha and beta
            if ((hashEntry->flag == HASH_FLAG_ALPHA) && (hashEntry->score <= alpha))
                return alpha;

            if ((hashEntry->flag == HASH_FLAG_BETA) && (hashEntry->score >= beta))
                return beta;
        }
        // RememberBestMove(); you can add this in the future
    }
    return NO_HASH_ENTRY;
}


tt transpositionTable[HASH_SIZE] {};

void recordHash(const int score, const int flag, const int depth)
{
    tt* hashEntry = &transpositionTable[hashKey % HASH_SIZE];

    hashEntry->hashKey = hashKey;
    hashEntry->score = score;
    hashEntry->flag = flag;
    hashEntry->depth = depth;
    //hashEntry->best = BestMove(); can add this later
}



// //////////////// //
//**** uci.cpp ****//
// //////////////// //
std::ofstream logFile{};