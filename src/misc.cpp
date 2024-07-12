//
// Created by Federico Saitta on 29/06/2024.
//

#include "misc.h"
#include "board.h"
#include "hashtable.h"
#include "macros.h"
#include "inline_functions.h"
#include <iostream>
#include <vector>
#include <sstream>

// File used for miscellanous functions such as a simple GUI and printing out bitboards etc,
// these could go under benchmark_tests but I will put more rigorous testing functions there.


const char promotedPieces[] = { [Queen] = 'q', [Rook] = 'r', [Bishop] = 'b', [Knight] = 'n',
                            [Queen + 6] = 'q', [Rook + 6] = 'r', [Bishop + 6] = 'b', [Knight + 6] = 'n',
                            [0] =  ' ' }; // these are always lowercase for both colors

static const char* unicodePieces[] { "♟", "♞", "♝", "♜", "♛", "♚", // White
                              "♙", "♘", "♗", "♖", "♕", "♔"}; // Black

// from https://www.chessprogramming.org/Flipping_Mirroring_and_Rotating#Mirror_Horizontally
static U64 mirrorHorizontal (U64 bb) {
    constexpr U64 k1 = 0x5555555555555555;
    constexpr U64 k2 = 0x3333333333333333;
    constexpr U64 k4 = 0x0f0f0f0f0f0f0f0f;
    bb = ((bb >> 1) & k1) | ((bb & k1) << 1);
    bb = ((bb >> 2) & k2) | ((bb & k2) << 2);
    bb = ((bb >> 4) & k4) | ((bb & k4) << 4);

    return bb;
}

void printBitBoard(const U64 bb, const bool mirrored) {
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



static constexpr char castlePieces[4] = {'K', 'Q', 'k', 'q'};
static constexpr std::string_view playingSides[2] = {"White", "Black"};
void printBoardFancy() { // this will always be the right way around, doesnt work on windows

    // we first have to mirror all our bitboards

    //before you were mirroring in place, very problematic!!!!
    U64 mirrorredBB[12]{};
    for (int piece=0; piece < 12; piece++) {
        if (bitboards[piece]) {
            mirrorredBB[piece] = mirrorHorizontal(bitboards[piece]);
        }
    }

    for (int square=63; square >= 0; --square) {
        if ((square + 1) % 8 == 0) std::cout << '\n' << (square + 1) / 8 << "| ";

        int piece { -1 };

        for (int bbPiece=0; bbPiece < 12; bbPiece++) {

            if ( GET_BIT(mirrorredBB[bbPiece], square) ) {
                piece = bbPiece;
                break;
            }
        }

        const char* symbol{ (piece == -1) ? "." : unicodePieces[piece] };
        std::cout << ' ' << symbol << ' ';
    }

    std::string castleRightsString{};
    for (int i = 0; i < 4; ++i) {
        if (castle & (1 << i)) { castleRightsString += castlePieces[i]; }
    }

    std::cout << "\n    A  B  C  D  E  F  G  H \n";
    std::cout << playingSides[side] << " to move, Castling: " << castleRightsString
              << ", En Passant: " << chessBoard[enPassantSQ] << '\n';
    std::cout << "HashKey: " << hashKey << '\n';
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
    std::printf("%s%s%c", chessBoard[getMoveStartSQ(move)],
                          chessBoard[getMoveTargetSQ(move)],
                          promotedPieces[getMovePromPiece(move)] );
}

std::string algebraicNotation(const int move) {
    std::string a { chessBoard[getMoveStartSQ(move)] };
    std::string b { chessBoard[getMoveTargetSQ(move)]};
    std::string c{ promotedPieces[getMovePromPiece(move)]};

    return a + b + c;
}


// mainly for debugging purposes
void printMovesList(const MoveList& moveList) {
    std::cout << "Move  Piece PromPiece Capture DoublePush EnPassant Castling \n";

    for (int moveCount = 0; moveCount < moveList.count; moveCount++) {
        const int move {moveList.moves[moveCount]};
        std::printf("%s%s%c   ", chessBoard[getMoveStartSQ(move)],
                          chessBoard[getMoveTargetSQ(move)],
                          promotedPieces[getMovePromPiece(move)] );

        std::cout << ( unicodePieces[getMovePiece(move)] ) << "      ";
        std::cout << ( ((getMovePromPiece(move) != 0)) ? unicodePieces[getMovePromPiece(move)] : "0" )<< "         ";
        std::cout << ((getMoveCapture(move) > 0) ? 1 : 0) << "        ";
        std::cout << ((getMoveDoublePush(move) > 0) ? 1 : 0) << "          ";
        std::cout << ((getMoveEnPassant(move) > 0) ? 1 : 0) << "         ";
        std::cout << ((getMoveCastling(move) > 0) ? 1 : 0);
        std::cout << '\n';
    }
}

std::vector<std::string> split(const std::string& str) {
    std::vector<std::string> tokens;
    std::istringstream iss(str);
    std::string token;
    while (iss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}
