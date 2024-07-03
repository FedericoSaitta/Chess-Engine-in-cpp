//
// Created by Federico Saitta on 29/06/2024.
//

#include "misc.h"

#include "globals.h"
#include "constants.h"
#include "inline_functions.h"

#include <iostream>

// File used for miscellanous functions such as a simple GUI and printing out bitboards etc,
// these could go under tests but I will put more rigorous testing functions there.



// from https://www.chessprogramming.org/Flipping_Mirroring_and_Rotating#Mirror_Horizontally
BITBOARD mirrorHorizontal (BITBOARD bb) {
    constexpr U64 k1 = 0x5555555555555555;
    constexpr U64 k2 = 0x3333333333333333;
    constexpr U64 k4 = 0x0f0f0f0f0f0f0f0f;
    bb = ((bb >> 1) & k1) | ((bb & k1) << 1);
    bb = ((bb >> 2) & k2) | ((bb & k2) << 2);
    bb = ((bb >> 4) & k4) | ((bb & k4) << 4);

    return bb;
}

void printBitBoard(const BITBOARD bb, const bool mirrored) {
    std::cout << '\n';
    for (int square = 63; square >= 0; --square) {
        if ((square + 1) % 8 == 0){ std::cout << (1 + square / 8) << "| "; }

        std::cout << ((bb >> square) & 1) << ' ';
        if (square % 8 == 0){ std::cout << '\n'; }
    }

    if (mirrored) { std::cout << "   A B C D E F G H"; }
    else { std::cout << "   H G F E D C B A"; }
    std::cout << '\n';
}


void printBoardFancy() { // this will always be the right way around, doesnt work on windows

    // we first have to mirror all our bitboards

    //before you were mirroring in place, very problematic!!!!
    BITBOARD mirrorredBB[12]{};
    for (int piece=0; piece < 12; piece++) {
        if (bitboards[piece]) {
            mirrorredBB[piece] = mirrorHorizontal(bitboards[piece]);
        }
    }

    
    for (int square=63; square >= 0; --square) {
        if ((square + 1) % 8 == 0) std::cout << '\n' << (square + 1) / 8 << "| ";

        int piece { -1 };

        for (int bbPiece=0; bbPiece < 12; bbPiece++) {

            if ( getBit(mirrorredBB[bbPiece], square) ) {
                piece = bbPiece;
                break;
            }
        }

        const char* emptySymbol = ".";
        const char* symbol{ (piece == -1) ? emptySymbol : unicodePieces[piece] };
        std::cout << ' ' << symbol << ' ';
    }
    std::cout << '\n';
    std::cout << "    A  B  C  D  E  F  G  H \n";
    std::cout << std::boolalpha;
    std::cout << "White to move: "<< !side << ", Castling: " << std::bitset<4>(castle)
              << ", En Passant: " << chessBoard[enPassantSQ] << '\n';
}

void printAttackedSquares(const int side) {
    std::cout << '\n';
    for (int square = 63; square >= 0; --square) {
        if ((square + 1) % 8 == 0){ std::cout << (1 + square / 8) << "| "; }
        
        std::cout << isSqAttacked(square, side) << ' ';
        if (square % 8 == 0){ std::cout << '\n'; }
    }
    std::cout << "   H G F E D C B A";
    std::cout << '\n';
}

// for UCI protocol
void printMove(const int move) {
    std::printf("%s%s%c\n", chessBoard[getMoveStartSQ(move)],
                          chessBoard[getMoveTargettSQ(move)],
                          promotedPieces[getMovePromPiece(move)] );
}

// mainly for debugging purposes
void printMovesList(const MoveList& moveList) {
    for (int moveCount = 0; moveCount < moveList.count; moveCount++) {
        const int move {moveList.moves[moveCount]};
        printMove(move);

    }
}
