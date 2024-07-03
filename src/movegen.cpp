//
// Created by Federico Saitta on 28/06/2024.
//
#include "globals.h"
#include "constants.h"
#include "inline_functions.h"

// for now we just generate all moves, later on when we will do quiescent search we will introduce
// specificaitons to distinguish between captures and quiet moves


///*** little subtlelty, we are not checking the landing square of the castling eg g1 for white with isSQAttacked
//// as this is a pseudo legal generator, in the make move function we will check that the king is not in check
/// and will hence eliminate those moves
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
            if (piece == Pawn) { // loop over white pawn bitboard

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

                    // need to initialize the attack bitboard, can only capture black piececs
                    attacks = pawnAttacks[White][startSquare] & occupancies[Black];

                    while (attacks) {
                        targetSquare = getLeastSigBitIndex(attacks);

                        if ( (startSquare >= A7) && (startSquare <= H7) ) {
                            // then we can add this move to the list
                            std::cout << "pawn capture promotion on: " << chessBoard[targetSquare] << '\n';
                        } else {
                            std::cout << "pawn capture on: " << chessBoard[targetSquare] << '\n';
                        }

                        setBitFalse(attacks, targetSquare);
                    }

                    // generate enPassantCaptures
                    if (enPassantSQ != 64) {
                        U64 enPassantAttacks = pawnAttacks[White][startSquare] & (1ULL << enPassantSQ);

                        if (enPassantAttacks) {
                            targetSquare = getLeastSigBitIndex(enPassantAttacks);
                            std::cout << "pawn en-passant on: " << chessBoard[targetSquare] << '\n';

                        }
                    }

                    setBitFalse(bitboard, startSquare);
                }
            } // this works

            if (piece == King) {
                // king side castling
                if (castle & WK) {
                    // checking that the space is empty
                    if( !getBit(occupancies[2], F1) && !getBit(occupancies[2], G1)) {
                        if ( !isSqAttacked(E1, Black) && !isSqAttacked(F1, Black) ) {
                            std::cout << "White WK castle" << '\n';
                        }
                    }
                }

                //queen side castling
                if (castle & WQ) {
                    // checking that the space is empty
                    if( !getBit(occupancies[2], B1) && !getBit(occupancies[2], C1) && !getBit(occupancies[2], D1)) {
                        if ( !isSqAttacked(E1, Black) && !isSqAttacked(D1, Black) ) {
                            std::cout << "White WQ castle" << '\n';
                        }
                    }
                }

            }
        } else {
            // generate black pawn moves and black king castling moves
            if (piece == (Pawn + 6) ) { // loop over white pawn bitboard

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
                    // need to initialize the attack bitboard, can only capture white piececs
                    attacks = pawnAttacks[Black][startSquare] & occupancies[White];

                    while (attacks) {
                        targetSquare = getLeastSigBitIndex(attacks);

                        if ( (startSquare >= A2) && (startSquare <= H2) ) {
                            // then we can add this move to the list
                            std::cout << "pawn capture promotion on: " << chessBoard[targetSquare] << '\n';
                        } else {
                            std::cout << "pawn capture on: " << chessBoard[targetSquare] << '\n';
                        }

                        setBitFalse(attacks, targetSquare);
                    }
                    // generate enPassantCaptures
                    if (enPassantSQ != 64) {
                        U64 enPassantAttacks = pawnAttacks[Black][startSquare] & (1ULL << enPassantSQ);

                        if (enPassantAttacks) {
                            targetSquare = getLeastSigBitIndex(enPassantAttacks);
                            std::cout << "pawn en-passant on: " << chessBoard[targetSquare] << '\n';
                        }
                    }

                    setBitFalse(bitboard, startSquare);
                }
            } // this works

            if (piece == (King + 6)) {
                // king side castling
                if (castle & BK) {
                    // checking that the space is empty
                    if( !getBit(occupancies[2], F8) && !getBit(occupancies[2], G8)) {
                        if ( !isSqAttacked(E8, White) && !isSqAttacked(F8, White) ) {
                            std::cout << "Black BK castle" << '\n';
                        }
                    }
                }

                //queen side castling
                if (castle & BQ) {
                    // checking that the space is empty
                    if( !getBit(occupancies[2], B8) && !getBit(occupancies[2], C8) && !getBit(occupancies[2], D8)) {
                        if ( !isSqAttacked(E8, White) && !isSqAttacked(D8, White) ) {
                            std::cout << "Black BQ castle" << '\n';
                        }
                    }
                }

            }
        }

        // generate knight moves
        if ( (side == White)? piece == Knight : piece == (Knight + 6) ) {
            while (bitboard) {
                startSquare = getLeastSigBitIndex(bitboard);

                // need to make sure landing squares are all but the ones occupied by your pieces
                attacks = bitKnightAttacks[startSquare] & ((side == White) ? ~occupancies[White] : ~occupancies[Black]);


                while (attacks) {
                    targetSquare = getLeastSigBitIndex(attacks);

                    // quiet moves
                    if ( !getBit( ((side == White) ? occupancies[Black] : occupancies[White]), targetSquare ) ){
                        std::cout << "Knight move on: " << chessBoard[targetSquare] << '\n';

                    } else {  // capture moves
                        std::cout << "Knight capture to: " << chessBoard[targetSquare] << '\n';
                    }

                    setBitFalse(attacks, targetSquare);
                }
                setBitFalse(bitboard, startSquare);
            }
        }

        // generate bishop moves
        if ( (side == White)? piece == Bishop : piece == (Bishop + 6) ) {
            while (bitboard) {
                startSquare = getLeastSigBitIndex(bitboard);

                // need to make sure landing squares are all but the ones occupied by your pieces
                attacks = getBishopAttacks(startSquare, occupancies[2]) & ((side == White) ? ~occupancies[White] : ~occupancies[Black]);


                while (attacks) {
                    targetSquare = getLeastSigBitIndex(attacks);

                    // quiet moves
                    if ( !getBit( ((side == White) ? occupancies[Black] : occupancies[White]), targetSquare ) ){
                        std::cout << "Bishop move on: " << chessBoard[targetSquare] << '\n';

                    } else {  // capture moves
                        std::cout << "Bishop capture to: " << chessBoard[targetSquare] << '\n';
                    }

                    setBitFalse(attacks, targetSquare);
                }
                setBitFalse(bitboard, startSquare);
            }
        }

        // generate rook moves
        if ( (side == White)? piece == Rook : piece == (Rook + 6) ) {
            while (bitboard) {
                startSquare = getLeastSigBitIndex(bitboard);

                // need to make sure landing squares are all but the ones occupied by your pieces
                attacks = getRookAttacks(startSquare, occupancies[2]) & ((side == White) ? ~occupancies[White] : ~occupancies[Black]);


                while (attacks) {
                    targetSquare = getLeastSigBitIndex(attacks);

                    // quiet moves
                    if ( !getBit( ((side == White) ? occupancies[Black] : occupancies[White]), targetSquare ) ){
                        std::cout << "Rook move on: " << chessBoard[targetSquare] << '\n';

                    } else {  // capture moves
                        std::cout << "Rook capture to: " << chessBoard[targetSquare] << '\n';
                    }

                    setBitFalse(attacks, targetSquare);
                }
                setBitFalse(bitboard, startSquare);
            }
        }

        // generate queen moves
        if ( (side == White)? piece == Queen : piece == (Queen + 6) ) {
            while (bitboard) {
                startSquare = getLeastSigBitIndex(bitboard);

                // need to make sure landing squares are all but the ones occupied by your pieces
                attacks = getQueenAttacks(startSquare, occupancies[2]) & ((side == White) ? ~occupancies[White] : ~occupancies[Black]);

                while (attacks) {
                    targetSquare = getLeastSigBitIndex(attacks);

                    // quiet moves
                    if ( !getBit( ((side == White) ? occupancies[Black] : occupancies[White]), targetSquare ) ){
                        std::cout << "Queen move on: " << chessBoard[targetSquare] << '\n';

                    } else {  // capture moves
                        std::cout << "Queen capture to: " << chessBoard[targetSquare] << '\n';
                    }

                    setBitFalse(attacks, targetSquare);
                }
                setBitFalse(bitboard, startSquare);
            }
        }

        // generate king moves
        if ( (side == White)? piece == King : piece == (King + 6) ) {
            while (bitboard) {
                startSquare = getLeastSigBitIndex(bitboard);

                // need to make sure landing squares are all but the ones occupied by your pieces
                attacks = bitKingAttacks[startSquare] & ((side == White) ? ~occupancies[White] : ~occupancies[Black]);

                while (attacks) {
                    targetSquare = getLeastSigBitIndex(attacks);

                    // quiet moves
                    if ( !getBit( ((side == White) ? occupancies[Black] : occupancies[White]), targetSquare ) ){
                        std::cout << "King move on: " << chessBoard[targetSquare] << '\n';

                    } else {  // capture moves
                        std::cout << "King capture to: " << chessBoard[targetSquare] << '\n';
                    }

                    setBitFalse(attacks, targetSquare);
                }
                setBitFalse(bitboard, startSquare);
            }
        }
    }

}

// Move encodings


// 0000 0000 0000 0000 0011 1111 start square           0x3f
// 0000 0000 0000 1111 1100 0000 target square          0xfc0
// 0000 0000 1111 0000 0000 0000 piece                  0xf000
// 0000 1111 0000 0000 0000 0000 promoted piece         0xf0000
// 0001 0000 0000 0000 0000 0000 capture flag           0x100000
// 0010 0000 0000 0000 0000 0000 double push flag       0x200000
// 0100 0000 0000 0000 0000 0000 en passant flag        0x400000
// 1000 0000 0000 0000 0000 0000 castling flag          0x800000


