//
// Created by Federico Saitta on 10/07/2024.
//
#include <iostream>
#include "macros.h"
#include "inline_functions.h"
#include "board.h"

#include "misc.h"
#define FLIP(sq) ((sq)^56)

//// ******* these boards are horizontally symmetric so no need for fancy flipping yet ////
// once you get to asymetterical boards make sure to find a little endian version

int mg_value[6] = { 82, 337, 365, 477, 1025,  0};
int eg_value[6] = { 94, 281, 297, 512,  936,  0};

// From Fruit engine
static constexpr int KnightMobOpening = 4;
static constexpr int KnightMobEndgame = 4;
static constexpr int BishopMobOpening = 5;
static constexpr int BishopMobEndgame = 5;
static constexpr int RookMobOpening = 2;
static constexpr int RookMobEndgame = 4;
static constexpr int QueenMobOpening = 1;
static constexpr int QueenMobEndgame = 2;

/* piece/sq tables */
/* values from Rofchade: http://www.talkchess.com/forum3/viewtopic.php?f=2&t=68311&start=19 */
int mg_pawn_table[64] = {
      0,   0,   0,   0,   0,   0,  0,   0,
     98, 134,  61,  95,  68, 126, 34, -11,
     -6,   7,  26,  31,  65,  56, 25, -20,
    -14,  13,   6,  21,  23,  12, 17, -23,
    -27,  -2,  -5,  12,  17,   6, 10, -25,
    -26,  -4,  -4, -10,   3,   3, 33, -12,
    -35,  -1, -20, -23, -15,  24, 38, -22,
      0,   0,   0,   0,   0,   0,  0,   0,
};

int eg_pawn_table[64] = {
      0,   0,   0,   0,   0,   0,   0,   0,
    178, 173, 158, 134, 147, 132, 165, 187,
     94, 100,  85,  67,  56,  53,  82,  84,
     32,  24,  13,   5,  -2,   4,  17,  17,
     13,   9,  -3,  -7,  -7,  -8,   3,  -1,
      4,   7,  -6,   1,   0,  -5,  -1,  -8,
     13,   8,   8,  10,  13,   0,   2,  -7,
      0,   0,   0,   0,   0,   0,   0,   0,
};

int mg_knight_table[64] = {
    -167, -89, -34, -49,  61, -97, -15, -107,
     -73, -41,  72,  36,  23,  62,   7,  -17,
     -47,  60,  37,  65,  84, 129,  73,   44,
      -9,  17,  19,  53,  37,  69,  18,   22,
     -13,   4,  16,  13,  28,  19,  21,   -8,
     -23,  -9,  12,  10,  19,  17,  25,  -16,
     -29, -53, -12,  -3,  -1,  18, -14,  -19,
    -105, -21, -58, -33, -17, -28, -19,  -23,
};

int eg_knight_table[64] = {
    -58, -38, -13, -28, -31, -27, -63, -99,
    -25,  -8, -25,  -2,  -9, -25, -24, -52,
    -24, -20,  10,   9,  -1,  -9, -19, -41,
    -17,   3,  22,  22,  22,  11,   8, -18,
    -18,  -6,  16,  25,  16,  17,   4, -18,
    -23,  -3,  -1,  15,  10,  -3, -20, -22,
    -42, -20, -10,  -5,  -2, -20, -23, -44,
    -29, -51, -23, -15, -22, -18, -50, -64,
};

int mg_bishop_table[64] = {
    -29,   4, -82, -37, -25, -42,   7,  -8,
    -26,  16, -18, -13,  30,  59,  18, -47,
    -16,  37,  43,  40,  35,  50,  37,  -2,
     -4,   5,  19,  50,  37,  37,   7,  -2,
     -6,  13,  13,  26,  34,  12,  10,   4,
      0,  15,  15,  15,  14,  27,  18,  10,
      4,  15,  16,   0,   7,  21,  33,   1,
    -33,  -3, -14, -21, -13, -12, -39, -21,
};

