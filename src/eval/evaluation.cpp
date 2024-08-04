//
// Created by Federico Saitta on 10/07/2024.
//
#include <iostream>
#include "../../include/macros.h"
#include "../../include/types.h"
#include "../../include/inline_functions.h"
#include "../../include/board.h"
 #include "evalparams.h"

#include <immintrin.h>
#include <algorithm>
#include "../../include/misc.h"
#define FLIP(sq) ((sq)^56)

//// ******* these boards are horizontally symmetric so no need for fancy flipping yet ////
// once you get to asymetterical boards make sure to find a little endian version
static int eval_table[12][64];

void init_tables()
{
    // flip white because of little endian
    for (int piece=0; piece < 6; piece++) {
        for (int square = 0; square < 64; square++) {

            eval_table[piece][square] = piece_value[piece] + pesto_table[piece][FLIP(square)];
            eval_table[piece + 6][square] = piece_value[piece] + pesto_table[piece][square];
        }
    }
}


int evaluate(const Board& pos) {
    int score[2]{};

    int gamePhase{};
    int square{};
    U64 bitboardCopy{}; // we need to ofc make a copy as we dont want to alter the bitboard

    for (int bbPiece=0; bbPiece < 12; bbPiece++) {
        bitboardCopy = pos.bitboards[bbPiece];

        // defo could improve the conditional branching in here
        while (bitboardCopy) {
            square = pop_lsb(&bitboardCopy);

            score[bbPiece / 6] += eval_table[bbPiece][square];

            gamePhase += gamephaseInc[bbPiece];

            // Note that some of these masks do not consider if the pawns are in front or behind the pieces
            switch(bbPiece) {
                case (PAWN):
                    // you could try and avoid conditional branching here
                    if ( countBits(pos.bitboards[PAWN] & fileMasks[square]) > 1) score[WHITE] += doublePawnPenalty * countBits(pos.bitboards[PAWN] & fileMasks[square]);

                    // adding penalties to isolated pawns
                    if ( (pos.bitboards[PAWN] & isolatedPawnMasks[square] ) == 0) score[WHITE] += isolatedPawnPenalty;

                    // adding bonuses to passed pawns
                    if ( (pos.bitboards[BLACK_PAWN] & white_passedPawnMasks[square] ) == 0) score[WHITE] += passedPawnBonus[getRankFromSquare[square]];
                    break;

                case (BLACK_PAWN):
                    // you could try and avoid conditional branching here
                    if ( countBits(pos.bitboards[BLACK_PAWN] & fileMasks[square]) > 1) score[BLACK] += doublePawnPenalty * countBits(pos.bitboards[BLACK_PAWN] & fileMasks[square]);

                    // adding penalties to isolated pawns
                    if ( (pos.bitboards[BLACK_PAWN] & isolatedPawnMasks[square] ) == 0) score[BLACK] += isolatedPawnPenalty;

                    // adding bonuses to passed pawns
                    if ( (pos.bitboards[PAWN] & black_passedPawnMasks[square] ) == 0) score[BLACK] += passedPawnBonus[7 - getRankFromSquare[square]];
                    break;

                case (ROOK):
                    if ( (pos.bitboards[PAWN] & fileMasks[square]) == 0) score[WHITE] += semiOpenFileScore;
                    if ( ( (pos.bitboards[PAWN] | pos.bitboards[BLACK_PAWN]) & fileMasks[square]) == 0) score[WHITE] += openFileScore;

                    score[WHITE] += RookMobility * countBits( getRookAttacks(square, pos.bitboards[BOTH_OCC]) );
                    break;

                case (BLACK_ROOK):
                    if ( (pos.bitboards[BLACK_PAWN] & fileMasks[square]) == 0) score[BLACK] += semiOpenFileScore;
                    if ( ( (pos.bitboards[PAWN] | pos.bitboards[BLACK_PAWN]) & fileMasks[square]) == 0) score[BLACK] += openFileScore;

                    score[BLACK] += RookMobility * countBits( getRookAttacks(square, pos.bitboards[BOTH_OCC]) );
                    break;

                // if the kings are on semi-open or open files they will be given penalties
                case (KING):
                    if ( (pos.bitboards[PAWN] & fileMasks[square]) == 0) score[WHITE] += kingSemiOpenFileScore;
                    if ( ( (pos.bitboards[PAWN] | pos.bitboards[BLACK_PAWN]) & fileMasks[square]) == 0) score[WHITE] += kingOpenFileScore;

                    score[WHITE] += kingShieldBonus * countBits( bitKingAttacks[square] & pos.bitboards[WHITE_OCC] );
                    break;

                case (BLACK_KING):
                    if ( (pos.bitboards[BLACK_PAWN] & fileMasks[square]) == 0) score[BLACK] += kingSemiOpenFileScore;
                    if ( ( (pos.bitboards[PAWN] | pos.bitboards[BLACK_PAWN]) & fileMasks[square]) == 0) score[BLACK] += kingOpenFileScore;

                    score[BLACK] += kingShieldBonus * countBits( bitKingAttacks[square] & pos.bitboards[BLACK_OCC] );
                    break;


                // mobility scores for sliding pieces except rooks, please test these and stop adding new features
                // these are very basic implementations, like the ones in fruit
                case (BISHOP):
                    score[WHITE] += BishopMobility * countBits( getBishopAttacks(square, pos.bitboards[BOTH_OCC]) );
                    break;

                case (BLACK_BISHOP):
                    score[BLACK] += BishopMobility * countBits( getBishopAttacks(square, pos.bitboards[BOTH_OCC]) );
                    break;

                case (KNIGHT):
                    score[WHITE] += KnightMobility * countBits( bitKnightAttacks[square] & pos.bitboards[BOTH_OCC] );
                    break;

                case (BLACK_KNIGHT):
                    score[BLACK] += KnightMobility * countBits( bitKnightAttacks[square] & pos.bitboards[BOTH_OCC] );
                    break;

                case (QUEEN):
                    score[WHITE] += QueenMobility * countBits( getQueenAttacks(square, pos.bitboards[BOTH_OCC]) );
                    break;

                case (BLACK_QUEEN):
                    score[BLACK] += QueenMobility * countBits( getQueenAttacks(square, pos.bitboards[BOTH_OCC]) );
                    break;

                default:
                    break;
            }
        }
    }

    // applying bishop pair bonus:
    if (countBits(pos.bitboards[WHITE_BISHOP]) > 1) score[WHITE] += bishopPairBonus;
    if (countBits(pos.bitboards[BLACK_BISHOP]) > 1) score[BLACK] += bishopPairBonus;

    const int mgScore = MgScore(score[pos.side]) - MgScore(score[pos.side^1]);
    const int egScore = EgScore(score[pos.side]) - EgScore(score[pos.side^1]);

    int mgPhase = gamePhase;
    if (mgPhase > 24) mgPhase = 24;  // in case of early promotion

    const int egPhase = 24 - mgPhase;
    return ((mgScore * mgPhase + egScore * egPhase) / 24);
}



