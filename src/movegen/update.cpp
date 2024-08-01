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
#include <__chrono/month.h>

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
template <bool UpdateHash>
void Board::movePieceQuiet(const int from, const int to) {
    const Piece piece { mailbox[from] };

	if constexpr (UpdateHash) {
		hashKey ^= randomPieceKeys[piece][from] ^ randomPieceKeys[piece][to];
	}

    bitboards[piece] ^= ( (1ULL << from) | (1ULL << to) );

    mailbox[to] = piece;
    mailbox[from] = NO_PIECE;

}

template <bool UpdateHash>
void Board::movePiece(const int from, const int to) {

	const Piece piece { mailbox[from] };
	const Piece capturedPiece { mailbox[to] };

	if constexpr (UpdateHash) {
		hashKey ^= randomPieceKeys[piece][from] ^ randomPieceKeys[piece][to] ^ randomPieceKeys[capturedPiece][to];
	}

	const U64 mask = (1ULL << from) | (1ULL << to);

	bitboards[piece] ^= mask;
	bitboards[capturedPiece] &= ~mask;

	mailbox[to] = piece;
	mailbox[from] = NO_PIECE;
}

template <bool UpdateHash>
void Board::putPiece(const Piece pc, const int s) {
	mailbox[s] = pc;
	bitboards[pc] |= (1ULL << s);
	if constexpr (UpdateHash) {
		hashKey ^= randomPieceKeys[pc][s];
	}
}

template <bool UpdateHash>
void Board::removePiece(const int s) {
	// for now just checking
	if constexpr (UpdateHash) {
		hashKey ^= randomPieceKeys[mailbox[s]][s];
	}
	bitboards[mailbox[s]] &= ~(1ULL << s);
	mailbox[s] = NO_PIECE;
}

//Undos a move in the current position, rolling it back to the previous position
void Board::undo(const Move move) {
	MoveFlags type = move.flags();
	const Color C { static_cast<Color>(side) };

	switch (type) {
		case QUIET:
			movePieceQuiet<false>(move.to(), move.from());
		break;
		case DOUBLE_PUSH:
			movePieceQuiet<false>(move.to(), move.from());
		break;
		case OO:
			if (C == BLACK) {
				movePieceQuiet<false>(G1, E1);
				movePieceQuiet<false>(F1, H1);
			} else {
				movePieceQuiet<false>(G8, E8);
				movePieceQuiet<false>(F8, H8);
			}
		break;
		case OOO:
			if (C == BLACK) {
				movePieceQuiet<false>(C1, E1);
				movePieceQuiet<false>(D1, A1);
			} else {
				movePieceQuiet<false>(C8, E8);
				movePieceQuiet<false>(D8, A8);
			}
		break;
		case EN_PASSANT:
			movePieceQuiet<false>(move.to(), move.from());
			putPiece<false>(make_piece(C, PAWN), move.to() + ((C == WHITE) ? 8 : -8));

		break;
		case PR_KNIGHT:
		case PR_BISHOP:
		case PR_ROOK:
		case PR_QUEEN:
			removePiece<false>(move.to());
		    putPiece<false>(make_piece(~C, PAWN), move.from());

		break;
		case PC_KNIGHT:
		case PC_BISHOP:
		case PC_ROOK:
		case PC_QUEEN:
			removePiece<false>(move.to());
			putPiece<false>(make_piece(~C, PAWN), move.from());
			putPiece<false>(history[gamePly].captured, move.to());
		break;
		case CAPTURE:
			movePieceQuiet<false>(move.to(), move.from());
			putPiece<false>(history[gamePly].captured, move.to());
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

		COPY_HASH();

		if (history[gamePly].enPassSq != 64) { hashKey ^= randomEnPassantKeys[history[gamePly].enPassSq]; }

		side ^= 1; // change side
		hashKey ^= sideKey;

		++gamePly;
		history[gamePly] = UndoInfo(history[gamePly - 1]);

		const MoveFlags type = move.flags();
		const auto C { static_cast<Color>(side) };

		switch (type) {
			case QUIET:
				//The to square is guaranteed to be empty here
				movePieceQuiet<true>(move.from(), move.to());
			break;

			case DOUBLE_PUSH:
				movePieceQuiet<true>(move.from(), move.to());

				if (C == WHITE) {
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
					movePieceQuiet<true>(E1, G1);
					movePieceQuiet<true>(H1, F1);
				} else {
					movePieceQuiet<true>(E8, G8);
					movePieceQuiet<true>(H8, F8);
				}			
				break;

			case OOO:
				if (C == BLACK) {
					movePieceQuiet<true>(E1, C1);
					movePieceQuiet<true>(A1, D1);
				} else {
					movePieceQuiet<true>(E8, C8);
					movePieceQuiet<true>(A8, D8);
				}
				break;

			case EN_PASSANT:
				movePieceQuiet<true>(move.from(), move.to());
				if (C == WHITE) {
					removePiece<true>(move.to() + 8);
				} else {
					removePiece<true>(move.to() - 8);
				}
			break;

			case PR_KNIGHT:
				removePiece<true>(move.from());
				putPiece<true>(make_piece(~C, KNIGHT), move.to());
			break;
			case PR_BISHOP:
				removePiece<true>(move.from());
			putPiece<true>(make_piece(~C, BISHOP), move.to());
			break;
			case PR_ROOK:
				removePiece<true>(move.from());
			putPiece<true>(make_piece(~C, ROOK), move.to());
			break;
			case PR_QUEEN:
				removePiece<true>(move.from());
			putPiece<true>(make_piece(~C, QUEEN), move.to());
			break;
			case PC_KNIGHT:
				removePiece<true>(move.from());
			history[gamePly].captured = mailbox[move.to()];
			removePiece<true>(move.to());

			putPiece<true>(make_piece(~C, KNIGHT), move.to());
			break;
			case PC_BISHOP:
				removePiece<true>(move.from());
			history[gamePly].captured = mailbox[move.to()];
			removePiece<true>(move.to());

			putPiece<true>(make_piece(~C, BISHOP), move.to());
			break;
			case PC_ROOK:
				removePiece<true>(move.from());
			history[gamePly].captured = mailbox[move.to()];
			removePiece<true>(move.to());

			putPiece<true>(make_piece(~C, ROOK), move.to());
			break;
			case PC_QUEEN:
				removePiece<true>(move.from());
			history[gamePly].captured = mailbox[move.to()];
			removePiece<true>(move.to());

			putPiece<true>(make_piece(~C, QUEEN), move.to());
			break;

			case CAPTURE:
			history[gamePly].captured = mailbox[move.to()];
			movePiece<true>(move.from(), move.to());
			
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
			RESTORE_HASH();

			return 0;
		}

		/*
		FOR ZOBRIST DEBUGGING PURPOSES from Marksim Kozh
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
	if ( move.isCapture() ) {
		return makeMove(move, 0); // make the move
		// forgetting this return statement causes issues within the quiescence search
	}
	return 0;
}


void Board::nullMove() {
	hashKey ^= sideKey;
	if (board.history[board.gamePly].enPassSq != 64) hashKey ^= randomEnPassantKeys[board.history[board.gamePly].enPassSq];

	board.side ^= 1; // make null move
	board.gamePly++;
	board.history[board.gamePly] = UndoInfo(board.history[board.gamePly - 1]);
}

void Board::undoNullMove() {
	board.side ^= 1;
	board.gamePly--;
}

