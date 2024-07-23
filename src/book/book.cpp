//
// Created by Federico Saitta on 23/07/2024.
//

#include "book.h"

#include "../include/macros.h"
#include "../include/init.h"
#include "../include/board.h"
#include "../include/inline_functions.h"
#include "../include/hashtable.h"


U64 generatePolyGlotKey() { // to uniquely identify a position
    U64 key{};
    U64 tempPieceBitboard{};

    for (int piece=0; piece<12; piece++) {
        tempPieceBitboard = bitboards[piece];

        while (tempPieceBitboard) {
            const int square = pop_lsb(&tempPieceBitboard);
            key ^= randomPieceKeys[piece][square];
        }
    }

    if (enPassantSQ != 64) {
        // should just loop over pawns and see if one of them can capture the enpassant square

        // for polyglot stuff
        U64 ourPawns{ bitboards[PAWN + 6*side] };
        if ( (GET_BIT(ourPawns, enPassantSQ - 1)) ) {
            // looking at the opponents pawns
            // checking that we are on the same rank for safety:
            if (getRankFromSquare[enPassantSQ - 1] == getRankFromSquare[enPassantSQ]) {
                std::cout << "en pass\n";
                key ^= randomEnPassantKeys[getFileFromSquare[enPassantSQ]];
            }
        }
        else if ( (GET_BIT(ourPawns, enPassantSQ + 1))) {
            if (getRankFromSquare[enPassantSQ + 1] == getRankFromSquare[enPassantSQ]) {
                std::cout << "en pass\n";
                key ^= randomEnPassantKeys[getFileFromSquare[enPassantSQ]];
            }
        }
    }
    if (castle & WK) key ^= randomCastlingKeys[0];
    if (castle & WQ) key ^= randomCastlingKeys[1];
    if (castle & BK) key ^= randomCastlingKeys[2];
    if (castle & BQ) key ^= randomCastlingKeys[3];

    if (side == WHITE) key ^= sideKey; // only done if side to move is black


    return key;
}
