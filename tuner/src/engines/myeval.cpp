#include "myeval.h"


#include <iostream>
#include "board.h"
#include "init.h"
#include "bit_operations.h"

#include "../engine/eval/evalparams.h"
#include "inline_functions.h"
#include <array>

#include <cmath>

#include <sstream>

#define TAPERED 1

using namespace myEval;

#define FLIP(sq) ((sq)^56)

#define TraceIncr(parameter, color) trace.parameter[color]++
#define TraceAdd(parameter, count, color) trace.parameter[color] += count
using namespace std;


#define u64 std::uint64_t

struct Trace
{
    int score;

    int KnightMobility[2]{};
    int BishopMobility[2]{};
    int RookMobility[2]{};
    int QueenMobility[2]{};
    int bishopPairBonus[2]{};

    int piece_value[6][2]{};

    int pawn_table[64][2]{};
    int knight_table[64][2]{};
    int bishop_table[64][2]{};
    int rook_table[64][2]{};
    int queen_table[64][2]{};
    int king_table[64][2]{};

    int doublePawnPenalty[2]{};
    int isolatedPawnPenalty[2]{};
    int phalanxPawnBonus[2]{};
    int protectedPawnBonus[2]{};

    int passedPawnBonus[8][2]{};


    int semiOpenFileScore[2]{};
    int openFileScore[2]{};

    int kingSemiOpenFileScore[2]{};
    int kingOpenFileScore[2]{};
    int kingShieldBonus[2]{};
};



[[nodiscard]] static u64 flip(u64 bb) {
    u64 result;
    char* bb_ptr = reinterpret_cast<char*>(&bb);
    char* result_ptr = reinterpret_cast<char*>(&result);
    for (int i = 0; i < sizeof(bb); i++)
    {
        result_ptr[i] = bb_ptr[sizeof(bb) - 1 - i];
    }
    return result;
}


