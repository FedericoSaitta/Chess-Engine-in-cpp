//
// Created by Federico Saitta on 10/07/2024.
//
#include <iostream>
#include "../../include/macros.h"
#include "../../include/inline_functions.h"
#include "../../include/board.h"
#include "evalparams.h"

#include <immintrin.h>
#include <algorithm>
#include "../../include/misc.h"
#define FLIP(sq) ((sq)^56)

//// ******* these boards are horizontally symmetric so no need for fancy flipping yet ////
// once you get to asymetterical boards make sure to find a little endian version

int mg_value[6] = { 82, 337, 365, 477, 1025,  0};
int eg_value[6] = { 94, 281, 297, 512,  936,  0};

int mg_table[12][64];
int eg_table[12][64];


void init_tables()
{
    // flip white because of little endian
    for (int piece=0; piece < 6; piece++) {
        for (int square = 0; square < 64; square++) {
            mg_table[piece]  [square] = mg_value[piece] + mg_pesto_table[piece][FLIP(square)];
            eg_table[piece]  [square] = eg_value[piece] + eg_pesto_table[piece][FLIP(square)];
            mg_table[piece+6][square] = mg_value[piece] + mg_pesto_table[piece][(square)];
            eg_table[piece+6][square] = eg_value[piece] + eg_pesto_table[piece][(square)];
        }
    }
}


