#pragma once


#include <iostream>
#include <cstring>
#include "macros.h"
#include "types.h"
#include "../src/movegen/movegen.h"
#include "inline_functions.h"

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

    // constructors
    Board() { resetBoard(); }
    explicit Board(const std::string& fenString) { parseFEN(fenString); }

    void parseFEN(const std::string& fenString);

    U64 getBitboard(const Piece pc) const { return bitboards[pc]; }
    U64 getBitboard(const Occupancies occ)  const { return bitboards[occ]; }

    U64 getBitboard(const PieceType pc, const Color c) const { return bitboards[pc + 6 * c]; }
    U64 getPieceTypeBitBoard(const int pc) const { return bitboards[pc] | bitboards[pc+6]; }

    void resetBoard() {
        for (int i = 0; i < 512; i ++ )history[i].resetUndoInfo();
        memset(bitboards, 0ULL, sizeof(bitboards));

        for (int i=0; i<64; i++) mailbox[i] = NO_PIECE;

        gamePly = 0;
        side = WHITE;
    }

    void resetOcc() {
        // used to only reset the occupancies
        bitboards[WHITE_OCC] = 0ULL;
        bitboards[BLACK_OCC] = 0ULL;
        bitboards[BOTH_OCC] = 0ULL;
    }

    Piece getMovedPiece(const Move move) const {return mailbox[move.from()];}
    Piece getCapturedPiece(const Move move) const { return mailbox[move.to()]; }

    template <bool UpdateHash> void movePiece(int from, int to);
    template <bool UpdateHash> void movePieceQuiet(int from, int to);

    template <bool UpdateHash> void putPiece(Piece pc, int s);
    template <bool UpdateHash> void removePiece(int s);

    void undo(Move move);
    int makeMove(Move move, int onlyCaptures);

    void nullMove();
    void undoNullMove();

    bool currentlyInCheck() const; // true if the side to move is in check
    bool nonPawnMaterial() const; // true if the side to move has pieces other than pawns and a king

    // SHOULD TEST THESE TWO FUNCTIONS
    U64 allAttackers(int square, U64 occupancy) const;

    void generateMoves(MoveList& moveList) const;
    
    int isSqAttacked(const int square, const int turn) const {

        // we use side^1 to look at the opponent's attacks and + 6 * side to look at our own pieces, side = 1 for black
        // attacked by pawns
        if ( (bitPawnAttacks[turn^1][square] & bitboards[PAWN + 6 * turn]) ) return 1;

        // attacked by knight
        if ( bitKnightAttacks[square] & bitboards[KNIGHT + 6 * turn] ) return 1;

        // attacked by bishop
        if ( (getBishopAttacks(square, bitboards[BOTH_OCC]) & bitboards[BISHOP + 6 * turn] ) ) return 1;

        // attacked by rook
        if ( (getRookAttacks(square, bitboards[BOTH_OCC]) & bitboards[ROOK + 6 * turn] ) ) return 1;

        // attacked by queen
        if ( (getQueenAttacks(square, bitboards[BOTH_OCC]) & bitboards[QUEEN + 6 * turn] ) ) return 1;

        // attacked by king
        if ( bitKingAttacks[square] & bitboards[KING + 6 * turn] ) return 1;

        return 0;
    }

    void printBoardFancy() const;
};