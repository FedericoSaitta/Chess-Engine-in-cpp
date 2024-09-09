// In this file all the pre-processor definitions are listed, these include board squares, pieces and states
// which are referred to as A1 or Bishop (for Bishop) instead of as ints.

// The square mapping of the bitboards is in little-Endian (A1: 0, ..., H8: 63) as seen below:

// Chess squares
// A8  B8  C8  D8  E8  F8  G8  H8
// A7  B7  C7  D7  E7  F7  G7  H7
// A6  B6  C6  D6  E6  F6  G6  H6
// A5  B5  C5  D5  E5  F5  G5  H5
// A4  B4  C4  D4  E4  F4  G4  H4
// A3  B3  C3  D3  E3  F3  G3  H3
// A2  B2  C2  D2  E2  F2  G2  H2
// A1  B1  C1  D1  E1  F1  G1  H1

// Respective Bitboard Indices
// 56  57  58  59  60  61  62  63
// 48  49  50  51  52  53  54  55
// 40  41  42  43  44  45  46  47
// 32  33  34  35  36  37  38  39
// 24  25  26  27  28  29  30  31
// 16  17  18  19  20  21  22  23
// 8   9   10  11  12  13  14  15
// 0   1   2   3   4   5   6   7

#pragma once

#include <cstdint>

#define U64 std::uint64_t

#define A1  0
#define B1  1
#define C1  2
#define D1  3
#define E1  4
#define F1  5
#define G1  6
#define H1  7

#define A2  8
#define B2  9
#define C2  10
#define D2  11
#define E2  12
#define F2  13
#define G2  14
#define H2  15

#define A3  16
#define B3  17
#define C3  18
#define D3  19
#define E3  20
#define F3  21
#define G3  22
#define H3  23

#define A4  24
#define B4  25
#define C4  26
#define D4  27
#define E4  28
#define F4  29
#define G4  30
#define H4  31

#define A5  32
#define B5  33
#define C5  34
#define D5  35
#define E5  36
#define F5  37
#define G5  38
#define H5  39

#define A6  40
#define B6  41
#define C6  42
#define D6  43
#define E6  44
#define F6  45
#define G6  46
#define H6  47

#define A7  48
#define B7  49
#define C7  50
#define D7  51
#define E7  52
#define F7  53
#define G7  54
#define H7  55

#define A8  56
#define B8  57
#define C8  58
#define D8  59
#define E8  60
#define F8  61
#define G8  62
#define H8  63

#define SET_BIT(board, square) ((board) |= (1ULL << (square)))
#define SET_BIT_FALSE(board, square) ((board) &= ~(1ULL << (square)))
#define GET_BIT(board, square) (((board) >> (square)) & 1ULL)