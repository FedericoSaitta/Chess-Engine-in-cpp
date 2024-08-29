#include <iostream>
#include "../../include/macros.h"
#include "../../include/types.h"
#include "../../include/inline_functions.h"
#include "../../include/board.h"
 #include "evalparams.h"

#include <algorithm>
#include "../../include/misc.h"
#include <assert.h>

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

    // As these bitboards are used extensively
    const U64 whitePawns{ pos.bitboards[WHITE_PAWN] };
    const U64 blackPawns{ pos.bitboards[BLACK_PAWN] };
    const U64 allPieces { pos.bitboards[BOTH_OCC] };

    // for bishop pair bonus
    std::int8_t whiteBishops{ 0 };
    std::int8_t blackBishops{ 0 };

    for (int bbPiece=0; bbPiece < 12; bbPiece++) {

        assert( std::abs(score[WHITE]) < (std::pow(2, 30) ) && "evaluate: eval is too high");
        assert( std::abs(score[BLACK]) < (std::pow(2, 30) ) && "evaluate: eval is too high");

        U64 bitboardCopy = pos.bitboards[bbPiece];

        // defo could improve the conditional branching in here
        while (bitboardCopy) {
            const int square = pop_lsb(&bitboardCopy);
            score[bbPiece / 6] += eval_table[bbPiece][square];

            gamePhase += gamephaseInc[bbPiece];

            // Note that some of these masks do not consider if the pawns are in front or behind the pieces
            switch(bbPiece) {
                case (PAWN):
                    // you could try and avoid conditional branching here
                    if ( countBits(whitePawns & fileMasks[square]) > 1) score[WHITE] += doublePawnPenalty * countBits(whitePawns & fileMasks[square]);

                    // adding penalties to isolated pawns
                    if ( (whitePawns & isolatedPawnMasks[square] ) == 0) score[WHITE] += isolatedPawnPenalty;

                    // adding bonuses to passed pawns
                    if ( (blackPawns & white_passedPawnMasks[square] ) == 0) score[WHITE] += passedPawnBonus[getRankFromSquare[square]];

                    score[WHITE] += countBits( whitePawns & bitPawnAttacks[WHITE][square] ) * protectedPawnBonus;

                    break;

                case (BLACK_PAWN):
                    // you could try and avoid conditional branching here
                    if ( countBits(blackPawns & fileMasks[square]) > 1) score[BLACK] += doublePawnPenalty * countBits(blackPawns & fileMasks[square]);

                    // adding penalties to isolated pawns
                    if ( (blackPawns & isolatedPawnMasks[square] ) == 0) score[BLACK] += isolatedPawnPenalty;

                    // adding bonuses to passed pawns
                    if ( (whitePawns & black_passedPawnMasks[square] ) == 0) score[BLACK] += passedPawnBonus[7 - getRankFromSquare[square]];

                    score[BLACK] += countBits( blackPawns & bitPawnAttacks[BLACK][square] ) * protectedPawnBonus;

                    break;

                case (ROOK):
                    if ( (whitePawns & fileMasks[square]) == 0) score[WHITE] += semiOpenFileScore;
                    if ( ( (whitePawns | blackPawns) & fileMasks[square]) == 0) score[WHITE] += openFileScore;

                    score[WHITE] += RookMobility * countBits( getRookAttacks(square, allPieces) );
                    break;

                case (BLACK_ROOK):
                    if ( (blackPawns & fileMasks[square]) == 0) score[BLACK] += semiOpenFileScore;
                    if ( ( (whitePawns | blackPawns) & fileMasks[square]) == 0) score[BLACK] += openFileScore;

                    score[BLACK] += RookMobility * countBits( getRookAttacks(square, allPieces) );
                    break;

                // if the kings are on semi-open or open files they will be given penalties
                case (KING):
                    if ( (whitePawns & fileMasks[square]) == 0) score[WHITE] += kingSemiOpenFileScore;
                    if ( ( (whitePawns | blackPawns) & fileMasks[square]) == 0) score[WHITE] += kingOpenFileScore;

                    score[WHITE] += kingShieldBonus * countBits( bitKingAttacks[square] & pos.bitboards[WHITE_OCC] );
                    break;

                case (BLACK_KING):
                    if ( (blackPawns & fileMasks[square]) == 0) score[BLACK] += kingSemiOpenFileScore;
                    if ( ( (whitePawns | blackPawns) & fileMasks[square]) == 0) score[BLACK] += kingOpenFileScore;

                    score[BLACK] += kingShieldBonus * countBits( bitKingAttacks[square] & pos.bitboards[BLACK_OCC] );
                    break;

                case (BISHOP):
                    whiteBishops += 1;
                    score[WHITE] += BishopMobility * countBits( getBishopAttacks(square, allPieces) );
                    break;

                case (BLACK_BISHOP):
                    blackBishops += 1;
                    score[BLACK] += BishopMobility * countBits( getBishopAttacks(square, allPieces) );
                    break;

                case (KNIGHT || BLACK_KNIGHT):
                    score[WHITE] += KnightMobility * countBits( bitKnightAttacks[square] & allPieces );
                    break;

                case (BLACK_KNIGHT):
                    score[BLACK] += KnightMobility * countBits( bitKnightAttacks[square] & allPieces );
                    break;

                case (QUEEN):
                    score[WHITE] += QueenMobility * countBits( getQueenAttacks(square, allPieces) );
                    break;

                case (BLACK_QUEEN):
                    score[BLACK] += QueenMobility * countBits( getQueenAttacks(square, allPieces) );
                    break;

                default:
                    break;
            }
        }
    }

    // adding bonuses to phalanx pawns
    score[WHITE] += countBits( whitePawns & (shiftLeft(whitePawns) | shiftRight(whitePawns) ) ) * phalanxPawnBonus;
    score[BLACK] += countBits( blackPawns & (shiftLeft(blackPawns) | shiftRight(blackPawns) ) ) * phalanxPawnBonus;

    // applying bishop pair bonus:
    if (whiteBishops > 1) score[WHITE] += bishopPairBonus;
    if (blackBishops > 1) score[BLACK] += bishopPairBonus;

    const int mgScore = MgScore(score[pos.side]) - MgScore(score[pos.side^1]);
    const int egScore = EgScore(score[pos.side]) - EgScore(score[pos.side^1]);

    const int mgPhase = std::min(gamePhase, 24); // in case of early promotion
    const int egPhase = 24 - mgPhase;

    return (mgScore * mgPhase + egScore * egPhase) / 24;
}



