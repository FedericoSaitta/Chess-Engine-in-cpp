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

    // make copies of board.bitboards because as we loop through them we remove the leftmost bits.
    U64 b1{};
    U64 attacks{};

    for (int piece=0; piece < 12; piece++) {

        b1 = board.bitboards[piece];

        // these are the special cases that dont have
        // generate WHITE pawn moves and WHITE king castling moves
        if (board.side == WHITE) {
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
                            addMove(moveList, Move(startSquare, targetSquare) );

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
                    if (board.enPassantSq != 64) {
                        U64 enPassantAttacks = bitPawnAttacks[WHITE][startSquare] & (1ULL << board.enPassantSq);

                        if (enPassantAttacks) {
                            targetSquare = bsf(enPassantAttacks);
                            addMove(moveList, Move(startSquare, targetSquare, EN_PASSANT) );
                        }
                    }
                }
            }// this works

            if (piece == KING) {
                // king board.side castling
                if (board.castle & WK) {
                    // checking that the space is empty
                    if( !GET_BIT(board.bitboards[BOTH_OCC], F1) && !GET_BIT(board.bitboards[BOTH_OCC], G1)) {
                        if ( !isSqAttacked(E1, BLACK) && !isSqAttacked(F1, BLACK) ) {
                            addMove(moveList, Move(E1, G1, OO) );
                        }
                    }
                }

                //QUEEN board.side castling
                if (board.castle & WQ) {
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
                            addMove(moveList, Move(startSquare, targetSquare) );

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
                    if (board.enPassantSq != 64) {
                        U64 enPassantAttacks = bitPawnAttacks[BLACK][startSquare] & (1ULL << board.enPassantSq);

                        if (enPassantAttacks) {
                            targetSquare = bsf(enPassantAttacks);
                            addMove(moveList, Move(startSquare, targetSquare, EN_PASSANT) );
                        }
                    }
                }
            } // this works

            if (piece == (KING + 6)) {
                // king board.side castling
                if (board.castle & BK) {
                    // checking that the space is empty
                    if( !GET_BIT(board.bitboards[BOTH_OCC], F8) && !GET_BIT(board.bitboards[BOTH_OCC], G8)) {
                        if ( !isSqAttacked(E8, WHITE) && !isSqAttacked(F8, WHITE) ) {
                            addMove(moveList, Move(E8, G8,OO) );
                        }
                    }
                }

                //QUEEN board.side castling
                if (board.castle & BQ) {
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
        if ( (board.side == WHITE)? piece == KNIGHT : piece == (KNIGHT + 6) ) {
            while (b1) {
                startSquare = pop_lsb(&b1);

                // need to make sure landing squares are all but the ones occupied by your pieces
                attacks = bitKnightAttacks[startSquare] & ((board.side == WHITE) ? ~board.bitboards[WHITE_OCC] : ~board.bitboards[BLACK_OCC]);


                while (attacks) {
                    targetSquare = pop_lsb(&attacks);

                    // quiet moves
                    if ( !GET_BIT( ((board.side == WHITE) ? board.bitboards[BLACK_OCC] : board.bitboards[WHITE_OCC]), targetSquare ) ){
                        addMove(moveList, Move(startSquare, targetSquare) );

                    } else {  // capture moves
                        addMove(moveList, Move(startSquare, targetSquare, CAPTURE) );
                    }
                }
            }
        }

        // generate BISHOP moves
        if ( (board.side == WHITE)? piece == BISHOP : piece == (BISHOP + 6) ) {
            while (b1) {
                startSquare = pop_lsb(&b1);

                // need to make sure landing squares are all but the ones occupied by your pieces
                attacks = getBishopAttacks(startSquare, board.bitboards[BOTH_OCC]) & ((board.side == WHITE) ? ~board.bitboards[WHITE_OCC] : ~board.bitboards[BLACK_OCC]);


                while (attacks) {
                    targetSquare = pop_lsb(&attacks);

                    // quiet moves
                    if ( !GET_BIT( ((board.side == WHITE) ? board.bitboards[BLACK_OCC] : board.bitboards[WHITE_OCC]), targetSquare ) ){
                        addMove(moveList, Move(startSquare, targetSquare) );

                    } else {  // capture moves
                        addMove(moveList, Move(startSquare, targetSquare, CAPTURE) );
                    }
                }
            }
        }

        // generate ROOK moves
        if ( (board.side == WHITE)? piece == ROOK : piece == (ROOK + 6) ) {
            while (b1) {
                startSquare = pop_lsb(&b1);

                // need to make sure landing squares are all but the ones occupied by your pieces
                attacks = getRookAttacks(startSquare, board.bitboards[BOTH_OCC]) & ((board.side == WHITE) ? ~board.bitboards[WHITE_OCC] : ~board.bitboards[BLACK_OCC]);

                while (attacks) {
                    targetSquare = pop_lsb(&attacks);

                    // quiet moves
                    if ( !GET_BIT( ((board.side == WHITE) ? board.bitboards[BLACK_OCC] : board.bitboards[WHITE_OCC]), targetSquare ) ){
                        addMove(moveList, Move(startSquare, targetSquare) );

                    } else {  // capture moves
                        addMove(moveList, Move(startSquare, targetSquare, CAPTURE) );
                    }
                }
            }
        }

        // generate QUEEN moves
        if ( (board.side == WHITE)? piece == QUEEN : piece == (QUEEN + 6) ) {
            while (b1) {
                startSquare = pop_lsb(&b1);

                // need to make sure landing squares are all but the ones occupied by your pieces
                attacks = getQueenAttacks(startSquare, board.bitboards[BOTH_OCC]) & ((board.side == WHITE) ? ~board.bitboards[WHITE_OCC] : ~board.bitboards[BLACK_OCC]);

                while (attacks) {
                    targetSquare = pop_lsb(&attacks);

                    // quiet moves
                    if ( !GET_BIT( ((board.side == WHITE) ? board.bitboards[BLACK_OCC] : board.bitboards[WHITE_OCC]), targetSquare ) ){
                        addMove(moveList, Move(startSquare, targetSquare) );

                    } else {  // capture moves
                        addMove(moveList, Move(startSquare, targetSquare, CAPTURE) );
                    }
                }
            }
        }

        // generate king moves
        if ( (board.side == WHITE)? piece == KING : piece == (KING + 6) ) {
            while (b1) {
                startSquare =pop_lsb(&b1);

                // need to make sure landing squares are all but the ones occupied by your pieces
                attacks = bitKingAttacks[startSquare] & ((board.side == WHITE) ? ~board.bitboards[WHITE_OCC] : ~board.bitboards[BLACK_OCC]);

                while (attacks) {
                    targetSquare = pop_lsb(&attacks);

                    // quiet moves
                    if ( !GET_BIT( ((board.side == WHITE) ? board.bitboards[BLACK_OCC] : board.bitboards[WHITE_OCC]), targetSquare ) ){
                        addMove(moveList, Move(startSquare, targetSquare) );

                    } else {  // capture moves
                        addMove(moveList, Move(startSquare, targetSquare, CAPTURE) );
                    }
                }
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

/// NEW MOVE APPROACH:
///
/// A move needs 16 bits to be stored
///
/// bit  0- 5: destination square (from 0 to 63)
/// bit  6-11: origin square (from 0 to 63)
/// bit 12-13: promotion piece type - 2 (from KNIGHT-2 to QUEEN-2)
/// bit 14-15: special move flag: promotion (1), en passant (2), castling (3)
/// NOTE: EN-PASSANT bit is set only when a pawn can be captured
///
///

/*
enum MoveFlags : int {
	QUIET = 0b0000, DOUBLE_PUSH = 0b0001,
	OO = 0b0010, OOO = 0b0011,
	CAPTURE = 0b1000,
	CAPTURES = 0b1111,
	EN_PASSANT = 0b1010,
	PROMOTIONS = 0b0111,
	PROMOTION_CAPTURES = 0b1100,
	PR_KNIGHT = 0b0100, PR_BISHOP = 0b0101, PR_ROOK = 0b0110, PR_QUEEN = 0b0111,
	PC_KNIGHT = 0b1100, PC_BISHOP = 0b1101, PC_ROOK = 0b1110, PC_QUEEN = 0b1111,
};

class Move {
private:
    //The internal representation of the move
    uint16_t move;
public:
    //Defaults to a null move (a1a1)
    inline Move() : move(0) {}

    inline Move(uint16_t m) { move = m; }

    inline Move(const uint16_t from, const uint16_t to) : move(0) {
        move = (from << 6) | to;
    }

    inline Move(const uint16_t from, const uint16_t to, const MoveFlags flags) : move(0) {
        move = (flags << 12) | (from << 6) | to;
    }

    inline int to() const { return int(move & 0x3f); }
    inline int from() const { return int((move >> 6) & 0x3f); }
    inline int to_from() const { return move & 0xffff; }
    inline MoveFlags flags() const { return MoveFlags((move >> 12) & 0xf); }

    inline bool is_capture() const {
        return (move >> 12) & CAPTURES;
    }
};



struct new_MoveList {
    Move moves[218];
    int count;
};
// should also change the type of moveList
void new_addMove(new_MoveList& moveList, const Move move) {
    moveList.moves[moveList.count] = move;
    moveList.count++;
}
*/