static Trace eval(const std::string& fen) {
    const Board pos{fen};

    Trace trace{};
    uint32_t score[2]{};

    int gamePhase{};
    u64 bitboardCopy{}; // we need to ofc make a copy as we dont want to alter the bitboard

    for (int bbPiece=0; bbPiece < 12; bbPiece++) {
        bitboardCopy = pos.bitboards[bbPiece];

        // defo could improve the conditional branching in here
        while (bitboardCopy) {
            int square = popLSB(&bitboardCopy);

            // Incrementing the piece value, we want king value to stay at 0
            // to keep the kings score at zero
            if ( (bbPiece%6) != KING ) {
                score[bbPiece / 6] += piece_value[bbPiece%6];
                TraceIncr(piece_value[bbPiece%6], bbPiece/6);
            }


            // Incrementing the square values
            switch (bbPiece%6) {
                case (PAWN):
                    if ((bbPiece / 6) == 0) {
                        score[bbPiece / 6] += pawn_table[FLIP(square)];
                        TraceIncr(pawn_table[FLIP(square)], WHITE);
                    } else {
                        score[bbPiece / 6] += pawn_table[square];
                        TraceIncr(pawn_table[square], BLACK);
                    }
                break;

                case(KNIGHT):
                    if ((bbPiece / 6) == 0) {
                        score[bbPiece / 6] += knight_table[FLIP(square)];
                        TraceIncr(knight_table[FLIP(square)], WHITE);
                    } else {
                        score[bbPiece / 6] += knight_table[square];
                        TraceIncr(knight_table[square], BLACK);
                    }
                break;

                case(BISHOP):
                    if ((bbPiece / 6) == 0) {
                        score[bbPiece / 6] += bishop_table[FLIP(square)];
                        TraceIncr(bishop_table[FLIP(square)], WHITE);
                    } else {
                        score[bbPiece / 6] += bishop_table[square];
                        TraceIncr(bishop_table[square], BLACK);
                    }
                break;

                case(ROOK):
                    if ((bbPiece / 6) == 0) {
                        score[bbPiece / 6] += rook_table[FLIP(square)];
                        TraceIncr(rook_table[FLIP(square)], WHITE);
                    } else {
                        score[bbPiece / 6] += rook_table[square];
                        TraceIncr(rook_table[square], BLACK);
                    }
                break;

                case(QUEEN):
                    if ((bbPiece / 6) == 0) {
                        score[bbPiece / 6] += queen_table[FLIP(square)];
                        TraceIncr(queen_table[FLIP(square)], WHITE);
                    } else {
                        score[bbPiece / 6] += queen_table[square];
                        TraceIncr(queen_table[square], BLACK);
                    }
                break;

                case(KING):
                    if ((bbPiece / 6) == 0) {
                        score[bbPiece / 6] += king_table[FLIP(square)];
                        TraceIncr(king_table[FLIP(square)], WHITE);
                    } else {
                        score[bbPiece / 6] += king_table[square];
                        TraceIncr(king_table[square], BLACK);
                    }
                break;

                default:
                    break;
            }


            gamePhase += gamephaseInc[bbPiece];


            // Note that some of these masks do not consider if the pawns are in front or behind the pieces
            switch(bbPiece) {
                case (PAWN):
                // you could try and avoid conditional branching here
                    if ( countBits(pos.bitboards[PAWN] & fileMasks[square]) > 1) {
                        score[WHITE] += doublePawnPenalty * countBits(pos.bitboards[PAWN] & fileMasks[square]);
                        TraceAdd(doublePawnPenalty, countBits(pos.bitboards[PAWN] & fileMasks[square]), WHITE);
                    }

                // adding score to isolated pawns
                if ( (pos.bitboards[PAWN] & isolatedPawnMasks[square] ) == 0) {
                    score[WHITE] += isolatedPawnPenalty;
                    TraceIncr(isolatedPawnPenalty, WHITE);
                }

                // adding bonuses to passed pawns
                if ( (pos.bitboards[BLACK_PAWN] & white_passedPawnMasks[square] ) == 0) {
                    score[WHITE] += passedPawnBonus[getRankFromSquare[square]];
                    TraceIncr(passedPawnBonus[getRankFromSquare[square]], WHITE);
                }

                score[WHITE] += countBits( pos.bitboards[PAWN] & bitPawnAttacks[WHITE][square] ) * protectedPawnBonus;
                TraceAdd(protectedPawnBonus, countBits( pos.bitboards[PAWN] & bitPawnAttacks[WHITE][square] ), WHITE);

                break;

                case (BLACK_PAWN):
                    // you could try and avoid conditional branching here
                        if ( countBits(pos.bitboards[BLACK_PAWN] & fileMasks[square]) > 1) {
                            score[BLACK] += doublePawnPenalty * countBits(pos.bitboards[BLACK_PAWN] & fileMasks[square]);
                            TraceAdd(doublePawnPenalty, countBits(pos.bitboards[BLACK_PAWN] & fileMasks[square]), BLACK);
                        }

                // adding score to isolated pawns
                if ( (pos.bitboards[BLACK_PAWN] & isolatedPawnMasks[square] ) == 0) {
                    score[BLACK] += isolatedPawnPenalty;
                    TraceIncr(isolatedPawnPenalty, BLACK);
                }

                // adding bonuses to passed pawns
                if ( (pos.bitboards[PAWN] & black_passedPawnMasks[square] ) == 0) {
                    score[BLACK] += passedPawnBonus[7 - getRankFromSquare[square]];
                    TraceIncr(passedPawnBonus[7 - getRankFromSquare[square]], BLACK);
                }


                score[BLACK] += countBits( pos.bitboards[BLACK_PAWN] & bitPawnAttacks[BLACK][square] ) * protectedPawnBonus;
                TraceAdd(protectedPawnBonus, countBits( pos.bitboards[BLACK_PAWN] & bitPawnAttacks[BLACK][square] ), BLACK);


                break;

                case (ROOK):
                    if ( (pos.bitboards[PAWN] & fileMasks[square]) == 0) {
                        score[WHITE] += semiOpenFileScore;
                        TraceIncr(semiOpenFileScore, WHITE);
                    }
                    if ( ( (pos.bitboards[PAWN] | pos.bitboards[BLACK_PAWN]) & fileMasks[square]) == 0) {
                        score[WHITE] += openFileScore;
                        TraceIncr(openFileScore, WHITE);
                    }

                    score[WHITE] += RookMobility * countBits( getRookAttacks(square, pos.bitboards[BOTH_OCC]) );
                    TraceAdd(RookMobility, countBits( getRookAttacks(square, pos.bitboards[BOTH_OCC]) ), WHITE);
                    break;

                case (BLACK_ROOK):

                    if ( (pos.bitboards[BLACK_PAWN] & fileMasks[square]) == 0) {
                        score[BLACK] += semiOpenFileScore;
                        TraceIncr(semiOpenFileScore, BLACK);
                    }


                    if ( ( (pos.bitboards[PAWN] | pos.bitboards[BLACK_PAWN]) & fileMasks[square]) == 0) {
                        score[BLACK] += openFileScore;
                        TraceIncr(openFileScore, BLACK);
                    }

                    score[BLACK] += RookMobility * countBits( getRookAttacks(square, pos.bitboards[BOTH_OCC] ) );
                    TraceAdd(RookMobility, countBits( getRookAttacks(square, pos.bitboards[BOTH_OCC]) ), BLACK);
                    break;

                // if the kings are on semi-open or open files they will be given score
                case (KING):
                    if ( (pos.bitboards[PAWN] & fileMasks[square]) == 0) {
                        score[WHITE] += kingSemiOpenFileScore;
                        TraceIncr(kingSemiOpenFileScore, WHITE);
                    }

                if ( ( (pos.bitboards[PAWN] | pos.bitboards[BLACK_PAWN]) & fileMasks[square]) == 0) {
                    score[WHITE] += kingOpenFileScore;
                    TraceIncr(kingOpenFileScore, WHITE);
                }

                TraceAdd(kingShieldBonus, countBits( bitKingAttacks[square] & pos.bitboards[WHITE_OCC]), WHITE);
                score[WHITE] += kingShieldBonus * countBits( bitKingAttacks[square] & pos.bitboards[WHITE_OCC] );
                break;

                case (BLACK_KING):
                    if ( (pos.bitboards[BLACK_PAWN] & fileMasks[square]) == 0) {
                        score[BLACK] += kingSemiOpenFileScore;
                        TraceIncr(kingSemiOpenFileScore, BLACK);
                    }
                    if ( ( (pos.bitboards[PAWN] | pos.bitboards[BLACK_PAWN]) & fileMasks[square]) == 0) {
                        score[BLACK] += kingOpenFileScore;
                        TraceIncr(kingOpenFileScore, BLACK);
                    }

                    TraceAdd(kingShieldBonus, countBits( bitKingAttacks[square] & pos.bitboards[BLACK_OCC] ), BLACK);
                    score[BLACK] += kingShieldBonus * countBits( bitKingAttacks[square] & pos.bitboards[BLACK_OCC] );
                    break;

                // mobility scores for sliding pieces except rooks, please test these and stop adding new features
                // these are very basic implementations, like the ones in fruit
                case (BISHOP):
                    score[WHITE] += BishopMobility * countBits( getBishopAttacks(square, pos.bitboards[BOTH_OCC]) );
                    TraceAdd(BishopMobility, countBits( getBishopAttacks(square, pos.bitboards[BOTH_OCC]) ), WHITE);
                    break;

                case (BLACK_BISHOP):
                    TraceAdd(BishopMobility, countBits( getBishopAttacks(square, pos.bitboards[BOTH_OCC]) ), BLACK);
                    score[BLACK] += BishopMobility * countBits( getBishopAttacks(square, pos.bitboards[BOTH_OCC]) );
                    break;

                case (KNIGHT):
                    TraceAdd(KnightMobility, countBits( bitKnightAttacks[square] & pos.bitboards[BOTH_OCC] ), WHITE);
                    score[WHITE] += KnightMobility * countBits( bitKnightAttacks[square] & pos.bitboards[BOTH_OCC] );
                    break;

                case (BLACK_KNIGHT):
                    TraceAdd(KnightMobility, countBits( bitKnightAttacks[square] & pos.bitboards[BOTH_OCC] ), BLACK);
                    score[BLACK] += KnightMobility * countBits( bitKnightAttacks[square] & pos.bitboards[BOTH_OCC] );
                    break;

                case (QUEEN):
                    TraceAdd(QueenMobility, countBits( getQueenAttacks(square, pos.bitboards[BOTH_OCC]) ), WHITE);
                    score[WHITE] += QueenMobility * countBits( getQueenAttacks(square, pos.bitboards[BOTH_OCC]) );
                    break;

                case (BLACK_QUEEN):
                    TraceAdd(QueenMobility, countBits( getQueenAttacks(square, pos.bitboards[BOTH_OCC]) ), BLACK);
                    score[BLACK] += QueenMobility * countBits( getQueenAttacks(square, pos.bitboards[BOTH_OCC]) );
                    break;

                default:
                    break;
            }

        }
    }

    score[WHITE] += countBits( pos.bitboards[PAWN] & (shiftLeft(pos.bitboards[PAWN]) | shiftRight(pos.bitboards[PAWN]) ) ) * phalanxPawnBonus;
    TraceAdd(phalanxPawnBonus, countBits( pos.bitboards[PAWN] & (shiftLeft(pos.bitboards[PAWN]) | shiftRight(pos.bitboards[PAWN]) ) ), WHITE);

    score[BLACK] +=  countBits( pos.bitboards[BLACK_PAWN] & (shiftLeft(pos.bitboards[BLACK_PAWN]) | shiftRight(pos.bitboards[BLACK_PAWN]) ) ) * phalanxPawnBonus;
    TraceAdd(phalanxPawnBonus,  countBits( pos.bitboards[BLACK_PAWN] & (shiftLeft(pos.bitboards[BLACK_PAWN]) | shiftRight(pos.bitboards[BLACK_PAWN]) ) ), BLACK);


    // applying bishop pair bonus:
    if (countBits(pos.bitboards[WHITE_BISHOP]) > 1) {
        score[WHITE] += bishopPairBonus;
        TraceIncr(bishopPairBonus, WHITE);
    }
    if (countBits(pos.bitboards[BLACK_BISHOP]) > 1) {
        score[BLACK] += bishopPairBonus;
        TraceIncr(bishopPairBonus, BLACK);
    }

    const int mgScore = MgScore(score[pos.side]) - MgScore(score[pos.side^1]);
    const int egScore = EgScore(score[pos.side]) - EgScore(score[pos.side^1]);

    int mgPhase = gamePhase;
    if (mgPhase > 24) mgPhase = 24;  // in case of early promotion

    const int egPhase = 24 - mgPhase;
    const int finalScore = ((mgScore * mgPhase + egScore * egPhase) / 24);
   // trace.score = finalScore;

    return trace;
}

