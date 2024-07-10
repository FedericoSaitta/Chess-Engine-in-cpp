//
// Created by Federico Saitta on 10/07/2024.
//

#ifndef BOARD_H
#define BOARD_H

#include <iostream>

#include "macros.h"
extern const char* chessBoard[65];

void parseFEN(const std::string& fenString);

extern U64 bitboards[12]; // Pawn, Knight, Bishop, Rook, Queen, King, p, n, b, r, q, k
extern U64 occupancies[3]; // white, black, both sides combined

extern int side; // side to move
extern int enPassantSQ; // index from 0 to 64, 64 represents no en-passant is on the board
extern int castle; // 4-bit integer of the form qkQK, if bit is turned on then castling is available



#endif //BOARD_H
