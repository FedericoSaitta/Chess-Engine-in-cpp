//
// Created by Federico Saitta on 28/06/2024.
//
#include <__algorithm/ranges_move.h>

#include "globals.h"
#include "constants.h"
#include "inline_functions.h"

// for now we just generate all moves, later on when we will do quiescent search we will introduce
// specificaitons to distinguish between captures and quiet moves


///*** little subtlelty, we are not checking the landing square of the castling eg g1 for white with isSQAttacked
//// as this is a pseudo legal generator, in the make move function we will check that the king is not in check
/// and will hence eliminate those moves
void generateMoves(MoveList& moveList) {

    // is this really needed though?
    moveList.count = 0;

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
                            addMove(moveList, encodeMove(startSquare, targetSquare, Pawn, Queen, 0, 0, 0, 0) );
                            addMove(moveList, encodeMove(startSquare, targetSquare, Pawn, Rook, 0, 0, 0, 0) );
                            addMove(moveList, encodeMove(startSquare, targetSquare, Pawn, Bishop, 0, 0, 0, 0) );
                            addMove(moveList, encodeMove(startSquare, targetSquare, Pawn, Knight, 0, 0, 0, 0) );


                        } else {
                            // one square ahead
                            addMove(moveList, encodeMove(startSquare, targetSquare, Pawn, 0, 0, 0, 0, 0) );
                            // two squares ahead
                            if ( (startSquare >= A2) && (startSquare <= H2) && !getBit(occupancies[2], targetSquare + 8)) {
                                addMove(moveList, encodeMove(startSquare, targetSquare + 8, Pawn, 0, 0, 1, 0, 0) );
                            }
                        }
                    }

                    // need to initialize the attack bitboard, can only capture black piececs
                    attacks = pawnAttacks[White][startSquare] & occupancies[Black];

                    while (attacks) {
                        targetSquare = getLeastSigBitIndex(attacks);

                        if ( (startSquare >= A7) && (startSquare <= H7) ) {
                            // then we can add this move to the list
                            addMove(moveList, encodeMove(startSquare, targetSquare, Pawn, Queen, 1, 0, 0, 0) );
                            addMove(moveList, encodeMove(startSquare, targetSquare, Pawn, Rook, 1, 0, 0, 0) );
                            addMove(moveList, encodeMove(startSquare, targetSquare, Pawn, Bishop, 1, 0, 0, 0) );
                            addMove(moveList, encodeMove(startSquare, targetSquare, Pawn, Knight, 1, 0, 0, 0) );

                        } else {
                            addMove(moveList, encodeMove(startSquare, targetSquare, Pawn, 0, 1, 0, 0, 0) );
                        }

                        setBitFalse(attacks, targetSquare);
                    }

                    // generate enPassantCaptures
                    if (enPassantSQ != 64) {
                        U64 enPassantAttacks = pawnAttacks[White][startSquare] & (1ULL << enPassantSQ);

                        if (enPassantAttacks) {
                            targetSquare = getLeastSigBitIndex(enPassantAttacks);
                            addMove(moveList, encodeMove(startSquare, targetSquare, Pawn, 0, 0, 0, 1, 0) );

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
                            addMove(moveList, encodeMove(startSquare, G1, King, 0, 0, 0, 0, 1) );
                        }
                    }
                }

                //queen side castling
                if (castle & WQ) {
                    // checking that the space is empty
                    if( !getBit(occupancies[2], B1) && !getBit(occupancies[2], C1) && !getBit(occupancies[2], D1)) {
                        if ( !isSqAttacked(E1, Black) && !isSqAttacked(D1, Black) ) {
                            addMove(moveList, encodeMove(startSquare, C1, King, 0, 0, 0, 0, 1) );
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
                            addMove(moveList, encodeMove(startSquare, targetSquare, Pawn + 6, Queen + 6, 0, 0, 0, 0) );
                            addMove(moveList, encodeMove(startSquare, targetSquare, Pawn + 6, Rook + 6, 0, 0, 0, 0) );
                            addMove(moveList, encodeMove(startSquare, targetSquare, Pawn + 6, Bishop + 6, 0, 0, 0, 0) );
                            addMove(moveList, encodeMove(startSquare, targetSquare, Pawn + 6, Knight + 6, 0, 0, 0, 0) );

                        } else {
                            // one square ahead
                            addMove(moveList, encodeMove(startSquare, targetSquare, Pawn + 6, 0, 0, 0, 0, 0) );

                            // two squares ahead
                            if ( (startSquare >= A7) && (startSquare <= H7) && !getBit(occupancies[2], targetSquare - 8)) {
                                addMove(moveList, encodeMove(startSquare, targetSquare - 8, Pawn + 6, 0, 0, 1, 0, 0) );
                            }
                        }
                    }
                    // need to initialize the attack bitboard, can only capture white piececs
                    attacks = pawnAttacks[Black][startSquare] & occupancies[White];

                    while (attacks) {
                        targetSquare = getLeastSigBitIndex(attacks);

                        if ( (startSquare >= A2) && (startSquare <= H2) ) {
                            // then we can add this move to the list
                            addMove(moveList, encodeMove(startSquare, targetSquare, Pawn + 6, Queen + 6, 1, 0, 0, 0) );
                            addMove(moveList, encodeMove(startSquare, targetSquare, Pawn + 6, Rook + 6, 1, 0, 0, 0) );
                            addMove(moveList, encodeMove(startSquare, targetSquare, Pawn + 6, Bishop + 6, 1, 0, 0, 0) );
                            addMove(moveList, encodeMove(startSquare, targetSquare, Pawn + 6, Knight + 6, 1, 0, 0, 0) );
                        } else {
                            addMove(moveList, encodeMove(startSquare, targetSquare, Pawn + 6, 0, 1, 0, 0, 0) );
                        }

                        setBitFalse(attacks, targetSquare);
                    }
                    // generate enPassantCaptures
                    if (enPassantSQ != 64) {
                        U64 enPassantAttacks = pawnAttacks[Black][startSquare] & (1ULL << enPassantSQ);

                        if (enPassantAttacks) {
                            targetSquare = getLeastSigBitIndex(enPassantAttacks);
                            addMove(moveList, encodeMove(startSquare, targetSquare, Pawn + 6, 0, 0, 0, 1, 0) );
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
                            addMove(moveList, encodeMove(startSquare, G8, King + 6, 0, 0, 0, 0, 1) );
                        }
                    }
                }

                //queen side castling
                if (castle & BQ) {
                    // checking that the space is empty
                    if( !getBit(occupancies[2], B8) && !getBit(occupancies[2], C8) && !getBit(occupancies[2], D8)) {
                        if ( !isSqAttacked(E8, White) && !isSqAttacked(D8, White) ) {
                            addMove(moveList, encodeMove(startSquare, C8, King + 6, 0, 0, 0, 0, 1) );
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
                        addMove(moveList, encodeMove(startSquare, targetSquare, piece, 0, 0, 0, 0, 0) );

                    } else {  // capture moves
                        addMove(moveList, encodeMove(startSquare, targetSquare, piece, 0, 1, 0, 0, 0) );
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
                        addMove(moveList, encodeMove(startSquare, targetSquare, piece, 0, 0, 0, 0, 0) );

                    } else {  // capture moves
                        addMove(moveList, encodeMove(startSquare, targetSquare, piece, 0, 1, 0, 0, 0) );
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
                        addMove(moveList, encodeMove(startSquare, targetSquare, piece, 0, 0, 0, 0, 0) );

                    } else {  // capture moves
                        addMove(moveList, encodeMove(startSquare, targetSquare, piece, 0, 1, 0, 0, 0) );
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
                        addMove(moveList, encodeMove(startSquare, targetSquare, piece, 0, 0, 0, 0, 0) );

                    } else {  // capture moves
                        addMove(moveList, encodeMove(startSquare, targetSquare, piece, 0, 1, 0, 0, 0) );
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
                        addMove(moveList, encodeMove(startSquare, targetSquare, piece, 0, 0, 0, 0, 0) );

                    } else {  // capture moves
                        addMove(moveList, encodeMove(startSquare, targetSquare, piece, 0, 1, 0, 0, 0) );
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