static int32_t round_value(tune_t value)
{
    return static_cast<int32_t>(round(value));
}
static void print_parameter(std::stringstream& ss, const pair_t parameter)
{
    const auto mg = round_value(parameter[static_cast<int32_t>(PhaseStages::Midgame)]);
    const auto eg = round_value(parameter[static_cast<int32_t>(PhaseStages::Endgame)]);
    if (mg == 0 && eg == 0)
    {
        ss << 0;
    }
    else
    {
        ss << "S(" << mg << ", " << eg << ")";
    }
}
static void print_single(std::stringstream& ss, const parameters_t& parameters, int& index, const std::string& name)
{
    ss << "constexpr int " << name << " = ";
    print_parameter(ss, parameters[index]);
    index++;

    ss << ";" << std::endl;
}
static void print_array(std::stringstream& ss, const parameters_t& parameters, int& index, const std::string& name, const int count)
{
    ss << "constexpr int " << name << "[] = {";
    for (auto i = 0; i < count; i++)
    {
        print_parameter(ss, parameters[index]);
        index++;

        if (i != count - 1)
        {
            ss << ", ";
        }
    }
    ss << "};" << std::endl;
}
static void print_pst(std::stringstream& ss, const parameters_t& parameters, int& index, const std::string& name)
{
    ss << "constexpr int " << name << "[] = {";
    for (auto i = 0; i < 64; i++)
    {
        print_parameter(ss, parameters[index]);
        index++;

        ss << ", ";

        if (i % 8 == 7)
        {
            ss << "\n";
        }
    }
    ss << "};" << std::endl;
}
static void print_array_2d(std::stringstream& ss, const parameters_t& parameters, int& index, const std::string& name, const int count1, const int count2)
{
    ss << "const int " << name << "[][" << count2 << "] = {\n";
    for (auto i = 0; i < count1; i++)
    {
        ss << "    {";
        for (auto j = 0; j < count2; j++)
        {
            print_parameter(ss, parameters[index]);
            index++;

            if (j != count2 - 1)
            {
                ss << ", ";
            }
        }
        ss << "},\n";
    }
    ss << "};\n";
}


