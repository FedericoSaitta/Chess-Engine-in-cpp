#include <iostream>
#include <fstream>
#include <string>
#include <assert.h>

#include <cstdint>
#include <algorithm>
#include <bit>
#include <new>

#include "macros.h"
#include "board.h"
#include "inline_functions.h"

#include "hashtable.h"
#include "search/search.h"

// here we put the Zobrist hashing and Trasposition tables
U64 randomPieceKeys[12][64]{};
U64 randomEnPassantKeys[64]{};
U64 randomCastlingKeys[16]{};
U64 sideKey{};

U64 hashKey{};

std::int64_t transpotitionTableEntries{};
tt* transpositionTable{ nullptr };
static U64 transpositionTableMask{};

U64 generateHashKey(const Board& pos) { // to uniquely identify a position
    U64 key{};
    U64 tempPieceBitboard{};

    for (int piece=0; piece<12; piece++) {
        tempPieceBitboard = pos.bitboards[piece];

        while (tempPieceBitboard) {
            const int square = pop_lsb(&tempPieceBitboard);
            key ^= randomPieceKeys[piece][square];
        }
    }

    if (pos.history[pos.gamePly].enPassSq != 64) key ^= randomEnPassantKeys[pos.history[pos.gamePly].enPassSq];

    key ^= randomCastlingKeys[pos.history[pos.gamePly].castle];
    if (pos.side == BLACK) key ^= sideKey; // only done if side to move is black

    return key;
}

// https://web.archive.org/web/20071031100051/http://www.brucemo.com/compchess/programming/hashing.htm
void initTranspositionTable(const int megaBytes) {
    const U64 requestedBytes = 0x100000ULL * std::max(megaBytes, 1);
    U64 entries = std::bit_floor(requestedBytes / sizeof(tt));
    tt* replacement = nullptr;

    while (entries > 0 && replacement == nullptr) {
        replacement = new (std::nothrow) tt[entries]{};
        if (replacement == nullptr) entries /= 2;
    }

    if (replacement == nullptr) {
        std::cerr << "ERR Allocation of transposition table failed\n";
        return;
    }

    delete[] transpositionTable;
    transpositionTable = replacement;
    transpotitionTableEntries = static_cast<std::int64_t>(entries);
    transpositionTableMask = entries - 1;
}

void freeTranspositionTable() {
    delete[] transpositionTable;
    transpositionTable = nullptr;
    transpotitionTableEntries = 0;
    transpositionTableMask = 0;
}

void clearTranspositionTable() {
    if (transpositionTable != nullptr) { // extra check for ucinewgame
        std::fill_n(transpositionTable, transpotitionTableEntries, tt{});
    }
}

int probeHash(const int alpha, const int beta, Move* best_move, const int depth, const int searchPly)
{
    // creates a pointer to the hash entry
    assert(transpositionTable != nullptr && transpotitionTableEntries > 0);
    const tt* hashEntry { &transpositionTable[hashKey & transpositionTableMask] };

    // make sure we have the correct hashKey, not sure about the depth line
    if (hashEntry->hashKey == hashKey) {
        if (hashEntry->depth >= depth) { // only look at same or higher depths evaluations

            // extracted stores score from transposition table
            int score = hashEntry->score;
            if (score < -MATE_SCORE) score += searchPly;
            if (score > MATE_SCORE) score -= searchPly;

            if (hashEntry->flag == HASH_FLAG_EXACT)
                return score;

            // you could change this to be fail soft
            // do some reading on why we are returning alpha and beta
            if ((hashEntry->flag == HASH_FLAG_ALPHA) && (score <= alpha))
                return alpha;

            if ((hashEntry->flag == HASH_FLAG_BETA) && (score >= beta))
                return beta;
        }
        // store best move
        assert(!hashEntry->bestMove.isNone() && "probeHash: trying to probe a null Move");
        *best_move = hashEntry->bestMove;
    }
    return NO_HASH_ENTRY; // in case we dont get a tt hit
}

void recordHash(int score, const Move bestMove, const int flag, const int depth, const int searchPly)
{
    assert(transpositionTable != nullptr && transpotitionTableEntries > 0);
    tt* hashEntry = &transpositionTable[hashKey & transpositionTableMask];

    // independent from distance of path taken from root node to current mating position
    if (score < -MATE_SCORE) score -= searchPly;
    if (score > MATE_SCORE) score += searchPly;

    assert(!bestMove.isNone() && "recordHash: Trying to store a null move");

    hashEntry->hashKey = hashKey;
    hashEntry->score = score;
    hashEntry->flag = flag;
    hashEntry->depth = depth;
    hashEntry->bestMove = bestMove;
}

int checkHashOccupancy() {
    float count {};
    for (const tt* position = transpositionTable; position < transpositionTable + transpotitionTableEntries; ++position) {
        if (position->hashKey != 0) {
            count++;
        }
    }
    return static_cast<int>( 1'000 * (count / transpotitionTableEntries) );
}
