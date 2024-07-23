//
// Created by Federico Saitta on 28/06/2024.
// deals with uupdating the board after a move is made, also has undo move function
#include "update.h"

#include "../../include/macros.h"
#include "types.h"
#include "board.h"

#include "../../include/hashtable.h"


#include <cstring>
#include "../../include/inline_functions.h"
#include "../../include/misc.h"

// this is for little endian board
static const int castlingRightsConstant[64] = {
    13, 15, 15, 15,  12, 15, 15, 14,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    7,  15, 15, 15,  3, 15, 15, 11 };


// 0 represents illegal move, 1 represents the legal move;
int makeMove(const int move, const int onlyCaptures){

    // quiet moves
    if(!onlyCaptures) { // here set to 0
        COPY_BOARD();

        const int startSQ { getMoveStartSQ(move) };
        const int targetSQ { getMoveTargetSQ(move) };
        const int piece { getMovePiece(move) };
        const int promPiece { getMovePromPiece(move) };
        const int capture { getMoveCapture(move) };
        const int doublePush { getMoveDoublePush(move) };
        const int enPassant { getMoveEnPassant(move) };
        const int castling { getMoveCastling(move) };

        // moving the piece
        SET_BIT_FALSE(board.bitboards[piece], startSQ);
        SET_BIT(board.bitboards[piece], targetSQ);

        // hashing the piece
        hashKey ^= randomPieceKeys[piece][startSQ];
        hashKey ^= randomPieceKeys[piece][targetSQ];

        if ( capture ) {

            const int startPiece { PAWN + 6 * (board.side^1) };
            const int endPiece { KING + 6 * (board.side^1) };;

            for (int bbPiece=startPiece; bbPiece <= endPiece; bbPiece++) {
                if ( GET_BIT(board.bitboards[bbPiece], targetSQ) ) {
                    // remove piece from bitboard
                    SET_BIT_FALSE(board.bitboards[bbPiece], targetSQ);

                    // remove piece from hashkey
                    hashKey ^= randomPieceKeys[bbPiece][targetSQ];

                    break;
                }
            }
        }

        if ( promPiece ) {
            SET_BIT_FALSE( board.bitboards[piece], targetSQ);
            SET_BIT( board.bitboards[promPiece], targetSQ);

            // hash the removal of the PAWN and add the new piece
            hashKey ^= randomPieceKeys[piece][targetSQ]; // removal of the PAWN
            hashKey ^= randomPieceKeys[promPiece][targetSQ]; // addition of promoted piece
        }

        if ( enPassant ) {
            if (board.side == WHITE) {
                SET_BIT_FALSE( board.bitboards[PAWN + 6], targetSQ - 8 );

                // hash the removal of the opponent's PAWN
                hashKey ^= randomPieceKeys[PAWN + 6][targetSQ - 8];

            } else {
                SET_BIT_FALSE( board.bitboards[PAWN], targetSQ + 8 );
                hashKey ^= randomPieceKeys[PAWN][targetSQ + 8];
            }
        }

        // hash en-passant square
        if (board.enPassantSq != 64) { hashKey ^= randomEnPassantKeys[board.enPassantSq]; }

        // need to reset enPassant square
        board.enPassantSq = 64;

        if ( doublePush ) {
            if (board.side == WHITE) {
                board.enPassantSq = targetSQ - 8;
                // update hashKey
                hashKey ^= randomEnPassantKeys[targetSQ - 8];
            } else {
                board.enPassantSq = targetSQ + 8;
                hashKey ^= randomEnPassantKeys[targetSQ + 8];
            }
        }

        // here you should set castling to false
        if ( castling ) {
            switch(targetSQ) {
                case (G1):
                    SET_BIT_FALSE(board.bitboards[ROOK], H1);
                    SET_BIT(board.bitboards[ROOK], F1);

                    // hash ROOK:
                    hashKey ^= randomPieceKeys[ROOK][H1];
                    hashKey ^= randomPieceKeys[ROOK][F1];
                    break;

                case (C1):
                    SET_BIT_FALSE(board.bitboards[ROOK], A1);
                    SET_BIT(board.bitboards[ROOK], D1);
                    // hash ROOK:
                    hashKey ^= randomPieceKeys[ROOK][A1];
                    hashKey ^= randomPieceKeys[ROOK][D1];
                    break;

                case (G8):
                    SET_BIT_FALSE(board.bitboards[BLACK_ROOK], H8);
                    SET_BIT(board.bitboards[BLACK_ROOK], F8);
                    // hash ROOK:
                    hashKey ^= randomPieceKeys[BLACK_ROOK][H8];
                    hashKey ^= randomPieceKeys[BLACK_ROOK][F8];
                    break;
                case (C8):
                    SET_BIT_FALSE(board.bitboards[BLACK_ROOK], A8);
                    SET_BIT(board.bitboards[BLACK_ROOK], D8);
                    // hash ROOK:
                    hashKey ^= randomPieceKeys[BLACK_ROOK][A8];
                    hashKey ^= randomPieceKeys[BLACK_ROOK][D8];
                    break;

                default: break;
            }

        }
        // hash castling
        hashKey ^= randomCastlingKeys[board.castle]; // get trid of castling

        // board.castle bit hack for updating the rights
        board.castle &= castlingRightsConstant[startSQ];
        board.castle &= castlingRightsConstant[targetSQ];

        hashKey ^= randomCastlingKeys[board.castle]; // re-insert castling rights

        board.resetOcc();
        for (int bbPiece=0; bbPiece < 6; bbPiece++) {
            board.bitboards[WHITE_OCC] |= board.bitboards[bbPiece]; // for white
            board.bitboards[BLACK_OCC] |= board.bitboards[bbPiece + 6]; // for black
            board.bitboards[BOTH_OCC] |= (board.bitboards[bbPiece] | board.bitboards[bbPiece + 6]); // for both
        }

        board.side ^= 1; // change board.side
        hashKey ^= sideKey;

        /* FOR ZOBRITST DEBUGGING PURPOSES
        U64 new_hasKey = generateHashKey();
        // if this doesnt match we output an error
        if (new_hasKey != hashKey) {
            printMove(move);
            printBoardFancy();
            std::cout << "This does not match, true hash is: " << new_hasKey << '\n';
        }
        */


        // make sure that the king has not been exposed into check
        if ( isSqAttacked(bsf( board.bitboards[KING + 6 * (board.side^1)] ), board.side)) {
            // square is illegal so we take it back
            RESTORE_BOARD();
            return 0;
        }
        return 1; // this is a legal move so we return true
    }

    // used in Quiescent search
    if ( getMoveCapture(move) ) {
        return makeMove(move, 0); // make the move
        // forgetting this return statement causes issues within the quiescence search
    }
    return 0;
}
