//
// Created by Federico Saitta on 10/07/2024.
//

#pragma once



#include "hashtable.h"
#include "board.h"
#include "init.h"
#include "inline_functions.h"
#include "macros.h"


struct MoveList {
    int moves[218];
    int count;
};

inline void addMove(MoveList& moveList, const int move) {
    moveList.moves[moveList.count] = move;
    moveList.count++;
}

void generateMoves(MoveList& moveList);


void generateLegalMoves(MoveList& moveList);

#define NO_PIECE 13
#define NO_SQUARE 64

struct UndoInfo {
    //The bitboard of squares on which pieces have either moved from, or have been moved to. Used for castling
    //legality checks
    U64 entry;

    //The piece that was captured on the last move
    int captured;

    //The en passant square. This is the square which pawns can move to in order to en passant capture an enemy pawn that has
    //double pushed on the previous move
    int epsq;

    constexpr UndoInfo() : entry(0), captured(NO_PIECE), epsq(NO_SQUARE) {}

    //This preserves the entry bitboard across moves
    UndoInfo(const UndoInfo& prev) :
        entry(prev.entry), captured(NO_PIECE), epsq(NO_SQUARE) {}
};

extern UndoInfo history[256];

//The bitboard of enemy pieces that are currently attacking the king, updated whenever generate_moves()
//is called
extern U64 checkers;

//The bitboard of pieces that are currently pinned to the king by enemy sliders, updated whenever
//generate_moves() is called
extern U64 pinned;