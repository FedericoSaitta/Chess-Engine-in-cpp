//
// Created by Federico Saitta on 28/06/2024.
//
#include "movegen.h"

#include "macros.h"
#include "init.h"
#include "board.h"
#include "inline_functions.h"
#include "misc.h"



///*** little subtlelty, we are not checking the landing square of the castling eg g1 for WHITE with isSQAttacked
//// as this is a pseudo legal generator, in the make move function we will check that the king is not in check
/// and will hence eliminate those moves

// captures only == 1 if you only want captures
void generateMoves(MoveList& moveList) {
    moveList.count = 0; // this is needed
    int startSquare{};
    int targetSquare{};

    const Color C { static_cast<Color>(board.side) };

    // make copies of board.bitboards because as we loop through them we remove the leftmost bits.
    U64 b1{};
    U64 attacks{};

    for (int piece=0; piece < 12; piece++) {

        b1 = board.bitboards[piece];

        // these are the special cases that dont have
        // generate WHITE pawn moves and WHITE king castling moves
        if (C == WHITE) {
            if (piece == PAWN) {
                // loop over WHITE pawn b1

                while(b1) {
                    startSquare = pop_lsb(&b1);

                    targetSquare = startSquare + 8;

                    // make sure the target is smaller or equal to start of the board
                    if ( (targetSquare <= H8) && !GET_BIT(board.bitboards[BOTH_OCC], targetSquare) ) {
                        // pawn promotion, maybe change this to row check or something?
                        if ( (startSquare >= A7) && (startSquare <= H7) ) {
                            // then we can add this move to the list
                            addMove(moveList, Move(startSquare, targetSquare, PR_QUEEN) );
                            addMove(moveList, Move(startSquare, targetSquare, PR_ROOK) );
                            addMove(moveList, Move(startSquare, targetSquare, PR_BISHOP) );
                            addMove(moveList, Move(startSquare, targetSquare, PR_KNIGHT) );

                        } else {
                            // one square ahead
                            addMove(moveList, Move(startSquare, targetSquare, QUIET) );

                            // two square ahead
                            if ( (startSquare >= A2) && (startSquare <= H2) && !GET_BIT(board.bitboards[BOTH_OCC], targetSquare + 8)) {
                                addMove(moveList, Move(startSquare, targetSquare + 8, DOUBLE_PUSH) );
                            }
                        }
                    }
                    // need to initialize the attack b1, can only capture BLACK piececs
                    attacks = bitPawnAttacks[WHITE][startSquare] & board.bitboards[BLACK_OCC];

                    while (attacks) {
                        targetSquare = pop_lsb(&attacks);

                        if ( (startSquare >= A7) && (startSquare <= H7) ) {
                            // then we can add this move to the list
                            addMove(moveList, Move(startSquare, targetSquare, PC_QUEEN) );
                            addMove(moveList, Move(startSquare, targetSquare, PC_ROOK) );
                            addMove(moveList, Move(startSquare, targetSquare, PC_BISHOP) );
                            addMove(moveList, Move(startSquare, targetSquare, PC_KNIGHT) );

                        } else {
                            addMove(moveList, Move(startSquare, targetSquare, CAPTURE) );
                        }
                    }

                    // generate enPassantCaptures
                    if (board.history[board.gamePly].enPassSq != 64) {
                        U64 enPassantAttacks = bitPawnAttacks[WHITE][startSquare] & (1ULL << board.history[board.gamePly].enPassSq);

                        if (enPassantAttacks) {
                            targetSquare = bsf(enPassantAttacks);
                            addMove(moveList, Move(startSquare, targetSquare, EN_PASSANT) );
                        }
                    }
                }
            }// this works

            if (piece == KING) {
                // king C castling
                if (board.history[board.gamePly].castle & WK) {
                    // checking that the space is empty
                    if( !GET_BIT(board.bitboards[BOTH_OCC], F1) && !GET_BIT(board.bitboards[BOTH_OCC], G1)) {
                        if ( !isSqAttacked(E1, BLACK) && !isSqAttacked(F1, BLACK) ) {
                            addMove(moveList, Move(E1, G1, OO) );
                        }
                    }
                }

                //QUEEN C castling
                if (board.history[board.gamePly].castle & WQ) {
                    // checking that the space is empty
                    if( !GET_BIT(board.bitboards[BOTH_OCC], B1) && !GET_BIT(board.bitboards[BOTH_OCC], C1) && !GET_BIT(board.bitboards[BOTH_OCC], D1)) {
                        if ( !isSqAttacked(E1, BLACK) && !isSqAttacked(D1, BLACK) ) {
                            addMove(moveList, Move(E1, C1, OOO) );
                        }
                    }
                }
            }

        } else {
            // generate BLACK pawn moves and BLACK king castling moves
            if (piece == (PAWN + 6) ) { // loop over WHITE pawn b1

                while(b1) {
                    startSquare = pop_lsb(&b1);

                    targetSquare = startSquare - 8;

                    // make sure the target is larger or equal to start of the board
                    if ( (targetSquare >= A1) && !GET_BIT(board.bitboards[BOTH_OCC], targetSquare) ) {
                        // pawn promotion, maybe change this to row check or something?
                        if ( (startSquare >= A2) && (startSquare <= H2) ) {
                            // then we can add this move to the list
                            addMove(moveList, Move(startSquare, targetSquare, PR_QUEEN) );
                            addMove(moveList, Move(startSquare, targetSquare, PR_ROOK) );
                            addMove(moveList, Move(startSquare, targetSquare, PR_BISHOP) );
                            addMove(moveList, Move(startSquare, targetSquare, PR_KNIGHT) );

                        } else {
                            // one square ahead
                            addMove(moveList, Move(startSquare, targetSquare, QUIET) );

                            // two squares ahead
                            if ( (startSquare >= A7) && (startSquare <= H7) && !GET_BIT(board.bitboards[BOTH_OCC], targetSquare - 8)) {
                                addMove(moveList, Move(startSquare, targetSquare - 8, DOUBLE_PUSH) );
                            }
                        }
                    }
                    // need to initialize the attack b1, can only capture WHITE piececs
                    attacks = bitPawnAttacks[BLACK][startSquare] & board.bitboards[WHITE_OCC];

                    while (attacks) {
                        targetSquare = pop_lsb(&attacks);

                        if ( (startSquare >= A2) && (startSquare <= H2) ) {
                            // then we can add this move to the list
                            addMove(moveList, Move(startSquare, targetSquare, PC_QUEEN) );
                            addMove(moveList, Move(startSquare, targetSquare, PC_ROOK) );
                            addMove(moveList, Move(startSquare, targetSquare, PC_BISHOP) );
                            addMove(moveList, Move(startSquare, targetSquare, PC_KNIGHT) );

                        } else {
                            addMove(moveList, Move(startSquare, targetSquare, CAPTURE) );
                        }
                    }
                    // generate enPassantCaptures
                    if (board.history[board.gamePly].enPassSq != 64) {
                        U64 enPassantAttacks = bitPawnAttacks[BLACK][startSquare] & (1ULL << board.history[board.gamePly].enPassSq);

                        if (enPassantAttacks) {
                            targetSquare = bsf(enPassantAttacks);
                            addMove(moveList, Move(startSquare, targetSquare, EN_PASSANT) );
                        }
                    }
                }
            } // this works

            if (piece == (KING + 6)) {
                // king C castling
                if (board.history[board.gamePly].castle & BK) {
                    // checking that the space is empty
                    if( !GET_BIT(board.bitboards[BOTH_OCC], F8) && !GET_BIT(board.bitboards[BOTH_OCC], G8)) {
                        if ( !isSqAttacked(E8, WHITE) && !isSqAttacked(F8, WHITE) ) {
                            addMove(moveList, Move(E8, G8,OO) );
                        }
                    }
                }

                //QUEEN C castling
                if (board.history[board.gamePly].castle & BQ) {
                    // checking that the space is empty
                    if( !GET_BIT(board.bitboards[BOTH_OCC], B8) && !GET_BIT(board.bitboards[BOTH_OCC], C8) && !GET_BIT(board.bitboards[BOTH_OCC], D8)) {
                        if ( !isSqAttacked(E8, WHITE) && !isSqAttacked(D8, WHITE) ) {
                            addMove(moveList, Move(E8, C8, OOO) );
                        }
                    }
                }

            }
        }

        // generate KNIGHT moves
        if ( (C == WHITE)? piece == KNIGHT : piece == (KNIGHT + 6) ) {
            while (b1) {
                startSquare = pop_lsb(&b1);

                // need to make sure landing squares are all but the ones occupied by your pieces
                attacks = bitKnightAttacks[startSquare] & ((C == WHITE) ? ~board.bitboards[WHITE_OCC] : ~board.bitboards[BLACK_OCC]);


                while (attacks) {
                    targetSquare = pop_lsb(&attacks);

                    // quiet moves
                    if ( !GET_BIT( ((C == WHITE) ? board.bitboards[BLACK_OCC] : board.bitboards[WHITE_OCC]), targetSquare ) ){
                        addMove(moveList, Move(startSquare, targetSquare, QUIET) );

                    } else {  // capture moves
                        addMove(moveList, Move(startSquare, targetSquare, CAPTURE) );
                    }
                }
            }
        }

        // generate BISHOP moves
        if ( (C == WHITE)? piece == BISHOP : piece == (BISHOP + 6) ) {
            while (b1) {
                startSquare = pop_lsb(&b1);

                // need to make sure landing squares are all but the ones occupied by your pieces
                attacks = getBishopAttacks(startSquare, board.bitboards[BOTH_OCC]) & ((C == WHITE) ? ~board.bitboards[WHITE_OCC] : ~board.bitboards[BLACK_OCC]);


                while (attacks) {
                    targetSquare = pop_lsb(&attacks);

                    // quiet moves
                    if ( !GET_BIT( ((C == WHITE) ? board.bitboards[BLACK_OCC] : board.bitboards[WHITE_OCC]), targetSquare ) ){
                        addMove(moveList, Move(startSquare, targetSquare, QUIET) );

                    } else {  // capture moves
                        addMove(moveList, Move(startSquare, targetSquare, CAPTURE) );
                    }
                }
            }
        }

        // generate ROOK moves
        if ( (C == WHITE)? piece == ROOK : piece == (ROOK + 6) ) {
            while (b1) {
                startSquare = pop_lsb(&b1);

                // need to make sure landing squares are all but the ones occupied by your pieces
                attacks = getRookAttacks(startSquare, board.bitboards[BOTH_OCC]) & ((C == WHITE) ? ~board.bitboards[WHITE_OCC] : ~board.bitboards[BLACK_OCC]);

                while (attacks) {
                    targetSquare = pop_lsb(&attacks);

                    // quiet moves
                    if ( !GET_BIT( ((C == WHITE) ? board.bitboards[BLACK_OCC] : board.bitboards[WHITE_OCC]), targetSquare ) ){
                        addMove(moveList, Move(startSquare, targetSquare, QUIET) );

                    } else {  // capture moves
                        addMove(moveList, Move(startSquare, targetSquare, CAPTURE) );
                    }
                }
            }
        }

        // generate QUEEN moves
        if ( (C == WHITE)? piece == QUEEN : piece == (QUEEN + 6) ) {
            while (b1) {
                startSquare = pop_lsb(&b1);

                // need to make sure landing squares are all but the ones occupied by your pieces
                attacks = getQueenAttacks(startSquare, board.bitboards[BOTH_OCC]) & ((C == WHITE) ? ~board.bitboards[WHITE_OCC] : ~board.bitboards[BLACK_OCC]);

                while (attacks) {
                    targetSquare = pop_lsb(&attacks);

                    // quiet moves
                    if ( !GET_BIT( ((C == WHITE) ? board.bitboards[BLACK_OCC] : board.bitboards[WHITE_OCC]), targetSquare ) ){
                        addMove(moveList, Move(startSquare, targetSquare, QUIET) );

                    } else {  // capture moves
                        addMove(moveList, Move(startSquare, targetSquare, CAPTURE) );
                    }
                }
            }
        }

        // generate king moves
        if ( (C == WHITE)? piece == KING : piece == (KING + 6) ) {
            while (b1) {
                startSquare =pop_lsb(&b1);

                // need to make sure landing squares are all but the ones occupied by your pieces
                attacks = bitKingAttacks[startSquare] & ((C == WHITE) ? ~board.bitboards[WHITE_OCC] : ~board.bitboards[BLACK_OCC]);

                while (attacks) {
                    targetSquare = pop_lsb(&attacks);

                    // quiet moves
                    if ( !GET_BIT( ((C == WHITE) ? board.bitboards[BLACK_OCC] : board.bitboards[WHITE_OCC]), targetSquare ) ){
                        addMove(moveList, Move(startSquare, targetSquare, QUIET) );

                    } else {  // capture moves
                        addMove(moveList, Move(startSquare, targetSquare, CAPTURE) );
                    }
                }
            }
        }
    }

}
