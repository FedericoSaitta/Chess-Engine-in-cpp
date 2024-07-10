//
// Created by Federico Saitta on 28/06/2024.
// deals with uupdating the board after a move is made, also has undo move function

#include "update.h"

#include "board.h"
#include "hashtable.h"
#include "macros.h"

#include <cstring>
#include "inline_functions.h"
#include "misc.h"

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
        SET_BIT_FALSE(bitboards[piece], startSQ);
        SET_BIT(bitboards[piece], targetSQ);

        // hashing the piece
        hashKey ^= randomPieceKeys[piece][startSQ];
        hashKey ^= randomPieceKeys[piece][targetSQ];

        if ( capture ) {
            int startPiece, endPiece;

            if (side == White ) { startPiece = Pawn + 6; endPiece = King + 6; }
            else { startPiece = Pawn; endPiece = King; }

            for (int bbPiece=startPiece; bbPiece <= endPiece; bbPiece++) {
                if ( GET_BIT(bitboards[bbPiece], targetSQ) ) {
                    // remove piece from bitboard
                    SET_BIT_FALSE(bitboards[bbPiece], targetSQ);

                    // remove piece from hashkey
                    hashKey ^= randomPieceKeys[bbPiece][targetSQ];

                    break;
                }
            }
        }

        if ( promPiece ) {
            SET_BIT_FALSE( bitboards[piece], targetSQ);
            SET_BIT( bitboards[promPiece], targetSQ);

            // hash the removal of the pawn and add the new piece
            hashKey ^= randomPieceKeys[piece][targetSQ]; // removal of the pawn
            hashKey ^= randomPieceKeys[promPiece][targetSQ]; // addition of promoted piece
        }

        if ( enPassant ) {
            if (side == White) {
                SET_BIT_FALSE( bitboards[Pawn + 6], targetSQ - 8 );

                // hash the removal of the opponent's pawn
                hashKey ^= randomPieceKeys[Pawn + 6][targetSQ - 8];

            } else {
                SET_BIT_FALSE( bitboards[Pawn], targetSQ + 8 );
                hashKey ^= randomPieceKeys[Pawn][targetSQ + 8];
            }
        }

        // hash en-passant square
        if (enPassantSQ != 64) { hashKey ^= randomEnPassantKeys[enPassantSQ]; }

        // need to reset enPassant square
        enPassantSQ = 64;

        if ( doublePush ) {
            if (side == White) {
                enPassantSQ = targetSQ - 8;
                // update hashKey
                hashKey ^= randomEnPassantKeys[targetSQ - 8];
            } else {
                enPassantSQ = targetSQ + 8;
                hashKey ^= randomEnPassantKeys[targetSQ + 8];
            }
        }

        // here you should set castling to false
        if ( castling ) {
            switch(targetSQ) {
                case (G1):
                    SET_BIT_FALSE(bitboards[Rook], H1);
                    SET_BIT(bitboards[Rook], F1);

                    // hash rook:
                    hashKey ^= randomPieceKeys[Rook][H1];
                    hashKey ^= randomPieceKeys[Rook][F1];
                    break;

                case (C1):
                    SET_BIT_FALSE(bitboards[Rook], A1);
                    SET_BIT(bitboards[Rook], D1);
                    // hash rook:
                    hashKey ^= randomPieceKeys[Rook][A1];
                    hashKey ^= randomPieceKeys[Rook][D1];
                    break;

                case (G8):
                    SET_BIT_FALSE(bitboards[Rook + 6], H8);
                    SET_BIT(bitboards[Rook + 6], F8);
                    // hash rook:
                    hashKey ^= randomPieceKeys[Rook + 6][H8];
                    hashKey ^= randomPieceKeys[Rook + 6][F8];
                    break;
                case (C8):
                    SET_BIT_FALSE(bitboards[Rook + 6], A8);
                    SET_BIT(bitboards[Rook + 6], D8);
                    // hash rook:
                    hashKey ^= randomPieceKeys[Rook + 6][A8];
                    hashKey ^= randomPieceKeys[Rook + 6][D8];
                    break;

                default: break;
            }

        }
        // hash castling
        hashKey ^= randomCastlingKeys[castle]; // get trid of castling

        // castle bit hack for updating the rights
        castle &= castlingRightsConstant[startSQ];
        castle &= castlingRightsConstant[targetSQ];

        hashKey ^= randomCastlingKeys[castle]; // re-insert castling rights


        // updating the occupancies by resetting them and re initializing them
        memset(occupancies, 0ULL, 24);
        for (int bbPiece=0; bbPiece < 6; bbPiece++) {
            occupancies[0] |= bitboards[bbPiece]; // for white
            occupancies[1] |= bitboards[bbPiece + 6]; // for black
            occupancies[2] |= (bitboards[bbPiece] | bitboards[bbPiece + 6]); // for both
        }

        side ^= 1; // change side
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
        if (const U64 kingBitboard{ (side == White) ? bitboards[King + 6] : bitboards[King] }; isSqAttacked(getLeastSigBitIndex(kingBitboard), side)) {
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
