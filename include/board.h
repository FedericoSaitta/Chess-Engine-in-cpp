//
// Created by Federico Saitta on 10/07/2024.
//

#pragma once


#include <iostream>
#include "macros.h"
#include "types.h"

extern const char* chessBoard[65];

extern Board board;

void parseFEN(const std::string& fenString);

#define COPY_BOARD()             \
Board boardCopy = board;         \
U64 hashKeyCopy = hashKey;       \

// restore board state
#define RESTORE_BOARD()       \
board = boardCopy;            \
hashKey = hashKeyCopy;