//
// Created by Federico Saitta on 28/06/2024.
//
#include "movegen.h"

#include "macros.h"
#include "init.h"
#include "board.h"
#include "inline_functions.h"



///*** little subtlelty, we are not checking the landing square of the castling eg g1 for white with isSQAttacked
//// as this is a pseudo legal generator, in the make move function we will check that the king is not in check
/// and will hence eliminate those moves

// captures only == 1 if you only want captures
void generateMoves(MoveList& moveList) {
    moveList.count = 0; // this is needed
    int startSquare{};
    int targetSquare{};

    // make copies of bitboards because as we loop through them we remove the leftmost bits.
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

                    // make sure the target is smaller or equal to start of the board
                    if ( (targetSquare <= H8) && !GET_BIT(occupancies[2], targetSquare) ) {
                        // pawn promotion, maybe change this to row check or something?
                        if ( (startSquare >= A7) && (startSquare <= H7) ) {
                            // then we can add this move to the list
                            addMove(moveList, ENCODE_MOVE(startSquare, targetSquare, piece, Queen, 0, 0, 0, 0) );
                            addMove(moveList, ENCODE_MOVE(startSquare, targetSquare, piece, Rook, 0, 0, 0, 0) );
                            addMove(moveList, ENCODE_MOVE(startSquare, targetSquare, piece, Bishop, 0, 0, 0, 0) );
                            addMove(moveList, ENCODE_MOVE(startSquare, targetSquare, piece, Knight, 0, 0, 0, 0) );


                        } else {
                            // one square ahead
                            addMove(moveList, ENCODE_MOVE(startSquare, targetSquare, piece, 0, 0, 0, 0, 0) );
                            // two squares ahead
                            if ( (startSquare >= A2) && (startSquare <= H2) && !GET_BIT(occupancies[2], targetSquare + 8)) {
                                addMove(moveList, ENCODE_MOVE(startSquare, targetSquare + 8, piece, 0, 0, 1, 0, 0) );
                            }
                        }
                    }

                    // need to initialize the attack bitboard, can only capture black piececs
                    attacks = bitPawnAttacks[White][startSquare] & occupancies[Black];

                    while (attacks) {
                        targetSquare = getLeastSigBitIndex(attacks);

                        if ( (startSquare >= A7) && (startSquare <= H7) ) {
                            // then we can add this move to the list
                            addMove(moveList, ENCODE_MOVE(startSquare, targetSquare, piece, Queen, 1, 0, 0, 0) );
                            addMove(moveList, ENCODE_MOVE(startSquare, targetSquare, piece, Rook, 1, 0, 0, 0) );
                            addMove(moveList, ENCODE_MOVE(startSquare, targetSquare, piece, Bishop, 1, 0, 0, 0) );
                            addMove(moveList, ENCODE_MOVE(startSquare, targetSquare, piece, Knight, 1, 0, 0, 0) );

                        } else {
                            addMove(moveList, ENCODE_MOVE(startSquare, targetSquare, piece, 0, 1, 0, 0, 0) );
                        }

                        SET_BIT_FALSE(attacks, targetSquare);
                    }

                    // generate enPassantCaptures
                    if (enPassantSQ != 64) {
                        U64 enPassantAttacks = bitPawnAttacks[White][startSquare] & (1ULL << enPassantSQ);

                        if (enPassantAttacks) {
                            targetSquare = getLeastSigBitIndex(enPassantAttacks);
                            addMove(moveList, ENCODE_MOVE(startSquare, targetSquare, piece, 0, 1, 0, 1, 0) );
                        }
                    }
                    SET_BIT_FALSE(bitboard, startSquare);
                }
            } // this works

            if (piece == King) {
                // king side castling
                if (castle & WK) {
                    // checking that the space is empty
                    if( !GET_BIT(occupancies[2], F1) && !GET_BIT(occupancies[2], G1)) {
                        if ( !isSqAttacked(E1, Black) && !isSqAttacked(F1, Black) ) {
                            addMove(moveList, ENCODE_MOVE(E1, G1, piece, 0, 0, 0, 0, 1) );
                        }
                    }
                }

                //queen side castling
                if (castle & WQ) {
                    // checking that the space is empty
                    if( !GET_BIT(occupancies[2], B1) && !GET_BIT(occupancies[2], C1) && !GET_BIT(occupancies[2], D1)) {
                        if ( !isSqAttacked(E1, Black) && !isSqAttacked(D1, Black) ) {
                            addMove(moveList, ENCODE_MOVE(E1, C1, piece, 0, 0, 0, 0, 1) );
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

                    // make sure the target is larger or equal to start of the board
                    if ( (targetSquare >= A1) && !GET_BIT(occupancies[2], targetSquare) ) {
                        // pawn promotion, maybe change this to row check or something?
                        if ( (startSquare >= A2) && (startSquare <= H2) ) {
                            // then we can add this move to the list
                            addMove(moveList, ENCODE_MOVE(startSquare, targetSquare, piece, Queen + 6, 0, 0, 0, 0) );
                            addMove(moveList, ENCODE_MOVE(startSquare, targetSquare, piece, Rook + 6, 0, 0, 0, 0) );
                            addMove(moveList, ENCODE_MOVE(startSquare, targetSquare, piece, Bishop + 6, 0, 0, 0, 0) );
                            addMove(moveList, ENCODE_MOVE(startSquare, targetSquare, piece, Knight + 6, 0, 0, 0, 0) );

                        } else {
                            // one square ahead
                            addMove(moveList, ENCODE_MOVE(startSquare, targetSquare, piece, 0, 0, 0, 0, 0) );

                            // two squares ahead
                            if ( (startSquare >= A7) && (startSquare <= H7) && !GET_BIT(occupancies[2], targetSquare - 8)) {
                                addMove(moveList, ENCODE_MOVE(startSquare, targetSquare - 8, piece, 0, 0, 1, 0, 0) );
                            }
                        }
                    }
                    // need to initialize the attack bitboard, can only capture white piececs
                    attacks = bitPawnAttacks[Black][startSquare] & occupancies[White];

                    while (attacks) {
                        targetSquare = getLeastSigBitIndex(attacks);

                        if ( (startSquare >= A2) && (startSquare <= H2) ) {
                            // then we can add this move to the list
                            addMove(moveList, ENCODE_MOVE(startSquare, targetSquare, piece, Queen + 6, 1, 0, 0, 0) );
                            addMove(moveList, ENCODE_MOVE(startSquare, targetSquare, piece, Rook + 6, 1, 0, 0, 0) );
                            addMove(moveList, ENCODE_MOVE(startSquare, targetSquare, piece, Bishop + 6, 1, 0, 0, 0) );
                            addMove(moveList, ENCODE_MOVE(startSquare, targetSquare, piece, Knight + 6, 1, 0, 0, 0) );
                        } else {
                            addMove(moveList, ENCODE_MOVE(startSquare, targetSquare, piece, 0, 1, 0, 0, 0) );
                        }

                        SET_BIT_FALSE(attacks, targetSquare);
                    }
                    // generate enPassantCaptures
                    if (enPassantSQ != 64) {
                        U64 enPassantAttacks = bitPawnAttacks[Black][startSquare] & (1ULL << enPassantSQ);

                        if (enPassantAttacks) {
                            targetSquare = getLeastSigBitIndex(enPassantAttacks);
                            addMove(moveList, ENCODE_MOVE(startSquare, targetSquare, piece, 0, 1, 0, 1, 0) );
                        }
                    }

                    SET_BIT_FALSE(bitboard, startSquare);
                }
            } // this works

            if (piece == (King + 6)) {
                // king side castling
                if (castle & BK) {
                    // checking that the space is empty
                    if( !GET_BIT(occupancies[2], F8) && !GET_BIT(occupancies[2], G8)) {
                        if ( !isSqAttacked(E8, White) && !isSqAttacked(F8, White) ) {
                            addMove(moveList, ENCODE_MOVE(E8, G8, piece, 0, 0, 0, 0, 1) );
                        }
                    }
                }

                //queen side castling
                if (castle & BQ) {
                    // checking that the space is empty
                    if( !GET_BIT(occupancies[2], B8) && !GET_BIT(occupancies[2], C8) && !GET_BIT(occupancies[2], D8)) {
                        if ( !isSqAttacked(E8, White) && !isSqAttacked(D8, White) ) {
                            addMove(moveList, ENCODE_MOVE(E8, C8, piece, 0, 0, 0, 0, 1) );
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
                    if ( !GET_BIT( ((side == White) ? occupancies[Black] : occupancies[White]), targetSquare ) ){
                        addMove(moveList, ENCODE_MOVE(startSquare, targetSquare, piece, 0, 0, 0, 0, 0) );

                    } else {  // capture moves
                        addMove(moveList, ENCODE_MOVE(startSquare, targetSquare, piece, 0, 1, 0, 0, 0) );
                    }

                    SET_BIT_FALSE(attacks, targetSquare);
                }
                SET_BIT_FALSE(bitboard, startSquare);
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
                    if ( !GET_BIT( ((side == White) ? occupancies[Black] : occupancies[White]), targetSquare ) ){
                        addMove(moveList, ENCODE_MOVE(startSquare, targetSquare, piece, 0, 0, 0, 0, 0) );

                    } else {  // capture moves
                        addMove(moveList, ENCODE_MOVE(startSquare, targetSquare, piece, 0, 1, 0, 0, 0) );
                    }

                    SET_BIT_FALSE(attacks, targetSquare);
                }
                SET_BIT_FALSE(bitboard, startSquare);
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
                    if ( !GET_BIT( ((side == White) ? occupancies[Black] : occupancies[White]), targetSquare ) ){
                        addMove(moveList, ENCODE_MOVE(startSquare, targetSquare, piece, 0, 0, 0, 0, 0) );

                    } else {  // capture moves
                        addMove(moveList, ENCODE_MOVE(startSquare, targetSquare, piece, 0, 1, 0, 0, 0) );
                    }

                    SET_BIT_FALSE(attacks, targetSquare);
                }
                SET_BIT_FALSE(bitboard, startSquare);
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
                    if ( !GET_BIT( ((side == White) ? occupancies[Black] : occupancies[White]), targetSquare ) ){
                        addMove(moveList, ENCODE_MOVE(startSquare, targetSquare, piece, 0, 0, 0, 0, 0) );

                    } else {  // capture moves
                        addMove(moveList, ENCODE_MOVE(startSquare, targetSquare, piece, 0, 1, 0, 0, 0) );
                    }

                    SET_BIT_FALSE(attacks, targetSquare);
                }
                SET_BIT_FALSE(bitboard, startSquare);
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
                    if ( !GET_BIT( ((side == White) ? occupancies[Black] : occupancies[White]), targetSquare ) ){
                        addMove(moveList, ENCODE_MOVE(startSquare, targetSquare, piece, 0, 0, 0, 0, 0) );

                    } else {  // capture moves
                        addMove(moveList, ENCODE_MOVE(startSquare, targetSquare, piece, 0, 1, 0, 0, 0) );
                    }

                    SET_BIT_FALSE(attacks, targetSquare);
                }
                SET_BIT_FALSE(bitboard, startSquare);
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


