//
// Created by Federico Saitta on 28/06/2024.
// deals with uupdating the board after a move is made, also has undo move function
#include "update.h"

#include "../../include/macros.h"
#include "types.h"
#include "board.h"

#include "../../include/hashtable.h"


#include <cstring>
#include <variant>

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

/*
inline void move_piece(int from, int to, int piece) {
    hashKey ^= randomPieceKeys[piece][from] ^ randomPieceKeys[piece][to];
    // need to find out the captured piece
   //     ^ zobrist::zobrist_table[board[to]][to];

    U64 mask = SQUARE_BB[from] | SQUARE_BB[to];
    piece_bb[board[from]] ^= mask;
    piece_bb[board[to]] &= ~mask;
    board[to] = board[from];
    board[from] = NO_PIECE;
}
*/

//Moves a piece to an empty square. Note that it is an error if the <to> square contains a piece
inline void move_piece_quiet(const int from, const int to) {
    const Piece piece { board.mailbox[from] };

    hashKey ^= randomPieceKeys[piece][from] ^ randomPieceKeys[piece][to];

    board.bitboards[piece] ^= ( (1ULL << from) | (1ULL << to) );

    board.mailbox[to] = piece;
    board.mailbox[from] = NO_PIECE;
}


inline void move_piece(const int from, const int to) {
    const Piece piece { board.mailbox[from] };
    const Piece capturedPiece { board.mailbox[to] };

    hashKey ^= randomPieceKeys[piece][from] ^ randomPieceKeys[piece][to] ^ randomPieceKeys[capturedPiece][to];

    const U64 mask = (1ULL << from) | (1ULL << to);

    board.bitboards[piece] ^= mask;
    board.bitboards[capturedPiece] &= ~mask;

    board.mailbox[to] = piece;
    board.mailbox[from] = NO_PIECE;
}


/*
// 0 represents illegal move, 1 represents the legal move;
int makeMove(const int move, const int onlyCaptures){

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


        // so this works
         if (getMoveFlags(move) == QUIET ) {
            move_piece_quiet(startSQ, targetSQ);
        }
        else {

          //  move_piece(startSQ, targetSQ);

            // moving the piece
            SET_BIT_FALSE(board.bitboards[piece], startSQ);
            SET_BIT(board.bitboards[piece], targetSQ);

            // hashing the piece
            hashKey ^= randomPieceKeys[piece][startSQ];
            hashKey ^= randomPieceKeys[piece][targetSQ];

        }


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
*/

inline void remove_piece(const int s) {
	hashKey ^= randomPieceKeys[board.mailbox[s]][s];
	board.bitboards[board.mailbox[s]] &= ~(1ULL << s);
	board.mailbox[s] = NO_PIECE;
}

inline void put_piece(Piece pc, int s) {
	board.mailbox[s] = pc;
	board.bitboards[pc] |= (1ULL << s);
	hashKey ^= randomPieceKeys[pc][s];
}

