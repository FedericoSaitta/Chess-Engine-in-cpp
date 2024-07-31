//
// Created by Federico Saitta on 28/06/2024.
// deals with uupdating the board after a move is made, also has undo move function
#include "update.h"

#include "../../include/macros.h"
#include "types.h"
#include <assert.h>
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


//Moves a piece to an empty square. Note that it is an error if the <to> square contains a piece
void Board::move_piece_quiet(const int from, const int to) {
    const Piece piece { mailbox[from] };

    hashKey ^= randomPieceKeys[piece][from] ^ randomPieceKeys[piece][to];

    bitboards[piece] ^= ( (1ULL << from) | (1ULL << to) );

    mailbox[to] = piece;
    mailbox[from] = NO_PIECE;

}

void Board::move_piece(const int from, const int to) {
	
	const Piece piece { mailbox[from] };
	const Piece capturedPiece { mailbox[to] };

	hashKey ^= randomPieceKeys[piece][from] ^ randomPieceKeys[piece][to] ^ randomPieceKeys[capturedPiece][to];

	const U64 mask = (1ULL << from) | (1ULL << to);

	bitboards[piece] ^= mask;
	bitboards[capturedPiece] &= ~mask;

	mailbox[to] = piece;
	mailbox[from] = NO_PIECE;
}

void Board::put_piece(Piece pc, int s) {
	mailbox[s] = pc;
	bitboards[pc] |= (1ULL << s);
	hashKey ^= randomPieceKeys[pc][s];
}

void Board::remove_piece(const int s) {
	// for now just checking
	hashKey ^= randomPieceKeys[mailbox[s]][s];
	bitboards[mailbox[s]] &= ~(1ULL << s);
	mailbox[s] = NO_PIECE;

}

//Undos a move in the current position, rolling it back to the previous position
void Board::undo(const Move move) {
	MoveFlags type = move.flags();
	const Color C { static_cast<Color>(side) };
	const Color Opponent { static_cast<Color>(side^1) };

	switch (type) {
		case QUIET:
			move_piece_quiet(move.to(), move.from());
		break;
		case DOUBLE_PUSH:
			move_piece_quiet(move.to(), move.from());
		break;
		case OO:
			if (C == BLACK) {
				move_piece_quiet(G1, E1);
				move_piece_quiet(F1, H1);
			} else {
				move_piece_quiet(G8, E8);
				move_piece_quiet(F8, H8);
			}
		break;
		case OOO:
			if (C == BLACK) {
				move_piece_quiet(C1, E1);
				move_piece_quiet(D1, A1);
			} else {
				move_piece_quiet(C8, E8);
				move_piece_quiet(D8, A8);
			}
		break;
		case EN_PASSANT:
			move_piece_quiet(move.to(), move.from());
			put_piece(make_piece(C, PAWN), move.to() + ((C == WHITE) ? 8 : -8));

		break;
		case PR_KNIGHT:
		case PR_BISHOP:
		case PR_ROOK:
		case PR_QUEEN:
			remove_piece(move.to());
		    put_piece(make_piece(~C, PAWN), move.from());

		break;
		case PC_KNIGHT:
		case PC_BISHOP:
		case PC_ROOK:
		case PC_QUEEN:
			remove_piece(move.to());
			put_piece(make_piece(~C, PAWN), move.from());
			put_piece(history[gamePly].captured, move.to());
		break;
		case CAPTURE:
			move_piece_quiet(move.to(), move.from());
			put_piece(history[gamePly].captured, move.to());
		break;
	}

	resetOcc();
	for (int bbPiece=0; bbPiece < 6; bbPiece++) {
		bitboards[WHITE_OCC] |= bitboards[bbPiece]; // for white
		bitboards[BLACK_OCC] |= bitboards[bbPiece + 6]; // for black
		bitboards[BOTH_OCC] |= (bitboards[bbPiece] | bitboards[bbPiece + 6]); // for both
	}

	side ^= 1;
	--gamePly;
}

