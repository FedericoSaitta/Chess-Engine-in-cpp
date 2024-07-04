//
// Created by Federico Saitta on 28/06/2024.
// deals with uupdating the board after a move is made, also has undo move function

#include "globals.h"
#include "constants.h"
#include "inline_functions.h"

#include <cstring>

// this is for little endian
const int castlingRightsConstant[64] = {
    13, 15, 15, 15,  12, 15, 15, 14,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    7, 15, 15, 15, 3, 15, 15, 11
};



// 0 represents illegal move, 1 represents the legal move;
int makeMove(const int move, const int onlyCaptures){

    // quiet moves
    if(!onlyCaptures) { // here set to 0
        copyBoard();

        const int startSQ { getMoveStartSQ(move) };
        const int targetSQ { getMoveTargettSQ(move) };
        const int piece { getMovePiece(move) };
        const int promPiece { getMovePromPiece(move) };
        const int capture { getMoveCapture(move) };
        const int doublePush { getMoveDoublePush(move) };
        const int enPassant { getMoveEnPassant(move) };
        const int castling { getMoveCastling(move) };

        setBitFalse(bitboards[piece], startSQ);
        setBit(bitboards[piece], targetSQ);

        if ( capture ) {
            int startPiece, endPiece;

            if (side == White ) { startPiece = Pawn + 6; endPiece = King + 6; }
            else { startPiece = Pawn; endPiece = King; }

            for (int bbPiece=startPiece; bbPiece <= endPiece; bbPiece++) {

                if ( getBit(bitboards[bbPiece], targetSQ) ) {
                    setBitFalse(bitboards[bbPiece], targetSQ);
                    break;
                }

            }
        }

        if ( promPiece ) {
            setBitFalse(bitboards[piece], targetSQ);
            setBit( bitboards[promPiece], targetSQ);
        }

        if ( enPassant ) {
            (side == White) ? setBitFalse( bitboards[Pawn + 6], targetSQ - 8) : setBitFalse( bitboards[Pawn], targetSQ + 8);
        }
        // need to reset enPassant square
        enPassantSQ = 64;

        if ( doublePush ) {
            // here we need to update the enPassant square
            enPassantSQ = (side == White) ? (targetSQ - 8) : (targetSQ + 8);
        }

        // here you should set castling to false
        if ( castling ) {
            switch(targetSQ) {
                case (G1):
                    setBitFalse(bitboards[Rook], H1);
                    setBit(bitboards[Rook], F1);
                    break;

                case (C1):
                    setBitFalse(bitboards[Rook], A1);
                    setBit(bitboards[Rook], D1);
                    break;

                case (G8):
                    setBitFalse(bitboards[Rook + 6], H8);
                    setBit(bitboards[Rook + 6], F8);
                    break;
                case (C8):
                    setBitFalse(bitboards[Rook + 6], A8);
                    setBit(bitboards[Rook + 6], D8);
                    break;

                default: break;
            }

        }


        castle &= castlingRightsConstant[startSQ];
        castle &= castlingRightsConstant[targetSQ];



        // updating the occupancies by resetting them and re initializing them
        memset(occupancies, 0ULL, 24);
        for (int bbPiece=0; bbPiece < 6; bbPiece++) {
            occupancies[0] |= bitboards[bbPiece]; // for white
            occupancies[1] |= bitboards[bbPiece + 6]; // for black
            occupancies[2] |= (bitboards[bbPiece] | bitboards[bbPiece + 6]); // for both
        }

        // change side
        side ^= 1;

        // make sure that the king has not been exposed into check
        const U64 kingBitboard{ (side == White) ? bitboards[King + 6] : bitboards[King] };
        if ( isSqAttacked(getLeastSigBitIndex(kingBitboard), side)) {
            // square is illegal so we take it back
            restoreBoard();
            return 0;
        }
        return 1;
    }

    // used in Quiescent search
    if ( getMoveCapture(move) ) {
        return makeMove(move, 0); // make the move
        // forgetting this return statement causes issues within the quiescence search
    }

    return 0;
}
