//
// Created by Federico Saitta on 28/06/2024.
//
#include "movegen.h"

#include "macros.h"
#include "init.h"
#include "board.h"
#include "inline_functions.h"
#include "misc.h"



///*** little subtlelty, we are not checking the landing square of the castling eg g1 for white with isSQAttacked
//// as this is a pseudo legal generator, in the make move function we will check that the king is not in check
/// and will hence eliminate those moves

// captures only == 1 if you only want captures
void generateMoves(MoveList& moveList) {
    moveList.count = 0; // this is needed
    int startSquare{};
    int targetSquare{};

    // make copies of bitboards because as we loop through them we remove the leftmost bits.
    U64 b1{};
    U64 attacks{};

    for (int piece=0; piece < 12; piece++) {

        b1 = bitboards[piece];

        // these are the special cases that dont have
        // generate white pawn moves and white king castling moves
        if (side == White) {
            if (piece == Pawn) { // loop over white pawn b1

                while(b1) {
                    startSquare = getLeastSigBitIndex(b1);

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

                    // need to initialize the attack b1, can only capture black piececs
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
                    SET_BIT_FALSE(b1, startSquare);
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
            if (piece == (Pawn + 6) ) { // loop over white pawn b1

                while(b1) {
                    startSquare = getLeastSigBitIndex(b1);

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
                    // need to initialize the attack b1, can only capture white piececs
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

                    SET_BIT_FALSE(b1, startSquare);
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
            while (b1) {
                startSquare = getLeastSigBitIndex(b1);

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
                SET_BIT_FALSE(b1, startSquare);
            }
        }

        // generate bishop moves
        if ( (side == White)? piece == Bishop : piece == (Bishop + 6) ) {
            while (b1) {
                startSquare = getLeastSigBitIndex(b1);

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
                SET_BIT_FALSE(b1, startSquare);
            }
        }

        // generate rook moves
        if ( (side == White)? piece == Rook : piece == (Rook + 6) ) {
            while (b1) {
                startSquare = getLeastSigBitIndex(b1);

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
                SET_BIT_FALSE(b1, startSquare);
            }
        }

        // generate queen moves
        if ( (side == White)? piece == Queen : piece == (Queen + 6) ) {
            while (b1) {
                startSquare = getLeastSigBitIndex(b1);

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
                SET_BIT_FALSE(b1, startSquare);
            }
        }

        // generate king moves
        if ( (side == White)? piece == King : piece == (King + 6) ) {
            while (b1) {
                startSquare = getLeastSigBitIndex(b1);

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
                SET_BIT_FALSE(b1, startSquare);
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

const int DEBRUIJN64[64] = {
    0, 47,  1, 56, 48, 27,  2, 60,
   57, 49, 41, 37, 28, 16,  3, 61,
   54, 58, 35, 52, 50, 42, 21, 44,
   38, 32, 29, 23, 17, 11,  4, 62,
   46, 55, 26, 59, 40, 36, 15, 53,
   34, 51, 20, 43, 31, 22, 10, 45,
   25, 39, 14, 33, 19, 30,  9, 24,
   13, 18,  8, 12,  7,  6,  5, 63
};
constexpr U64 MAGIC = 0x03f79d71b4cb0a89;

//Returns the index of the least significant bit in the b1
constexpr int bsf(U64 b) {
    return int(DEBRUIJN64[MAGIC * (b ^ (b - 1)) >> 58]);
}

inline int pop_lsb(U64* b) {
    const int lsb = bsf(*b);
    *b &= *b - 1;
    return lsb;
}


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

    inline Move(int from, int to) : move(0) {
        move = (from << 6) | to;
    }

    inline Move(int from, int to, MoveFlags flags) : move(0) {
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