int Board::makeMove(const Move move, const int onlyCaptures) {
	
	if(!onlyCaptures) {
		COPY_BOARD();
		if (history[gamePly].enPassSq != 64) { hashKey ^= randomEnPassantKeys[history[gamePly].enPassSq]; }

		// these is something funky going on with this
		side ^= 1; // change side
		hashKey ^= sideKey;

		++gamePly;
		history[gamePly] = UndoInfo(history[gamePly - 1]);

		MoveFlags type = move.flags();

		const Color C { static_cast<Color>(side) };
		const Color sideWhoMoved { static_cast<Color>(side^1) };

		switch (type) {
			case QUIET:
				//The to square is guaranteed to be empty here
				move_piece_quiet(move.from(), move.to());
			break;

			case DOUBLE_PUSH:
				move_piece_quiet(move.from(), move.to());

				if (side == WHITE) {
					history[gamePly].enPassSq = move.to() + 8;
					hashKey ^= randomEnPassantKeys[move.to() + 8];

				} else {
					history[gamePly].enPassSq = move.to() - 8;
					hashKey ^= randomEnPassantKeys[move.to() - 8];
				}
			//This is the square behind the pawn that was double-pushed

			break;

			case OO:
				if (C == BLACK) {
					move_piece_quiet(E1, G1);
					move_piece_quiet(H1, F1);
				} else {
					move_piece_quiet(E8, G8);
					move_piece_quiet(H8, F8);
				}			
				break;

			case OOO:
				if (C == BLACK) {
					move_piece_quiet(E1, C1);
					move_piece_quiet(A1, D1);
				} else {
					move_piece_quiet(E8, C8);
					move_piece_quiet(A8, D8);
				}
				break;

			case EN_PASSANT:
				move_piece_quiet(move.from(), move.to());
				if (side == WHITE) {
					remove_piece(move.to() + 8);
					hashKey ^= randomPieceKeys[PAWN + 6][move.to() + 8]; // hash the removal of the opponent's PAWN

				} else {
					remove_piece(move.to() - 8);
					hashKey ^= randomPieceKeys[PAWN][move.to() - 8];
				}
			break;

			case PR_KNIGHT:
				remove_piece(move.from());
				put_piece(make_piece(~C, KNIGHT), move.to());
			break;
			case PR_BISHOP:
				remove_piece(move.from());
			put_piece(make_piece(~C, BISHOP), move.to());
			break;
			case PR_ROOK:
				remove_piece(move.from());
			put_piece(make_piece(~C, ROOK), move.to());
			break;
			case PR_QUEEN:
				remove_piece(move.from());
			put_piece(make_piece(~C, QUEEN), move.to());
			break;
			case PC_KNIGHT:
				remove_piece(move.from());
			history[gamePly].captured = mailbox[move.to()];
			remove_piece(move.to());

			put_piece(make_piece(~C, KNIGHT), move.to());
			break;
			case PC_BISHOP:
				remove_piece(move.from());
			history[gamePly].captured = mailbox[move.to()];
			remove_piece(move.to());

			put_piece(make_piece(~C, BISHOP), move.to());
			break;
			case PC_ROOK:
				remove_piece(move.from());
			history[gamePly].captured = mailbox[move.to()];
			remove_piece(move.to());

			put_piece(make_piece(~C, ROOK), move.to());
			break;
			case PC_QUEEN:
				remove_piece(move.from());
			history[gamePly].captured = mailbox[move.to()];
			remove_piece(move.to());

			put_piece(make_piece(~C, QUEEN), move.to());
			break;

			case CAPTURE:
			history[gamePly].captured = mailbox[move.to()];
			move_piece(move.from(), move.to());
			
			break;
		}

		// hash castling
		hashKey ^= randomCastlingKeys[history[gamePly].castle]; // get trid of castling

		// castle bit hack for updating the rights
		history[gamePly].castle &= castlingRightsConstant[move.from()];
		history[gamePly].castle &= castlingRightsConstant[move.to()];

		hashKey ^= randomCastlingKeys[history[gamePly].castle]; // re-insert castling rights

		resetOcc();
		for (int bbPiece=0; bbPiece < 6; bbPiece++) {
			bitboards[WHITE_OCC] |= bitboards[bbPiece]; // for white
			bitboards[BLACK_OCC] |= bitboards[bbPiece + 6]; // for black
			bitboards[BOTH_OCC] |= (bitboards[bbPiece] | bitboards[bbPiece + 6]); // for both
		}

		// make sure that the king has not been exposed into check
		if ( isSqAttacked(bsf( bitboards[KING + 6 * (side^1)] ), side)) {
			// square is illegal so we take it back
			undo(move);
			RESTORE_BOARD();

			return 0;
		}

		/*
		U64 new_hasKey = generateHashKey();
		// if this doesnt match we output an error
		if (new_hasKey != hashKey) {
			printMove(move);
			printBoardFancy();
			std::cout << "This does not match, true hash is: " << new_hasKey << '\n';
		}
		*/

		return 1; // this is a legal move so we return true
	}
	
	// used in Quiescent search
	if ( move.is_capture() ) {
		return makeMove(move, 0); // make the move
		// forgetting this return statement causes issues within the quiescence search
	}
	return 0;
}