int makeMove(const Move move, const int onlyCaptures) {
	
	if(!onlyCaptures) {
		COPY_BOARD();
		
		////history[game_ply] = UndoInfo(history[game_ply - 1]);

		MoveFlags type = move.flags();
		//history[game_ply].entry |= SQUARE_BB[move.to()] | SQUARE_BB[move.from()];

		const int startSQ { move.from() };
		const int targetSQ { move.to() };

		const Color C { static_cast<Color>(board.side) };

		if (board.enPassantSq != 64) { hashKey ^= randomEnPassantKeys[board.enPassantSq]; }
		board.enPassantSq = 64;

		switch (type) {
			case QUIET:
				//The to square is guaranteed to be empty here
				move_piece_quiet(move.from(), move.to());
			break;
			case DOUBLE_PUSH:
				move_piece_quiet(move.from(), move.to());

				if (board.side == WHITE) {
					board.enPassantSq = targetSQ - 8;
					hashKey ^= randomEnPassantKeys[targetSQ - 8];
				} else {
					board.enPassantSq = targetSQ + 8;
					hashKey ^= randomEnPassantKeys[targetSQ + 8];
				}
			//This is the square behind the pawn that was double-pushed
			//history[game_ply].epsq = move.from() + relative_dir<C>(NORTH);
			break;

			case OO:
				if (board.side == WHITE) {
					move_piece_quiet(E1, G1);
					move_piece_quiet(H1, F1);
				} else {
					move_piece_quiet(E8, G8);
					move_piece_quiet(H8, F8);
				}			
			break;

			case OOO:
				if (board.side == WHITE) {
					move_piece_quiet(E1, C1);
					move_piece_quiet(A1, D1);
				} else {
					move_piece_quiet(E8, C8);
					move_piece_quiet(A8, D8);
				}

			break;
			case EN_PASSANT:
				move_piece_quiet(move.from(), move.to());
				if (board.side == WHITE) {
					SET_BIT_FALSE( board.bitboards[PAWN + 6], targetSQ - 8 );
					hashKey ^= randomPieceKeys[PAWN + 6][targetSQ - 8]; // hash the removal of the opponent's PAWN

				} else {
					SET_BIT_FALSE( board.bitboards[PAWN], targetSQ + 8 );
					hashKey ^= randomPieceKeys[PAWN][targetSQ + 8];
				}
			break;

			case PR_KNIGHT:
				remove_piece(move.from());
				put_piece(make_piece(C, KNIGHT), move.to());
			break;
			case PR_BISHOP:
				remove_piece(move.from());
			put_piece(make_piece(C, BISHOP), move.to());
			break;
			case PR_ROOK:
				remove_piece(move.from());
			put_piece(make_piece(C, ROOK), move.to());
			break;
			case PR_QUEEN:
				remove_piece(move.from());
			put_piece(make_piece(C, QUEEN), move.to());
			break;
			case PC_KNIGHT:
				remove_piece(move.from());
			//history[game_ply].captured = board[move.to()];
			remove_piece(move.to());
			
			put_piece(make_piece(C, KNIGHT), move.to());
			break;
			case PC_BISHOP:
				remove_piece(move.from());
			//history[game_ply].captured = board[move.to()];
			remove_piece(move.to());

			put_piece(make_piece(C, BISHOP), move.to());
			break;
			case PC_ROOK:
				remove_piece(move.from());
			//history[game_ply].captured = board[move.to()];
			remove_piece(move.to());

			put_piece(make_piece(C, ROOK), move.to());
			break;
			case PC_QUEEN:
				remove_piece(move.from());
			//history[game_ply].captured = board[move.to()];
			remove_piece(move.to());

			put_piece(make_piece(C, QUEEN), move.to());
			break;

			case CAPTURE:
			//	history[game_ply].captured = board[move.to()];
			move_piece(move.from(), move.to());
			
			break;
		}
		
		// hash castling
		hashKey ^= randomCastlingKeys[board.castle]; // get trid of castling

		// board.castle bit hack for updating the rights
		board.castle &= castlingRightsConstant[move.from()];
		board.castle &= castlingRightsConstant[move.to()];

		hashKey ^= randomCastlingKeys[board.castle]; // re-insert castling rights

		board.resetOcc();
		for (int bbPiece=0; bbPiece < 6; bbPiece++) {
			board.bitboards[WHITE_OCC] |= board.bitboards[bbPiece]; // for white
			board.bitboards[BLACK_OCC] |= board.bitboards[bbPiece + 6]; // for black
			board.bitboards[BOTH_OCC] |= (board.bitboards[bbPiece] | board.bitboards[bbPiece + 6]); // for both
		}

		board.side ^= 1; // change board.side
		hashKey ^= sideKey;


		// make sure that the king has not been exposed into check
		if ( isSqAttacked(bsf( board.bitboards[KING + 6 * (board.side^1)] ), board.side)) {
			// square is illegal so we take it back
			RESTORE_BOARD();
			return 0;
		}
		return 1; // this is a legal move so we return true
	}
	
	// used in Quiescent search
	if ( move.is_capture() ) {
		return makeMove(move, 0); // make the move
		// forgetting this return statement causes issues within the quiescence search
	}
	return 0;
}

/*
//Undos a move in the current position, rolling it back to the previous position
template<Color C>
void Position::undo(const Move move) {
	MoveFlags type = move.flags();
	switch (type) {
	case QUIET:
		move_piece_quiet(move.to(), move.from());
		break;
	case DOUBLE_PUSH:
		move_piece_quiet(move.to(), move.from());
		break;
	case OO:
		if (C == WHITE) {
			move_piece_quiet(g1, e1);
			move_piece_quiet(f1, h1);
		} else {
			move_piece_quiet(g8, e8);
			move_piece_quiet(f8, h8);
		}
		break;
	case OOO:
		if (C == WHITE) {
			move_piece_quiet(c1, e1);
			move_piece_quiet(d1, a1);
		} else {
			move_piece_quiet(c8, e8);
			move_piece_quiet(d8, a8);
		}
		break;
	case EN_PASSANT:
		move_piece_quiet(move.to(), move.from());
		put_piece(make_piece(~C, PAWN), move.to() + relative_dir<C>(SOUTH));
		break;
	case PR_KNIGHT:
	case PR_BISHOP:
	case PR_ROOK:
	case PR_QUEEN:
		remove_piece(move.to());
		put_piece(make_piece(C, PAWN), move.from());
		break;
	case PC_KNIGHT:
	case PC_BISHOP:
	case PC_ROOK:
	case PC_QUEEN:
		remove_piece(move.to());
		put_piece(make_piece(C, PAWN), move.from());
		put_piece(history[game_ply].captured, move.to());
		break;
	case CAPTURE:
		move_piece_quiet(move.to(), move.from());
		put_piece(history[game_ply].captured, move.to());
		break;
	}

	side_to_play = ~side_to_play;
	--game_ply;
}
*/