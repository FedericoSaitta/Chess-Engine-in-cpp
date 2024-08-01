//
// Created by Federico Saitta on 08/07/2024.
//
#include <iostream>
#include <fstream>
#include <string>

#include <vector>
#include <cstdint>

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

U64 generateHashKey() { // to uniquely identify a position
    U64 key{};
    U64 tempPieceBitboard{};

    for (int piece=0; piece<12; piece++) {
        tempPieceBitboard = board.bitboards[piece];

        while (tempPieceBitboard) {
            const int square = pop_lsb(&tempPieceBitboard);
            key ^= randomPieceKeys[piece][square];
        }
    }

    if (board.history[board.gamePly].enPassSq != 64) key ^= randomEnPassantKeys[board.history[board.gamePly].enPassSq];

    key ^= randomCastlingKeys[board.history[board.gamePly].castle];
    if (board.side == BLACK) key ^= sideKey; // only done if side to move is black

    return key;
}


void initTranspositionTable(const int megaBytes) {
    const U64 hashSize = 0x100000 * megaBytes;
    transpotitionTableEntries = hashSize / sizeof(tt);

    if (transpositionTable != nullptr)
        clearTranspositionTable();

    transpositionTable = static_cast<tt *>(malloc(transpotitionTableEntries * sizeof(tt)));

    if (transpositionTable == nullptr) {
        std::cerr << "ERR Allocation of memory has failed\n";
        initTranspositionTable(megaBytes / 2);
    } else {
        // if the allocation has succeded
        clearTranspositionTable();
    }
}

void clearTranspositionTable() {
    for (int index=0; index < transpotitionTableEntries; index++) {
        transpositionTable[index].hashKey=0;
        transpositionTable[index].depth=0;
        transpositionTable[index].flag=0;
        transpositionTable[index].score=0;
        transpositionTable[index].bestMove=Move(0, 0);
    }
}



int probeHash(const int alpha, const int beta, Move* best_move, const int depth)
{
    // creates a pointer to the hash entry
    const tt* hashEntry { &transpositionTable[hashKey % transpotitionTableEntries] };

    // make sure we have the correct hashKey
    if (hashEntry->hashKey == hashKey) {
        if (hashEntry->depth >= depth) { // only look at same or higher depths evaluations

            // extracted stores score fmor transposition table
            int score = hashEntry->score;
            if (score < -MATE_SCORE) score += searchPly;
            if (score > MATE_SCORE) score -= searchPly;

            if (hashEntry->flag == HASH_FLAG_EXACT)
                return score;

            // do some reading on why we are returning alpha and beta
            if ((hashEntry->flag == HASH_FLAG_ALPHA) && (score <= alpha))
                return alpha;

            if ((hashEntry->flag == HASH_FLAG_BETA) && (score >= beta))
                return beta;
        }
        // store best move
        *best_move = hashEntry->bestMove;
    }
    return NO_HASH_ENTRY; // in case we dont get a tt hit
}

void recordHash(int score, const Move bestMove, const int flag, const int depth)
{
    tt* hashEntry = &transpositionTable[hashKey % transpotitionTableEntries];

    // independent from distance of path taken from root node to current mating position
    if (score < -MATE_SCORE) score += searchPly;
    if (score > MATE_SCORE) score -= searchPly;

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
    return static_cast<int>( 1'000 * ((count) / transpotitionTableEntries) );
}
