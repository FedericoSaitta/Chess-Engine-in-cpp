//
// Created by Federico Saitta on 10/07/2024.
//

#pragma once


#include <iostream>
#include "macros.h"
#include "types.h"

extern const char* chessBoard[65];

void parseFEN(const std::string& fenString);

#define COPY_HASH()             \
U64 hashKeyCopy = hashKey;

// restore board state
#define RESTORE_HASH()       \
hashKey = hashKeyCopy;

class Board {

public:
    UndoInfo history[512];
    U64 bitboards[15]; // 0-11: pieces, 12-14: occupancies

    Piece mailbox[64];

    int gamePly{};

    int side{ WHITE };


    inline U64 getBitboard(const Piece pc) const { return bitboards[pc]; }
    inline U64 getBitboard(const Occupancies occ)  const { return bitboards[occ]; }

    inline U64 getBitboard(const PieceType pc, const Color c) const { return bitboards[pc + 6 * c]; }

    inline void resetBoard() {
        memset(bitboards, 0ULL, sizeof(bitboards));
        history->resetUndoInfo();

        for (int i=0; i<64; i++) mailbox[i] = NO_PIECE;

        gamePly = 0;

        side = WHITE;
        history[gamePly].enPassSq = 64;
        history[gamePly].castle = 0ULL;
        history[gamePly].captured = NO_PIECE;

    }

    inline void resetOcc() {
        // used to only reset the occupancies
        bitboards[WHITE_OCC] = 0ULL;
        bitboards[BLACK_OCC] = 0ULL;
        bitboards[BOTH_OCC] = 0ULL;
    }


    inline Piece getMovedPiece(const Move move) const {return mailbox[move.from()];}
    inline Piece getCapturedPiece(const Move move) const { return mailbox[move.to()]; }

    template <bool UpdateHash> void movePiece(int from, int to);
    template <bool UpdateHash> void movePieceQuiet(int from, int to);

    template <bool UpdateHash> void putPiece(Piece pc, int s);
    template <bool UpdateHash> void removePiece(int s);

    void undo(Move move);
    int makeMove(Move move, int onlyCaptures);

    static void nullMove();
    static void undoNullMove();
};

extern Board board;