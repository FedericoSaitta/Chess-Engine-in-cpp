#include "board.h"

#include <vector>
#include <sstream>

#include "../include/hashtable.h"
#include "../engine/search/search.h"
#include "types.h"
#include "inline_functions.h"

#include "bit_operations.h"

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


static Piece charToPiece(const char c) {
    switch (c) {
        case 'P': return WHITE_PAWN;  // White Pawn
        case 'N': return WHITE_KNIGHT;  // White Knight
        case 'B': return WHITE_BISHOP;  // White Bishop
        case 'R': return WHITE_ROOK;  // White Rook
        case 'Q': return WHITE_QUEEN;  // White Queen
        case 'K': return WHITE_KING;  // White King
        case 'p': return BLACK_PAWN;  // Black Pawn
        case 'n': return BLACK_KNIGHT;  // Black Knight
        case 'b': return BLACK_BISHOP;  // Black Bishop
        case 'r': return BLACK_ROOK;  // Black Rook
        case 'q': return BLACK_QUEEN; // Black Queen
        case 'k': return BLACK_KING; // Black King
        default: return NO_PIECE;  // Invalid piece
    }
}

// think about using string view as you only need to read from the FEN string i think??
void Board::parseFEN(const std::string& fenString) {

    // re-setting the board state each time a new FEN is parsed
    resetBoard();

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
            const Piece piece { charToPiece(static_cast<unsigned char>(c)) };
            setBit(bitboards[piece], rank * 8 + file);
            mailbox[rank * 8 + file] = piece;
            file++;
        }
    }

    side = (parts[1][0] == 'w') ? WHITE : BLACK;

    for (const char c : parts[2]) {
        switch (c) {
            case 'K': castle |= WK; break;
            case 'Q': castle |= WQ; break;
            case 'k': castle |= BK; break;
            case 'q': castle |= BQ; break;
            default: break;
        }
    }

    if(parts[3][0] == '-') {
        enPassSq = 64; // the 64th index represents the 'outside the board' square
    } else {
        const int col { parts[3][0] - 'a'};
        const int row { 8 - (parts[3][1] - '0') };
        enPassSq = 56 - 8 * row + col;
    }


    // Lastly populate the white and black occupancy bitboards
    for (int bbPiece=0; bbPiece < 6; bbPiece++) {
        bitboards[WHITE_OCC] |= bitboards[bbPiece]; // for white
        bitboards[BLACK_OCC] |= bitboards[bbPiece + 6]; // for black
        bitboards[BOTH_OCC] |= (bitboards[bbPiece] | bitboards[bbPiece + 6]); // for both
    }

    // Now we initialize the zobrist hash key
    hashkey = generatehashkey(*this);
}


bool Board::currentlyInCheck() const {
    return isSqAttacked( bsf(bitboards[KING + 6 * side]), side^1);
}

bool Board::nonPawnMaterial() const {
    return ( bitboards[QUEEN + 6 * side] | bitboards[ROOK + 6 * side] | bitboards[BISHOP + 6 * side] | bitboards[KNIGHT + 6 * side]);
}


U64 Board::allAttackers(const int square, const U64 occupancy) const {
    const U64 whitePawns   = bitboards[WHITE_PAWN];
    const U64 blackPawns   = bitboards[BLACK_PAWN];

    const U64 attackingKnights = bitboards[WHITE_KNIGHT] | bitboards[BLACK_KNIGHT];
    const U64 attackingBishops = bitboards[WHITE_BISHOP] | bitboards[BLACK_BISHOP];
    const U64 attackingRooks   = bitboards[WHITE_ROOK] | bitboards[BLACK_ROOK];
    const U64 attackingQueens  = bitboards[WHITE_QUEEN] | bitboards[BLACK_QUEEN];
    const U64 attackingKing    = bitboards[WHITE_KING] | bitboards[BLACK_KING];

    return (bitPawnAttacks[WHITE][square] & whitePawns & bitboards[BLACK_OCC])
         | (bitPawnAttacks[BLACK][square] & blackPawns & bitboards[WHITE_OCC])
         | (bitKnightAttacks[square] & attackingKnights)
         | (getBishopAttacks(square, occupancy) & (attackingBishops | attackingQueens))
         | (getRookAttacks(square, occupancy) & (attackingRooks | attackingQueens))
         | (bitKingAttacks[square] & attackingKing);
}


bool Board::isInsufficientMaterial() const {
        // Check for KvK, KvN, KvB, and KvNN.
        return !(bitboards[WHITE_PAWN] | bitboards[BLACK_PAWN] | bitboards[WHITE_ROOK] | bitboards[BLACK_ROOK] | bitboards[WHITE_QUEEN] | bitboards[BLACK_QUEEN])
                && ( !several(bitboards[WHITE_OCC]) || !several(bitboards[BLACK_OCC]))
                && ( !several(bitboards[WHITE_KNIGHT] | bitboards[WHITE_BISHOP] | bitboards[BLACK_KNIGHT] | bitboards[BLACK_BISHOP])
                || (!(bitboards[WHITE_BISHOP] | bitboards[BLACK_BISHOP]) && countBits(bitboards[WHITE_KNIGHT] | bitboards[BLACK_KNIGHT]) <= 2) );
    }




