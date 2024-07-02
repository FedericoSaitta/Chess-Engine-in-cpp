//
// Created by Federico Saitta on 28/06/2024.
//
#include "globals.h"
#include "constants.h"
#include "inline_functions.h"

// for now we just generate all moves, later on when we will do quiescent search we will introduce
// specificaitons to distinguish between captures and quiet moves

void generateMoves() {
    int startSquare{};
    int targetSquare{};

    // make a copy of the bitboards
    U64 bitboard{};
    U64 attacks{};

    for (int piece=0; piece < 12; piece++) {

        bitboard = bitboards[piece];

        // these are the special cases that dont have
        // generate white pawn moves and white king castling moves
        if (side == White) {
            if (piece == P) { // loop over white pawn bitboard

                while(bitboard) {
                    startSquare = getLeastSigBitIndex(bitboard);

                    targetSquare = startSquare + 8;

                    if ( (targetSquare < H8) && !getBit(occupancies[2], targetSquare) ) {
                        // pawn promotion, maybe change this to row check or something?
                        if ( (startSquare >= A7) && (startSquare <= H7) ) {
                            // then we can add this move to the list
                            std::cout << "pawn promotion on: " << chessBoard[targetSquare] << '\n';


                        } else {
                            // one square ahead
                            std::cout << "pawn move to: " << chessBoard[targetSquare] << '\n';
                            // two squares ahead
                            if ( (startSquare >= A2) && (startSquare <= H2) && !getBit(occupancies[2], targetSquare + 8)) {
                                std::cout << "double pawn move to: " << chessBoard[targetSquare + 8] << '\n';
                            }
                        }
                    }
                    setBitFalse(bitboard, startSquare);
                }
            }


        } else { // generate black pawn moves and black king castling moves
            if (piece == (P + 6) ) { // loop over white pawn bitboard

                while(bitboard) {
                    startSquare = getLeastSigBitIndex(bitboard);

                    targetSquare = startSquare - 8;

                    if ( (targetSquare > A1) && !getBit(occupancies[2], targetSquare) ) {
                        // pawn promotion, maybe change this to row check or something?
                        if ( (startSquare >= A2) && (startSquare <= H2) ) {
                            // then we can add this move to the list
                            std::cout << "pawn promotion on: " << chessBoard[targetSquare] << '\n';

                        } else {
                            // one square ahead
                            std::cout << "pawn move to: " << chessBoard[targetSquare] << '\n';
                            // two squares ahead
                            if ( (startSquare >= A7) && (startSquare <= H7) && !getBit(occupancies[2], targetSquare - 8)) {
                                std::cout << "double pawn move to: " << chessBoard[targetSquare - 8] << '\n';
                            }
                        }
                    }
                    setBitFalse(bitboard, startSquare);
                }
            }



        }

        // generate knight moves


        // generate bishop moves


        // generate rook moves


        // generate queen moves


        // generate king moves

    }


}


