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
#include "search.h"

// here we put the Zobrist hashing and Trasposition tables
U64 randomPieceKeys[12][64]{};
U64 randomEnPassantKeys[64]{};
U64 randomCastlingKeys[16]{};
U64 sideKey{};

U64 hashKey{};



U64 generateHashKey() { // to uniquely identify a position
    U64 key{};
    U64 tempPieceBitboard{};


    for (int piece=0; piece<12; piece++) {
        tempPieceBitboard = bitboards[piece];

        while (tempPieceBitboard) {
            const int square = getLeastSigBitIndex(tempPieceBitboard);
            key ^= randomPieceKeys[piece][square];
            SET_BIT_FALSE(tempPieceBitboard, square);
        }
    }

    if (enPassantSQ != 64) key ^= randomEnPassantKeys[enPassantSQ];
    key ^= randomCastlingKeys[castle];
    if (side == Black) key ^= sideKey; // only done if side to move is black

    return key;
}

void clearTranspositionTable() {
    for (int index=0; index < HASH_SIZE; index++) {
        transpositionTable[index].hashKey=0;
        transpositionTable[index].depth=0;
        transpositionTable[index].flag=0;
        transpositionTable[index].score=0;
    }
}
// value for when no hash is found


int probeHash(const int alpha, const int beta, int* best_move, const int depth)
{
    // creates a pointer to the hash entry
    const tt* hashEntry { &transpositionTable[hashKey % HASH_SIZE] };

    // make sure we have the correct hashKey
    if (hashEntry->hashKey == hashKey) {
        if (hashEntry->depth >= depth) { // only look at same or higher depths evaluations

            // extracted stores score fmor transposition table
            int score = hashEntry->score;
            if (score < -MATE_SCORE) score += ply;
            if (score > MATE_SCORE) score -= ply;

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
    return NO_HASH_ENTRY;
}


tt transpositionTable[HASH_SIZE] {};

void recordHash(int score, const int bestMove, const int flag, const int depth)
{
    tt* hashEntry = &transpositionTable[hashKey % HASH_SIZE];

    // independent from distance of path taken from root node to current mating position
    if (score < -MATE_SCORE) score += ply;
    if (score > MATE_SCORE) score -= ply;

    hashEntry->hashKey = hashKey;
    hashEntry->score = score;
    hashEntry->flag = flag;
    hashEntry->depth = depth;
    hashEntry->bestMove = bestMove;
}

int checkHashOccupancy() {
    float count{};
    for (const tt position: transpositionTable) {
        if ( position.hashKey != 0) {
            count++;
        }
    }
    return static_cast<int>( 1'000 * ((count) / HASH_SIZE) );
}
