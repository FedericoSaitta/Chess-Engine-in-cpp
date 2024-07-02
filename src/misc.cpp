//
// Created by Federico Saitta on 29/06/2024.
//

#include "misc.h"

#include "globals.h"
#include "constants.h"
#include "inline_functions.h"

#include <iostream>

// File used for miscellanous functions such as a simple GUI and printing out bitboards etc,
// these could go under tests but there we will put more rigorous testing functions

const char asciiPieces[] { "PNBRQKpkbrqk" };
const int charPieces[] = { ['P'] = 0, ['N'] = 1, ['B'] = 2, ['R'] = 3, ['Q'] = 4, ['K'] = 5, // white
                           ['p'] = 6, ['n'] = 7, ['b'] = 8, ['r'] = 9, ['q'] = 10, ['k'] = 11}; // black
const char* unicodePieces[] { "♟", "♞", "♝", "♜", "♛", "♚", // White
                              "♙", "♘", "♗", "♖", "♕", "♔"}; // Black

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

// prints the bitboard, run bb through mirrorHorizontal to obtain normal view of the board (A1: bottom left)
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

// this should also print the castling rights and the en - passant square and the side to move
void printBoardFancy() { // this will always be the right way around, doesnt work on windows

    // we first have to mirror all our bitboards

    //before you were mirroring in place, very problematic!!!!
    BITBOARD mirrorredBB[12]{};
    for (int bbPiece=0; bbPiece < 12; bbPiece++) {
        if (bitboards[bbPiece]) {
            mirrorredBB[bbPiece] = mirrorHorizontal(bitboards[bbPiece]);
        }
    }

    const char* emptySymbol = ".";

    for (int square=63; square >= 0; --square) {
        if ((square + 1) % 8 == 0) std::cout << '\n' << (square + 1) / 8 << "| ";

        int piece { -1 };

        for (int bbPiece=0; bbPiece < 12; bbPiece++) {

            if ( getBit(mirrorredBB[bbPiece], square) ) {
                piece = bbPiece;
                break;
            }
        }

        const char* symbol{ (piece == -1) ? emptySymbol : unicodePieces[piece] };
        std::cout << ' ' << symbol << ' ';
    }
    std::cout << '\n';
    std::cout << "    A  B  C  D  E  F  G  H \n";
    std::cout << std::boolalpha;
    std::cout << "White to move: "<< !side << ", Castling: " << std::bitset<4>(castle)
              << ", En Passant: " << chessBoard[enPassantSQ] << '\n';
}

void printAttackedSquares(int side) {
    std::cout << '\n';
    for (int square = 63; square >= 0; --square) {
        if ((square + 1) % 8 == 0){ std::cout << (1 + square / 8) << "| "; }
        
        std::cout << isSqAttacked(square, side) << ' ';
        if (square % 8 == 0){ std::cout << '\n'; }
    }
    std::cout << "   H G F E D C B A";
    std::cout << '\n';
}
