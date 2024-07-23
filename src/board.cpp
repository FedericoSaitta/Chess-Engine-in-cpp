
#include "board.h"
#include "macros.h"

#include <vector>
#include <sstream>
#include <cstring>

#include "hashtable.h"
#include "search/search.h"

Board board{};
const char* chessBoard[65] = {
    "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
    "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
    "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
    "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
    "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
    "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
    "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
    "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8", "--" //represents empty square
};


static int charToPiece(const char c) {
    switch (c) {
        case 'P': return 0;  // White Pawn
        case 'N': return 1;  // White Knight
        case 'B': return 2;  // White Bishop
        case 'R': return 3;  // White Rook
        case 'Q': return 4;  // White Queen
        case 'K': return 5;  // White King
        case 'p': return 6;  // Black Pawn
        case 'n': return 7;  // Black Knight
        case 'b': return 8;  // Black Bishop
        case 'r': return 9;  // Black Rook
        case 'q': return 10; // Black Queen
        case 'k': return 11; // Black King
        default: return -1;  // Invalid piece
    }
}

// think about using string view as you only need to read from the FEN string i think??
void parseFEN(const std::string& fenString) {

    // re-setting the board state each time a new FEN is parsed
    board.resetBoard();

    repetitionIndex = 0;
    memset(repetitionTable, 0, sizeof(repetitionTable));

    // FEN string like: rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
    // is the accepted format, missing whitespaces will result in errors
    std::istringstream iss(fenString);
    std::vector<std::string> parts{};
    std::string part{};
    while (std::getline(iss, part, ' ')) {
        parts.push_back(part);
    }

    const std::string_view boardConfig { parts[0] };
    int rank{7}, file{};
    for (const char c : boardConfig) {
        if (c == '/') {
            rank--;
            file = 0;
        } else if (std::isdigit(c)) {
            file += (c - '0'); // Skip empty squares, offseeting the char by position of '0' in ASCII
        } else {
            const int piece { charToPiece(static_cast<unsigned char>(c)) };
            SET_BIT(board.bitboards[piece], rank * 8 + file);
            file++;
        }
    }

    board.side = (parts[1][0] == 'w') ? WHITE : BLACK;

    for (const char c : parts[2]) {
        switch (c) {
            case 'K': board.castle |= WK; break;
            case 'Q': board.castle |= WQ; break;
            case 'k': board.castle |= BK; break;
            case 'q': board.castle |= BQ; break;
            default: break;
        }
    }


    if(parts[3][0] == '-') {
        board.enPassantSq = 64; // the 64th index represents the 'outside the board' square
    } else {
        const int col { parts[3][0] - 'a'};
        const int row { 8 - (parts[3][1] - '0') };
        board.enPassantSq = 56 - 8 * row + col;
    }


    // Lastly populate the white and black occupancy bitboards
    for (int bbPiece=0; bbPiece < 6; bbPiece++) {
        board.bitboards[WHITE_OCC] |= board.bitboards[bbPiece]; // for white
        board.bitboards[BLACK_OCC] |= board.bitboards[bbPiece + 6]; // for black
        board.bitboards[BOTH_OCC] |= (board.bitboards[bbPiece] | board.bitboards[bbPiece + 6]); // for both
    }

    // Now we initialize the zobrist hash key
    hashKey = generateHashKey();
}