int eg_bishop_table[64] = {
    -14, -21, -11,  -8, -7,  -9, -17, -24,
     -8,  -4,   7, -12, -3, -13,  -4, -14,
      2,  -8,   0,  -1, -2,   6,   0,   4,
     -3,   9,  12,   9, 14,  10,   3,   2,
     -6,   3,  13,  19,  7,  10,  -3,  -9,
    -12,  -3,   8,  10, 13,   3,  -7, -15,
    -14, -18,  -7,  -1,  4,  -9, -15, -27,
    -23,  -9, -23,  -5, -9, -16,  -5, -17,
};

int mg_rook_table[64] = {
     32,  42,  32,  51, 63,  9,  31,  43,
     27,  32,  58,  62, 80, 67,  26,  44,
     -5,  19,  26,  36, 17, 45,  61,  16,
    -24, -11,   7,  26, 24, 35,  -8, -20,
    -36, -26, -12,  -1,  9, -7,   6, -23,
    -45, -25, -16, -17,  3,  0,  -5, -33,
    -44, -16, -20,  -9, -1, 11,  -6, -71,
    -19, -13,   1,  17, 16,  7, -37, -26,
};

int eg_rook_table[64] = {
    13, 10, 18, 15, 12,  12,   8,   5,
    11, 13, 13, 11, -3,   3,   8,   3,
     7,  7,  7,  5,  4,  -3,  -5,  -3,
     4,  3, 13,  1,  2,   1,  -1,   2,
     3,  5,  8,  4, -5,  -6,  -8, -11,
    -4,  0, -5, -1, -7, -12,  -8, -16,
    -6, -6,  0,  2, -9,  -9, -11,  -3,
    -9,  2,  3, -1, -5, -13,   4, -20,
};

int mg_queen_table[64] = {
    -28,   0,  29,  12,  59,  44,  43,  45,
    -24, -39,  -5,   1, -16,  57,  28,  54,
    -13, -17,   7,   8,  29,  56,  47,  57,
    -27, -27, -16, -16,  -1,  17,  -2,   1,
     -9, -26,  -9, -10,  -2,  -4,   3,  -3,
    -14,   2, -11,  -2,  -5,   2,  14,   5,
    -35,  -8,  11,   2,   8,  15,  -3,   1,
     -1, -18,  -9,  10, -15, -25, -31, -50,
};

int eg_queen_table[64] = {
     -9,  22,  22,  27,  27,  19,  10,  20,
    -17,  20,  32,  41,  58,  25,  30,   0,
    -20,   6,   9,  49,  47,  35,  19,   9,
      3,  22,  24,  45,  57,  40,  57,  36,
    -18,  28,  19,  47,  31,  34,  39,  23,
    -16, -27,  15,   6,   9,  17,  10,   5,
    -22, -23, -30, -16, -16, -23, -36, -32,
    -33, -28, -22, -43,  -5, -32, -20, -41,
};

int mg_king_table[64] = {
    -65,  23,  16, -15, -56, -34,   2,  13,
     29,  -1, -20,  -7,  -8,  -4, -38, -29,
     -9,  24,   2, -16, -20,   6,  22, -22,
    -17, -20, -12, -27, -30, -25, -14, -36,
    -49,  -1, -27, -39, -46, -44, -33, -51,
    -14, -14, -22, -46, -44, -30, -15, -27,
      1,   7,  -8, -64, -43, -16,   9,   8,
    -15,  36,  12, -54,   8, -28,  24,  14,
};

int eg_king_table[64] = {
    -74, -35, -18, -18, -11,  15,   4, -17,
    -12,  17,  14,  17,  17,  38,  23,  11,
     10,  17,  23,  15,  20,  45,  44,  13,
     -8,  22,  24,  27,  26,  33,  26,   3,
    -18,  -4,  21,  24,  27,  23,   9, -11,
    -19,  -3,  11,  21,  23,  16,   7,  -9,
    -27, -11,   4,  13,  14,   4,  -5, -17,
    -53, -34, -21, -11, -28, -14, -24, -43
};

