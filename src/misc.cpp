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
#include <stdio.h>

// File used for miscellanous functions such as a simple GUI and printing out bitboards etc,
// these could go under benchmark_tests but I will put more rigorous testing functions there.


const char promotedPieces[] { ' ', 'n', 'b', 'r', 'q', ' ',   // for white pieces
                              ' ', 'n', 'b', 'r', 'q', ' ' }; // for black pieces

#if defined(__unix__) || defined(__APPLE__)

const char* unicodePieces[] { "♟", "♞", "♝", "♜", "♛", "♚", // White
                              "♙", "♘", "♗", "♖", "♕", "♔"}; // Black

#elif defined(_WIN32)
const char* unicodePieces[] { "P", "N", "B", "R", "Q", "K", // White
                              "p", "n", "b", "r", "q", "k"}; // Black
#endif


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
    Piece mirrorredMailBox[64] {};

    for (int row = 0; row < 8; ++row) {
        int start = row * 8;  // Start index of the current row
        int end = start + 7;  // End index of the current row

        // Swap elements to reverse the row
        while (start < end) {
            // Swap elements at 'start' and 'end'
            mirrorredMailBox[start] = board.mailbox[end];
            mirrorredMailBox[end] = board.mailbox[start];

            ++start;
            --end;
        }
    }

    for (int square=63; square >= 0; square--) {
        if ((square + 1) % 8 == 0) std::cout << '\n' << (square + 1) / 8 << "| ";

        const Piece piece { mirrorredMailBox[square] };

        const char* symbol{ (piece == NO_PIECE) ? "." : unicodePieces[piece] };
        std::cout << ' ' << symbol << ' ';
    }

    std::string castleRightsString{};
    for (int i = 0; i < 4; ++i) {
        if (board.history[board.gamePly].castle & (1 << i)) { castleRightsString += castlePieces[i]; }
    }

    std::cout << "\n    A  B  C  D  E  F  G  H \n";
    std::cout << playingSides[board.side] << " to move, Castling: " << castleRightsString
              << ", En Passant: " << chessBoard[board.history[board.gamePly].enPassSq] << '\n';
    std::cout << "HashKey: " << hashKey << std::endl;
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
void printMove(const Move move) {
    std::printf("%s%s%c ", chessBoard[move.from()],
                          chessBoard[move.to()],
                          promotedPieces[move.promotionPiece()] );
    std::cout << std::bitset<4> (move.flags()) << "   " << getStringFlags(move.flags());
}

std::string algebraicNotation(const Move move) {
    std::string a { chessBoard[move.from()] };
    std::string b { chessBoard[move.to()]};
    std::string c{ promotedPieces[move.promotionPiece()]};

    return a + b + c;
}


std::string getStringFlags(const MoveFlags flag) {
    switch (flag) {
        case QUIET: return "QUIET      ";
        case DOUBLE_PUSH: return "DOUBLE PUSH";
        case OO: return "OO         ";
        case OOO: return "OOO        ";

        case PR_KNIGHT: return "PR_KNIGHT  ";
        case PR_BISHOP: return "PR_BISHOP  ";
        case PR_ROOK: return "PR_ROOK    ";
        case PR_QUEEN: return "PR_QUEEN   ";

        case EN_PASSANT: return "EN_PASSANT ";

        case PC_KNIGHT: return "PC_KNIGHT  ";
        case PC_BISHOP: return "PC_BISHOP  ";
        case PC_ROOK: return "PC_ROOK    ";
        case PC_QUEEN: return "PC_QUEEN   ";

        case CAPTURE: return "CAPTURE    ";
    }
    return " ";
}

// mainly for debugging purposes
void printMovesList(const MoveList& moveList) {
    std::cout << "Move  BIT-Flags \n";

    for (int moveCount = 0; moveCount < moveList.count; moveCount++) {
        const Move move {moveList.moves[moveCount].first};
        std::printf("%s%s%c   ", chessBoard[move.from()],
                          chessBoard[move.to()],
                          promotedPieces[move.promotionPiece()] );

        std::cout << std::bitset<4> (move.flags()) << "   " << getStringFlags(move.flags()) << '\n';
    }

}
Move parseMove(const std::string_view move) {

    const int startSquare = (move[0] - 'a') + (move[1] - '0') * 8 - 8;
    const int endSquare = (move[2] - 'a') + (move[3] - '0') * 8 - 8;

    MoveList moveList;
    generateMoves(moveList);

    for (int count=0; count< moveList.count; count++) {

        if ( ((moveList.moves[count].first).from() == startSquare) &&  ((moveList.moves[count].first).to() == endSquare) ){
            const int promotedPiece{ moveList.moves[count].first.promotionPiece() };

            if (promotedPiece) {

                if ( ((promotedPiece % 6) == QUEEN) && (move[4] == 'q') ) return moveList.moves[count].first;
                if ( ((promotedPiece % 6) == ROOK) && (move[4] == 'r') ) return moveList.moves[count].first;
                if ( ((promotedPiece % 6) == BISHOP) && (move[4] == 'b') ) return moveList.moves[count].first;
                if ( ((promotedPiece % 6) == KNIGHT) && (move[4] == 'n') ) return moveList.moves[count].first;

            } else { return moveList.moves[count].first; }
        }
    }
    return 0; // returns null move
}

void printHistoryInfo() {

    for (int i=0; i < 10; i++) {

        std::cout << "castle: " << board.history[i].castle
                  << " captured: " << board.history[i].captured
                  << " en pass: " << chessBoard[board.history[i].enPassSq] << '\n';

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