int evaluate() {
    int mg[2]{};
    int eg[2]{};

    int mgMobility[2]{};
    int egMobility[2]{};

    int gamePhase{};
    int penalties[2]{}; // 0 is white, 1 is black
    int square{};
    U64 bitboardCopy{}; // we need to ofc make a copy as we dont want to alter the bitboard

    for (int bbPiece=0; bbPiece < 12; bbPiece++) {
        bitboardCopy = bitboards[bbPiece];

        // defo could improve the conditional branching in here
        while (bitboardCopy) {
            square = pop_lsb(&bitboardCopy);

            // std::cout << "Piece "  << " square: " << chessBoard[square] << " middlegame value: " << mg_table[bbPiece][square] << '\n';
            // std::cout << "Piece " << " square: " << chessBoard[square] << " engdame value: " << eg_table[bbPiece][square] << '\n';

            mg[bbPiece / 6] += mg_table[bbPiece][square];
            eg[bbPiece / 6] += eg_table[bbPiece][square];
            gamePhase += gamephaseInc[bbPiece];


            // Note that some of these masks do not consider if the pawns are in front or behind the pieces
            switch(bbPiece) {
                case (PAWN):
                    // you could try and avoid conditional branching here
                    if ( countBits(bitboards[PAWN] & fileMasks[square]) > 1) penalties[WHITE] += doublePawnPenalty * countBits(bitboards[PAWN] & fileMasks[square]);

                    // adding penalties to isolated pawns
                    if ( (bitboards[PAWN] & isolatedPawnMasks[square] ) == 0) penalties[WHITE] += isolatedPawnPenalty;

                    // adding bonuses to passed pawns
                    if ( (bitboards[BLACK_PAWN] & white_passedPawnMasks[square] ) == 0) penalties[WHITE] += passedPawnBonus[getRankFromSquare[square]];
                    break;

                case (BLACK_PAWN):
                    // you could try and avoid conditional branching here
                        if ( countBits(bitboards[BLACK_PAWN] & fileMasks[square]) > 1) penalties[BLACK] += doublePawnPenalty * countBits(bitboards[BLACK_PAWN] & fileMasks[square]);

                    // adding penalties to isolated pawns
                    if ( (bitboards[BLACK_PAWN] & isolatedPawnMasks[square] ) == 0) penalties[BLACK] += isolatedPawnPenalty;

                    // adding bonuses to passed pawns
                    if ( (bitboards[PAWN] & black_passedPawnMasks[square] ) == 0) penalties[BLACK] += passedPawnBonus[7 - getRankFromSquare[square]];
                    break;

                case (ROOK):
                    if ( (bitboards[PAWN] & fileMasks[square]) == 0) penalties[WHITE] += semiOpenFileScore;
                    if ( ( (bitboards[PAWN] | bitboards[BLACK_PAWN]) & fileMasks[square]) == 0) penalties[WHITE] += openFileScore;

                    mgMobility[WHITE] += RookMobOpening * countBits( getRookAttacks(square, occupancies[2]) );
                    egMobility[WHITE] += RookMobEndgame * countBits( getRookAttacks(square, occupancies[2]) );
                    break;

                case (BLACK_ROOK):
                    if ( (bitboards[BLACK_PAWN] & fileMasks[square]) == 0) penalties[BLACK] += semiOpenFileScore;
                    if ( ( (bitboards[PAWN] | bitboards[BLACK_PAWN]) & fileMasks[square]) == 0) penalties[BLACK] += openFileScore;

                    mgMobility[BLACK] += RookMobOpening * countBits( getRookAttacks(square, occupancies[2]) );
                    egMobility[BLACK] += RookMobEndgame * countBits( getRookAttacks(square, occupancies[2]) );
                    break;

                // if the kings are on semi-open or open files they will be given penalties
                case (KING):
                    if ( (bitboards[PAWN] & fileMasks[square]) == 0) penalties[WHITE] -= semiOpenFileScore;
                    if ( ( (bitboards[PAWN] | bitboards[BLACK_PAWN]) & fileMasks[square]) == 0) penalties[WHITE] -= openFileScore;

                    penalties[WHITE] += kingShieldBonus * countBits( bitKingAttacks[square] & occupancies[WHITE] );
                    break;

                case (BLACK_KING):
                    if ( (bitboards[BLACK_PAWN] & fileMasks[square]) == 0) penalties[BLACK] -= semiOpenFileScore;
                    if ( ( (bitboards[PAWN] | bitboards[BLACK_PAWN]) & fileMasks[square]) == 0) penalties[BLACK] -= openFileScore;

                    penalties[BLACK] += kingShieldBonus * countBits( bitKingAttacks[square] & occupancies[BLACK] );
                    break;



                // mobility scores for sliding pieces except rooks, please test these and stop adding new features
                // these are very basic implementations, like the ones in fruit
                case (BISHOP):
                    mgMobility[WHITE] += BishopMobOpening * countBits( getBishopAttacks(square, occupancies[2]) );
                    egMobility[WHITE] += BishopMobEndgame * countBits( getBishopAttacks(square, occupancies[2]) );
                    break;

                case (BLACK_BISHOP):
                    mgMobility[BLACK] += BishopMobOpening * countBits( getBishopAttacks(square, occupancies[2]) );
                    egMobility[BLACK] += BishopMobEndgame * countBits( getBishopAttacks(square, occupancies[2]) );
                    break;

                case (KNIGHT):
                    mgMobility[WHITE] += KnightMobOpening * countBits( bitKnightAttacks[square] & occupancies[2] );
                    egMobility[WHITE] += KnightMobEndgame * countBits( bitKnightAttacks[square] & occupancies[2] );
                    break;

                case (BLACK_KNIGHT):
                    mgMobility[BLACK] += KnightMobOpening * countBits( bitKnightAttacks[square] & occupancies[2] );
                    egMobility[BLACK] += KnightMobEndgame * countBits( bitKnightAttacks[square] & occupancies[2] );
                    break;

                // For now we dont assign any multipliers to quees
                case (QUEEN):
                    mgMobility[WHITE] += QueenMobOpening * countBits( getQueenAttacks(square, occupancies[2]) );
                    egMobility[WHITE] += QueenMobEndgame * countBits( getQueenAttacks(square, occupancies[2]) );
                    break;

                case (BLACK_QUEEN):
                    mgMobility[BLACK] += QueenMobOpening * countBits( getQueenAttacks(square, occupancies[2]) );
                    egMobility[BLACK] += QueenMobEndgame * countBits( getQueenAttacks(square, occupancies[2]) );
                    break;

                default:
                    break;
            }
        }

    }

    // std::cout << "mgScore white: " << mg[side] << " mgScore black: " << mg[side^1] << '\n';

    const int mgScore = mg[side] + mgMobility[side] - mg[side^1] - mgMobility[side^1];
    const int egScore = eg[side] + egMobility[side] - eg[side^1] - egMobility[side^1];
    int mgPhase = gamePhase;
    if (mgPhase > 24) mgPhase = 24;  // in case of early promotion

    const int egPhase = 24 - mgPhase;

    return ((mgScore * mgPhase + egScore * egPhase) / 24) + penalties[side] - penalties[side^1];
}