int* mg_pesto_table[6] =
{
    mg_pawn_table,
    mg_knight_table,
    mg_bishop_table,
    mg_rook_table,
    mg_queen_table,
    mg_king_table
};

int* eg_pesto_table[6] =
{
    eg_pawn_table,
    eg_knight_table,
    eg_bishop_table,
    eg_rook_table,
    eg_queen_table,
    eg_king_table
};

int gamephaseInc[12] = {0, 1, 1, 2, 4, 0, 0, 1, 1, 2, 4, 0,};
int mg_table[12][64];
int eg_table[12][64];

constexpr int doublePawnPenalty = -10;
constexpr int isolatedPawnPenalty = -10;
constexpr int passedPawnBonus[8] {0, 5, 10, 20, 35, 60, 100, 200};

constexpr int semiOpenFileScore { 10 };
constexpr int openFileScore { 15 };

constexpr int kingShieldBonus { 5 };


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
            square = getLeastSigBitIndex(bitboardCopy);

            // std::cout << "Piece "  << " square: " << chessBoard[square] << " middlegame value: " << mg_table[bbPiece][square] << '\n';
            // std::cout << "Piece " << " square: " << chessBoard[square] << " engdame value: " << eg_table[bbPiece][square] << '\n';

            mg[(bbPiece < 6) ? 0 : 1] += mg_table[bbPiece][square];
            eg[(bbPiece < 6) ? 0 : 1] += eg_table[bbPiece][square];
            gamePhase += gamephaseInc[bbPiece];



            // Note that some of these masks do not consider if the pawns are in front or behind the pieces
            switch(bbPiece) {
                case (Pawn):
                    // you could try and avoid conditional branching here
                        if ( countBits(bitboards[Pawn] & fileMasks[square]) > 1) penalties[White] += doublePawnPenalty * countBits(bitboards[Pawn] & fileMasks[square]);

                    // adding penalties to isolated pawns
                    if ( (bitboards[Pawn] & isolatedPawnMasks[square] ) == 0) penalties[White] += isolatedPawnPenalty;

                    // adding bonuses to passed pawns
                    if ( (bitboards[Pawn + 6] & white_passedPawnMasks[square] ) == 0) penalties[White] += passedPawnBonus[getRankFromSquare[square]];
                    break;

                case (Pawn + 6):
                    // you could try and avoid conditional branching here
                        if ( countBits(bitboards[Pawn + 6] & fileMasks[square]) > 1) penalties[Black] += doublePawnPenalty * countBits(bitboards[Pawn + 6] & fileMasks[square]);

                    // adding penalties to isolated pawns
                    if ( (bitboards[Pawn + 6] & isolatedPawnMasks[square] ) == 0) penalties[Black] += isolatedPawnPenalty;

                    // adding bonuses to passed pawns
                    if ( (bitboards[Pawn] & black_passedPawnMasks[square] ) == 0) penalties[Black] += passedPawnBonus[7 - getRankFromSquare[square]];
                    break;

                case (Rook):
                    if ( (bitboards[Pawn] & fileMasks[square]) == 0) penalties[White] += semiOpenFileScore;
                    if ( ( (bitboards[Pawn] | bitboards[Pawn + 6]) & fileMasks[square]) == 0) penalties[White] += openFileScore;

                    mgMobility[White] += RookMobOpening * countBits( getRookAttacks(square, occupancies[2]) );
                    egMobility[White] += RookMobEndgame * countBits( getRookAttacks(square, occupancies[2]) );
                    break;

                case (Rook + 6):
                    if ( (bitboards[Pawn + 6] & fileMasks[square]) == 0) penalties[Black] += semiOpenFileScore;
                    if ( ( (bitboards[Pawn] | bitboards[Pawn + 6]) & fileMasks[square]) == 0) penalties[Black] += openFileScore;

                    mgMobility[Black] += RookMobOpening * countBits( getRookAttacks(square, occupancies[2]) );
                    egMobility[Black] += RookMobEndgame * countBits( getRookAttacks(square, occupancies[2]) );
                    break;

                // if the kings are on semi-open or open files they will be given penalties
                case (King):
                    if ( (bitboards[Pawn] & fileMasks[square]) == 0) penalties[White] -= semiOpenFileScore;
                    if ( ( (bitboards[Pawn] | bitboards[Pawn + 6]) & fileMasks[square]) == 0) penalties[White] -= openFileScore;

                    penalties[White] += kingShieldBonus * countBits( bitKingAttacks[square] & occupancies[White] );
                    break;

                case (King + 6):
                    if ( (bitboards[Pawn + 6] & fileMasks[square]) == 0) penalties[Black] -= semiOpenFileScore;
                    if ( ( (bitboards[Pawn] | bitboards[Pawn + 6]) & fileMasks[square]) == 0) penalties[Black] -= openFileScore;

                    penalties[Black] += kingShieldBonus * countBits( bitKingAttacks[square] & occupancies[Black] );
                    break;



                // mobility scores for sliding pieces except rooks, please test these and stop adding new features
                // these are very basic implementations, like the ones in fruit
                case (Bishop):
                    mgMobility[White] += BishopMobOpening * countBits( getBishopAttacks(square, occupancies[2]) );
                    egMobility[White] += BishopMobEndgame * countBits( getBishopAttacks(square, occupancies[2]) );
                    break;

                case (Bishop + 6):
                    mgMobility[Black] += BishopMobOpening * countBits( getBishopAttacks(square, occupancies[2]) );
                    egMobility[Black] += BishopMobEndgame * countBits( getBishopAttacks(square, occupancies[2]) );
                    break;

                case (Knight):
                    mgMobility[White] += KnightMobOpening * countBits( bitKnightAttacks[square] & occupancies[2] );
                    egMobility[White] += KnightMobEndgame * countBits( bitKnightAttacks[square] & occupancies[2] );
                    break;

                case (Knight + 6):
                    mgMobility[Black] += KnightMobOpening * countBits( bitKnightAttacks[square] & occupancies[2] );
                    egMobility[Black] += KnightMobEndgame * countBits( bitKnightAttacks[square] & occupancies[2] );
                    break;

                // For now we dont assign any multipliers to quees
                case (Queen):
                    mgMobility[White] += QueenMobOpening * countBits( getQueenAttacks(square, occupancies[2]) );
                    egMobility[White] += QueenMobEndgame * countBits( getQueenAttacks(square, occupancies[2]) );
                    break;

                case (Queen + 6):
                    mgMobility[Black] += QueenMobOpening * countBits( getQueenAttacks(square, occupancies[2]) );
                    egMobility[Black] += QueenMobEndgame * countBits( getQueenAttacks(square, occupancies[2]) );
                    break;

                default:
                    break;
            }


            SET_BIT_FALSE(bitboardCopy, square);
        }

    }

    // std::cout << "mgScore white: " << mg[side] << " mgScore black: " << mg[side^1] << '\n';

    /* tapered eval */
    const int mgScore = mg[side] + mgMobility[side] - mg[side^1] - mgMobility[side^1];
    const int egScore = eg[side] + egMobility[side] - eg[side^1] - egMobility[side^1];
    int mgPhase = gamePhase;
    if (mgPhase > 24) mgPhase = 24; /* in case of early promotion */

    const int egPhase = 24 - mgPhase;

    return ((mgScore * mgPhase + egScore * egPhase) / 24) + penalties[side] - penalties[side^1];;
}


