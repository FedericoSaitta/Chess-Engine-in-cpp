//
// Created by Federico Saitta on 04/07/2024.
//

#ifndef EVALUATION_H
#define EVALUATION_H

#include <iostream>

#include "globals.h"
#include "constants.h"
#include "inline_functions.h"
#include "misc.h"



//// ******* these boards are horizontally symmetric so no need for fancy flipping yet ////
// once you get to asymetterical boards make sure to find a little endian version

// later on you can add more complicated evaluation tables
constexpr int materialScore[12] { 100, 320, 330, 500, 900, 20'000,   // for white
                               -100, -320, -330, -500, -900, -20'000};  // for black

constexpr int pawnScores[64] { 0,  0,  0,  0,  0,  0,  0,  0,
                              50, 50, 50, 50, 50, 50, 50, 50,
                              10, 10, 20, 30, 30, 20, 10, 10,
                              5,  5, 10, 25, 25, 10,  5,  5,
                              0,  0,  0, 20, 20,  0,  0,  0,
                              5, -5,-10,  0,  0,-10, -5,  5,
                              5, 10, 10,-20,-20, 10, 10,  5,
                              0,  0,  0,  0,  0,  0,  0,  0};

constexpr int knightScores[64] {-50,-40,-30,-30,-30,-30,-40,-50,
                                -40,-20,  0,  0,  0,  0,-20,-40,
                                -30,  0, 10, 15, 15, 10,  0,-30,
                                -30,  5, 15, 20, 20, 15,  5,-30,
                                -30,  0, 15, 20, 20, 15,  0,-30,
                                -30,  5, 10, 15, 15, 10,  5,-30,
                                -40,-20,  0,  5,  5,  0,-20,-40,
                                -50,-40,-30,-30,-30,-30,-40,-50};

constexpr int bishopScores[64] {-20,-10,-10,-10,-10,-10,-10,-20,
                                -10,  0,  0,  0,  0,  0,  0,-10,
                                -10,  0,  5, 10, 10,  5,  0,-10,
                                -10,  5,  5, 10, 10,  5,  5,-10,
                                -10,  0, 10, 10, 10, 10,  0,-10,
                                -10, 10, 10, 10, 10, 10, 10,-10,
                                -10,  5,  0,  0,  0,  0,  5,-10,
                                -20,-10,-10,-10,-10,-10,-10,-20};

constexpr int rookScores[64] {0,  0,  0,  0,  0,  0,  0,  0,
                              5, 10, 10, 10, 10, 10, 10,  5,
                             -5,  0,  0,  0,  0,  0,  0, -5,
                             -5,  0,  0,  0,  0,  0,  0, -5,
                             -5,  0,  0,  0,  0,  0,  0, -5,
                             -5,  0,  0,  0,  0,  0,  0, -5,
                             -5,  0,  0,  0,  0,  0,  0, -5,
                              0,  0,  0,  5,  5,  0,  0,  0};

constexpr int queenScores[64] {-20,-10,-10, -5, -5,-10,-10,-20,
                                -10,  0,  0,  0,  0,  0,  0,-10,
                                -10,  0,  5,  5,  5,  5,  0,-10,
                                 -5,  0,  5,  5,  5,  5,  0, -5,
                                  0,  0,  5,  5,  5,  5,  0, -5,
                                -10,  5,  5,  5,  5,  5,  0,-10,
                                -10,  0,  5,  0,  0,  0,  0,-10,
                                -20,-10,-10, -5, -5,-10,-10,-20};


constexpr int kingScores[64] {-30,-40,-40,-50,-50,-40,-40,-30,
                        -30,-40,-40,-50,-50,-40,-40,-30,
                        -30,-40,-40,-50,-50,-40,-40,-30,
                        -30,-40,-40,-50,-50,-40,-40,-30,
                        -20,-30,-30,-40,-40,-30,-30,-20,
                        -10,-20,-20,-20,-20,-20,-20,-10,
                         20, 20,  0,  0,  0,  0, 20, 20,
                         20, 30, 10,  0,  0, 10, 30, 20};

// now H8 is the first one and A1 is the last one
constexpr int mirrorScore[64] = {A8, B8, C8, D8, E8, F8, G8, H8,
                                A7, B7, C7, D7, E7, F7, G7, H7,
                                A6, B6, C6, D6, E6, F6, G6, H6,
                                A5, B5, C5, D5, E5, F5, G5, H5,
                                A4, B4, C4, D4, E4, F4, G4, H4,
                                A3, B3, C3, D3, E3, F3, G3, H3,
                                A2, B2, C2, D2, E2, F2, G2, H2,
                                A1, B1, C1, D1, E1, F1, G1, H1};
inline int evaluate() {
    int score {};
    int square;
    U64 bitboardCopy; // we need to ofc make a copy as we dont want to alter the bitboard

    for (int bbPiece=0; bbPiece < 12; bbPiece++) {
        bitboardCopy = bitboards[bbPiece];

        while (bitboardCopy) {

            square = getLeastSigBitIndex(bitboardCopy);

            score += materialScore[bbPiece];

            // this could defo be made better
            switch (bbPiece) {
                case Pawn: score += pawnScores[square[mirrorScore]]; break;
                case Knight: score += knightScores[square[mirrorScore]]; break;
                case Bishop: score += bishopScores[square[mirrorScore]]; break;
                case Rook: score += rookScores[square[mirrorScore]]; break;
                case Queen: score += queenScores[square[mirrorScore]]; break;
                case King: score += kingScores[square[mirrorScore]]; break;

                case (Pawn + 6): score -= pawnScores[square]; break;
                case (Knight + 6): score -= knightScores[square]; break;
                case (Bishop + 6): score -= bishopScores[square]; break;
                case (Rook + 6): score -= rookScores[square]; break;
                case (Queen + 6): score -= queenScores[square]; break;
                case (King + 6): score -= kingScores[square]; break;

                default: break;
            }

            setBitFalse(bitboardCopy, square);
        }

    }
    return (side == White) ? score : -score;

}




#endif //EVALUATION_H