parameters_t myEvaluation::get_initial_parameters()
{
    parameters_t parameters;

    get_initial_parameter_single(parameters, KnightMobility);
    get_initial_parameter_single(parameters, BishopMobility);
    get_initial_parameter_single(parameters, RookMobility);
    get_initial_parameter_single(parameters, QueenMobility);
    get_initial_parameter_single(parameters, bishopPairBonus);

    get_initial_parameter_array(parameters, piece_value, 6);

    get_initial_parameter_array(parameters, pawn_table, 64);
    get_initial_parameter_array(parameters, knight_table, 64);
    get_initial_parameter_array(parameters, bishop_table, 64);
    get_initial_parameter_array(parameters, rook_table, 64);
    get_initial_parameter_array(parameters, queen_table, 64);
    get_initial_parameter_array(parameters, king_table, 64);

    get_initial_parameter_single(parameters, doublePawnPenalty);
    get_initial_parameter_single(parameters, isolatedPawnPenalty);

    get_initial_parameter_single(parameters, phalanxPawnBonus);
    get_initial_parameter_single(parameters, protectedPawnBonus);

    get_initial_parameter_array(parameters, passedPawnBonus, 8);

    get_initial_parameter_single(parameters, semiOpenFileScore);
    get_initial_parameter_single(parameters, openFileScore);

    get_initial_parameter_single(parameters, kingSemiOpenFileScore);
    get_initial_parameter_single(parameters, kingOpenFileScore);
    get_initial_parameter_single(parameters, kingShieldBonus);

    return parameters;
}

