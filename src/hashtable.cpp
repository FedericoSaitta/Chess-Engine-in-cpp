//
// Created by Federico Saitta on 08/07/2024.
//
#include <iostream>
#include <fstream>
#include <string>
#include <random>
#include <vector>
#include <cstdint>

#include "globals.h"
#include "macros.h"
#include "inline_functions.h"
#include "evaluation.h"
#include "misc.h"

// here we put the Zobrist hashing and Trasposition tables
U64 randomPieceKeys[12][64]{};
U64 randomEnPassantKeys[64]{};
U64 randomCastlingKeys[16]{};
U64 sideKey{};

U64 hashKey{};


void initRandomKeys() {
//    std::random_device rd;
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

U64 generateHashKey() { // to uniquely identify a position
    U64 key{};
    U64 tempPieceBitboard{};


    for (int piece=0; piece<12; piece++) {
        tempPieceBitboard = bitboards[piece];

        while (tempPieceBitboard) {
            const int square = getLeastSigBitIndex(tempPieceBitboard);
            key ^= randomPieceKeys[piece][square];
            setBitFalse(tempPieceBitboard, square);
        }
    }

    if (enPassantSQ != 64) key ^= randomEnPassantKeys[enPassantSQ];
    key ^= randomCastlingKeys[castle];
    if (side == Black) key ^= sideKey; // only done if side to move is black

    return key;
}




