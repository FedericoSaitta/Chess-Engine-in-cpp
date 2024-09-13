#include "../../chess/board.h"
#include <assert.h>
#include <variant>

#include "../../chess/types.h"
#include "../../include/hashtable.h"

#include "config.h"
#include "../../chess/inline_functions.h"
#include "../misc.h"
#include "game_statistics.h"

#include "bit_operations.h"

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
void Board::movePieceQuiet(const int from, const int to) {

	assert((mailbox[from] != NO_PIECE) && "movePieceQuiet: start square is empty");
	assert((mailbox[to] == NO_PIECE) && "movePieceQuiet: out of bounds capture hashkey indexing");

    const Piece piece { mailbox[from] };

	hashkey ^= randomPieceKeys[piece][from] ^ randomPieceKeys[piece][to];

    bitboards[piece] ^= ( (1ULL << from) | (1ULL << to) );

    mailbox[to] = piece;
    mailbox[from] = NO_PIECE;

}

void Board::movePiece(const int from, const int to) {

	assert((mailbox[from] != NO_PIECE) && "movePiece: start square is empty");
	assert((mailbox[to] != NO_PIECE) && "movePiece: out of bounds capture hashkey indexing");

	const Piece piece { mailbox[from] };
	const Piece capturedPiece { mailbox[to] };

	hashkey ^= randomPieceKeys[piece][from] ^ randomPieceKeys[piece][to] ^ randomPieceKeys[capturedPiece][to];

	const U64 mask = (1ULL << from) | (1ULL << to);

	bitboards[piece] ^= mask;
	bitboards[capturedPiece] &= ~mask;

	mailbox[to] = piece;
	mailbox[from] = NO_PIECE;
}

void Board::putPiece(const Piece pc, const int s) {

	assert((pc != NO_PIECE) && "putPiece: piece is NO_PIECE");

	mailbox[s] = pc;
	bitboards[pc] |= (1ULL << s);
	hashkey ^= randomPieceKeys[pc][s];

}

void Board::removePiece(const int s) {

	assert((mailbox[s] != NO_PIECE) && "removePiece: piece is NO_PIECE");

	hashkey ^= randomPieceKeys[mailbox[s]][s];

	bitboards[mailbox[s]] &= ~(1ULL << s);
	mailbox[s] = NO_PIECE;
}


int Board::makeMove(const Move move, const int onlyCaptures) {

	assert(!move.isNone() && "makeMove: making a NULL MOVE");
	assert(move.to() != move.from() && "makeMove: start and end-square are same");

	if(!onlyCaptures) {
		nodes++;
		// should be put at the bottom of this
		const Board copyBoard { *this };

		assert(*this == copyBoard && "Two boards arent equal");

		if (enPassSq != 64) { hashkey ^= randomEnPassantKeys[enPassSq]; }
		enPassSq = 64;

		side ^= 1; // change side
		hashkey ^= sideKey;

		const MoveFlags type = move.flags();
		const auto C { static_cast<Color>(side) };

		switch (type) {
			case QUIET:
				//The to square is guaranteed to be empty here
				movePieceQuiet(move.from(), move.to());
			break;

			case DOUBLE_PUSH:
				movePieceQuiet(move.from(), move.to());

				if (C == WHITE) {
					enPassSq = move.to() + 8;
					hashkey ^= randomEnPassantKeys[move.to() + 8];

				} else {
					enPassSq = move.to() - 8;
					hashkey ^= randomEnPassantKeys[move.to() - 8];
				}
			//This is the square behind the pawn that was double-pushed

			break;

			case OO:
				if (C == BLACK) {
					movePieceQuiet(E1, G1);
					movePieceQuiet(H1, F1);
				} else {
					movePieceQuiet(E8, G8);
					movePieceQuiet(H8, F8);
				}			
				break;

			case OOO:
				if (C == BLACK) {
					movePieceQuiet(E1, C1);
					movePieceQuiet(A1, D1);
				} else {
					movePieceQuiet(E8, C8);
					movePieceQuiet(A8, D8);
				}
				break;

			case EN_PASSANT:
				movePieceQuiet(move.from(), move.to());
				if (C == WHITE) {
					removePiece(move.to() + 8);
				} else {
					removePiece(move.to() - 8);
				}
			break;

			case PR_KNIGHT:
				removePiece(move.from());
				putPiece(make_piece(~C, KNIGHT), move.to());
			break;
			case PR_BISHOP:
				removePiece(move.from());
			putPiece(make_piece(~C, BISHOP), move.to());
			break;
			case PR_ROOK:
				removePiece(move.from());
			putPiece(make_piece(~C, ROOK), move.to());
			break;
			case PR_QUEEN:
				removePiece(move.from());
			putPiece(make_piece(~C, QUEEN), move.to());
			break;
			case PC_KNIGHT:
				removePiece(move.from());
			
			removePiece(move.to());

			putPiece(make_piece(~C, KNIGHT), move.to());
			break;
			case PC_BISHOP:
				removePiece(move.from());
			
			removePiece(move.to());

			putPiece(make_piece(~C, BISHOP), move.to());
			break;
			case PC_ROOK:
				removePiece(move.from());
			
			removePiece(move.to());

			putPiece(make_piece(~C, ROOK), move.to());
			break;
			case PC_QUEEN:
				removePiece(move.from());
			
			removePiece(move.to());

			putPiece(make_piece(~C, QUEEN), move.to());
			break;

			case CAPTURE:
			
			movePiece(move.from(), move.to());
			
			break;
		}

		// hash castling
		hashkey ^= randomCastlingKeys[castle]; // get trid of castling

		// castle bit hack for updating the rights
		castle &= castlingRightsConstant[move.from()];
		castle &= castlingRightsConstant[move.to()];

		hashkey ^= randomCastlingKeys[castle]; // re-insert castling rights

		resetOcc();
		for (int bbPiece=0; bbPiece < 6; bbPiece++) {
			bitboards[WHITE_OCC] |= bitboards[bbPiece]; // for white
			bitboards[BLACK_OCC] |= bitboards[bbPiece + 6]; // for black
		}

		bitboards[BOTH_OCC] |= (bitboards[WHITE_OCC] | bitboards[BLACK_OCC]); // for both

		// Zobrist Debug test from Maksim Korzh
		assert((generatehashkey(*this) == hashkey) && "makeMove: hashkey is wrong");

		// make sure that the king has not been exposed into check
		if ( isSqAttacked(bsf( bitboards[KING + 6 * (side^1)] ), side)) {
			// square is illegal so we take it back
			*this = copyBoard;

			return 0;
		}

		return 1; // this is a legal move so we return true
	}
	
	// used in Quiescent search
	if ( move.isCapture() || move.isQueenPromotion() ) {
		return makeMove(move, 0); // make the move
		// forgetting this return statement causes issues within the quiescence search
	}
	return 0;
}


void Board::nullMove() {
	hashkey ^= sideKey;
	if (enPassSq != 64) hashkey ^= randomEnPassantKeys[enPassSq];
	enPassSq = 64;
	side ^= 1; // make null move

	assert((generatehashkey(*this) == hashkey) && "nullMove: hashkey is wrong");
}

