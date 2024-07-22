//
// Created by Federico Saitta on 18/07/2024.
//
#pragma once



#include "evaluation.h"

// these will be used once ill decide to improve upon eval function, one used by WEISS doesnt look bad at all
// note that WEISS is very strong about 3000... and still uses hand crafted eval so it is definetely possible :)
#define S(mg, eg) ((int)((unsigned int)(eg) << 16) + (mg))
#define MgScore(s) ((int16_t)((uint16_t)((unsigned)((s)))))
#define EgScore(s) ((int16_t)((uint16_t)((unsigned)((s) + 0x8000) >> 16)))

// [7788s] Epoch 2200 (0.309436 eps), error 0.119876, LR 1
constexpr int KnightMobility = S(-1, 2);
constexpr int BishopMobility = S(4, 11);
constexpr int RookMobility = S(2, 4);
constexpr int QueenMobility = S(2, 5);
constexpr int bishopPairBonus = S(14, 81);
constexpr int piece_value[] = {S(74, 122), S(281, 438), S(272, 398), S(356, 764), S(841, 1441), 0};
constexpr int pawn_table[] = {0, 0, 0, 0, 0, 0, 0, 0,
S(104, 169), S(85, 165), S(76, 151), S(118, 91), S(96, 96), S(108, 105), S(-81, 172), S(-73, 174),
S(-3, 78), S(12, 66), S(38, 38), S(24, 1), S(39, 1), S(86, 20), S(26, 42), S(-2, 46),
S(-8, 46), S(-1, 41), S(0, 28), S(20, 3), S(28, 10), S(31, 16), S(5, 30), S(-10, 28),
S(-11, 29), S(-10, 27), S(-3, 12), S(4, 10), S(10, 10), S(13, 17), S(-2, 19), S(-7, 14),
S(-12, 20), S(-17, 19), S(-6, 17), S(-2, 13), S(6, 25), S(14, 20), S(7, 15), S(0, 5),
S(-14, 28), S(-22, 21), S(-20, 26), S(-12, 22), S(-16, 33), S(16, 30), S(11, 17), S(-10, 0),
0, 0, 0, 0, 0, 0, 0, 0,
};
constexpr int knight_table[] = {S(-183, -58), S(-136, 36), S(-144, 74), S(-12, 39), S(20, 29), S(-116, 58), S(-41, 9), S(-126, -79),
S(-29, -7), S(-38, 31), S(4, 44), S(25, 77), S(32, 70), S(68, 20), S(-53, 37), S(-14, 7),
S(-49, 21), S(-2, 55), S(30, 76), S(39, 76), S(77, 58), S(95, 71), S(52, 46), S(30, 16),
S(-10, 27), S(1, 63), S(16, 85), S(25, 99), S(13, 101), S(37, 93), S(12, 79), S(18, 35),
S(-26, 20), S(-14, 59), S(2, 86), S(0, 91), S(10, 90), S(3, 86), S(22, 61), S(-11, 32),
S(-41, -16), S(-19, 34), S(-8, 44), S(2, 70), S(6, 64), S(-2, 43), S(-8, 37), S(-25, -8),
S(-60, 8), S(-45, 28), S(-25, 27), S(-14, 37), S(-16, 41), S(-16, 18), S(-44, 25), S(-29, 2),
S(-134, -38), S(-37, -42), S(-59, 16), S(-41, 18), S(-31, 18), S(-28, 1), S(-31, -19), S(-140, 7),
};
constexpr int bishop_table[] = {S(-72, 52), S(-67, 65), S(-114, 62), S(-93, 71), S(-109, 63), S(-117, 62), S(38, 36), S(-33, 75),
S(-65, 55), S(-32, 44), S(-36, 41), S(-34, 43), S(-24, 38), S(-8, 45), S(-33, 44), S(-20, 36),
S(-28, 44), S(-19, 49), S(18, 24), S(-7, 34), S(24, 25), S(22, 47), S(25, 46), S(-1, 52),
S(-38, 43), S(-4, 42), S(-19, 33), S(22, 23), S(-4, 34), S(5, 29), S(-9, 52), S(-13, 49),
S(-22, 29), S(-22, 36), S(-20, 39), S(-8, 30), S(3, 19), S(-27, 31), S(-23, 33), S(-6, 21),
S(-20, 28), S(-10, 31), S(-20, 28), S(-16, 30), S(-22, 30), S(-14, 17), S(-15, 21), S(-7, 36),
S(-4, 23), S(-11, 14), S(-7, 19), S(-24, 18), S(-22, 19), S(-17, 3), S(0, 18), S(-10, -9),
S(-6, 18), S(-11, 20), S(-24, 10), S(-35, 24), S(-40, 24), S(-28, 25), S(-20, 27), S(-17, 17),
};
constexpr int rook_table[] = {S(48, 70), S(27, 78), S(29, 76), S(26, 77), S(60, 67), S(102, 60), S(85, 68), S(86, 75),
S(2, 80), S(-11, 86), S(18, 81), S(37, 84), S(28, 89), S(78, 53), S(23, 67), S(55, 59),
S(-19, 72), S(12, 63), S(8, 74), S(19, 64), S(39, 55), S(67, 62), S(99, 39), S(45, 57),
S(-23, 63), S(-13, 62), S(-11, 69), S(9, 62), S(-4, 67), S(15, 59), S(19, 53), S(13, 57),
S(-39, 47), S(-43, 63), S(-41, 64), S(-35, 59), S(-35, 56), S(-18, 53), S(-6, 51), S(-7, 45),
S(-41, 26), S(-37, 37), S(-45, 38), S(-35, 38), S(-35, 36), S(-25, 35), S(6, 26), S(-17, 17),
S(-55, 29), S(-35, 23), S(-30, 29), S(-31, 26), S(-32, 30), S(-13, 20), S(-5, 10), S(-70, 36),
S(-21, 32), S(-23, 37), S(-17, 37), S(-11, 29), S(-12, 29), S(-14, 42), S(-3, 29), S(-14, 10),
};
constexpr int queen_table[] = {S(-22, 72), S(4, 78), S(0, 98), S(50, 65), S(38, 100), S(78, 97), S(92, 93), S(43, 98),
S(-29, 59), S(-67, 96), S(-37, 101), S(-62, 134), S(-38, 163), S(17, 146), S(-46, 164), S(16, 136),
S(-22, 25), S(-27, 52), S(-30, 70), S(-26, 88), S(-4, 122), S(54, 133), S(61, 114), S(20, 180),
S(-26, 20), S(-22, 44), S(-33, 54), S(-31, 80), S(-35, 123), S(-9, 130), S(5, 125), S(11, 126),
S(-14, 1), S(-18, 39), S(-17, 23), S(-25, 66), S(-27, 69), S(-12, 81), S(-2, 57), S(8, 76),
S(-16, -8), S(-6, 3), S(-11, 23), S(-14, 8), S(-15, 14), S(-12, 31), S(3, 9), S(-3, 28),
S(-13, -12), S(-8, -13), S(1, -36), S(-6, -8), S(0, -27), S(3, -57), S(9, -68), S(5, -38),
S(-1, -25), S(-5, -41), S(0, -49), S(5, -26), S(-3, -39), S(-34, -18), S(-5, -53), S(-7, -45),
};
constexpr int king_table[] = {S(-12, -166), S(61, -48), S(151, -48), S(54, -32), S(82, -63), S(24, -15), S(69, -35), S(20, -171),
S(-18, -45), S(51, 32), S(136, 8), S(82, -3), S(50, 12), S(32, 29), S(32, 49), S(-87, -24),
S(37, 8), S(71, 41), S(84, 38), S(-13, 39), S(31, 34), S(129, 37), S(79, 42), S(-4, 4),
S(-34, 14), S(41, 35), S(33, 50), S(-2, 60), S(-37, 58), S(45, 41), S(15, 36), S(-60, 13),
S(-15, -23), S(22, 16), S(39, 36), S(-44, 61), S(-11, 50), S(-25, 39), S(-6, 19), S(-74, -2),
S(-17, -19), S(-60, 19), S(-52, 33), S(-61, 47), S(-52, 40), S(-52, 25), S(-32, 5), S(-50, -13),
S(-7, -10), S(-23, 4), S(-53, 18), S(-101, 30), S(-78, 22), S(-77, 19), S(-25, -8), S(-1, -33),
S(-25, -66), S(13, -37), S(-24, -15), S(-104, -11), S(-39, -51), S(-105, -4), S(0, -38), S(11, -96),
};
constexpr int doublePawnPenalty = S(-3, -6);
constexpr int isolatedPawnPenalty = S(-11, -17);
constexpr int passedPawnBonus[] = {0, S(-9, 18), S(-15, 21), S(-7, 45), S(18, 66), S(35, 130), S(57, 72), S(200, 200)};
constexpr int semiOpenFileScore = S(15, 18);
constexpr int openFileScore = S(18, -7);
constexpr int kingSemiOpenFileScore = S(-27, 22);
constexpr int kingOpenFileScore = S(-16, -32);
constexpr int kingShieldBonus = S(6, 1);


inline const int* pesto_table[6] =
{
    pawn_table,
    knight_table,
    bishop_table,
    rook_table,
    queen_table,
    king_table
};

constexpr int gamephaseInc[12] = {0, 1, 1, 2, 4, 0, 0, 1, 1, 2, 4, 0,};