static coefficients_t get_coefficients(const Trace& trace)
{
    coefficients_t coefficients;

    get_coefficient_single(coefficients, trace.KnightMobility);
    get_coefficient_single(coefficients, trace.BishopMobility);
    get_coefficient_single(coefficients, trace.RookMobility);
    get_coefficient_single(coefficients, trace.QueenMobility);
    get_coefficient_single(coefficients, trace.bishopPairBonus);

    get_coefficient_array(coefficients, trace.piece_value, 6);

    get_coefficient_array(coefficients, trace.pawn_table, 64);
    get_coefficient_array(coefficients, trace.knight_table, 64);
    get_coefficient_array(coefficients, trace.bishop_table, 64);
    get_coefficient_array(coefficients, trace.rook_table, 64);
    get_coefficient_array(coefficients, trace.queen_table, 64);
    get_coefficient_array(coefficients, trace.king_table, 64);

    get_coefficient_single(coefficients, trace.doublePawnPenalty);
    get_coefficient_single(coefficients, trace.isolatedPawnPenalty);
    get_coefficient_single(coefficients, trace.phalanxPawnBonus);
    get_coefficient_single(coefficients, trace.protectedPawnBonus);

    get_coefficient_array(coefficients, trace.passedPawnBonus, 8);

    get_coefficient_single(coefficients, trace.semiOpenFileScore);
    get_coefficient_single(coefficients, trace.openFileScore);

    get_coefficient_single(coefficients, trace.kingSemiOpenFileScore);
    get_coefficient_single(coefficients, trace.kingOpenFileScore);
    get_coefficient_single(coefficients, trace.kingShieldBonus);

    return coefficients;
}

void myEvaluation::print_parameters(const parameters_t& parameters)
{
    parameters_t parameters_copy = parameters;

    int index = 0;
    std::stringstream ss;

    print_single(ss, parameters_copy, index, "KnightMobility");
    print_single(ss, parameters_copy, index, "BishopMobility");
    print_single(ss, parameters_copy, index, "RookMobility");
    print_single(ss, parameters_copy, index, "QueenMobility");
    print_single(ss, parameters_copy, index, "bishopPairBonus");

    print_array(ss, parameters_copy, index, "piece_value", 6);

    print_pst(ss, parameters_copy, index, "pawn_table");
    print_pst(ss, parameters_copy, index, "knight_table");
    print_pst(ss, parameters_copy, index, "bishop_table");
    print_pst(ss, parameters_copy, index, "rook_table");
    print_pst(ss, parameters_copy, index, "queen_table");
    print_pst(ss, parameters_copy, index, "king_table");

    print_single(ss, parameters_copy, index, "doublePawnPenalty");
    print_single(ss, parameters_copy, index, "isolatedPawnPenalty");

    print_single(ss, parameters_copy, index, "phalanxPawnBonus");
    print_single(ss, parameters_copy, index, "protectedPawnBonus");

    print_array(ss, parameters_copy, index, "passedPawnBonus", 8);

    print_single(ss, parameters_copy, index, "semiOpenFileScore");
    print_single(ss, parameters_copy, index, "openFileScore");

    print_single(ss, parameters_copy, index, "kingSemiOpenFileScore");
    print_single(ss, parameters_copy, index, "kingOpenFileScore");
    print_single(ss, parameters_copy, index, "kingShieldBonus");

    std::cout << ss.str() << "\n";
}

EvalResult myEvaluation::get_fen_eval_result(const std::string& fen)
{
    const auto trace = eval(fen);
    EvalResult result;

    result.coefficients = get_coefficients(trace);
    result.score = 0;
    
    return result;
}

EvalResult myEvaluation::get_external_eval_result(const chess::Board& board)
{
    throw std::runtime_error("Not implemented");
}