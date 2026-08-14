#include "board.h"
#include "macros.h"

#include <vector>
#include <sstream>

#include "hashtable.h"
#include "search/search.h"
#include "../include/types.h"
#include "inline_functions.h"

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

void Board::parseFEN(const std::string& fenString) {
    tryParseFEN(fenString);
}

bool Board::tryParseFEN(const std::string& fenString) {
    std::istringstream iss(fenString);
    std::string boardConfig;
    std::string sideToken;
    std::string castleToken;
    std::string enPassantToken;
    if (!(iss >> boardConfig >> sideToken >> castleToken >> enPassantToken)) {
        std::cerr << "Invalid FEN: expected at least four fields\n";
        return false;
    }

    int ranks = 1;
    int files = 0;
    int whiteKings = 0;
    int blackKings = 0;
    for (const char c : boardConfig) {
        if (c == '/') {
            if (files != 8 || ranks >= 8) {
                std::cerr << "Invalid FEN board layout\n";
                return false;
            }
            ++ranks;
            files = 0;
        } else if (c >= '1' && c <= '8') {
            files += c - '0';
        } else {
            const Piece piece = charToPiece(c);
            if (piece == NO_PIECE) {
                std::cerr << "Invalid FEN piece: " << c << '\n';
                return false;
            }
            ++files;
            whiteKings += piece == WHITE_KING;
            blackKings += piece == BLACK_KING;
        }

        if (files > 8) {
            std::cerr << "Invalid FEN rank width\n";
            return false;
        }
    }

    if (ranks != 8 || files != 8 || whiteKings != 1 || blackKings != 1) {
        std::cerr << "Invalid FEN board or king count\n";
        return false;
    }
    if ((sideToken != "w" && sideToken != "b")
        || (enPassantToken != "-"
            && (enPassantToken.size() != 2
                || enPassantToken[0] < 'a' || enPassantToken[0] > 'h'
                || (enPassantToken[1] != '3' && enPassantToken[1] != '6')))) {
        std::cerr << "Invalid FEN side or en-passant field\n";
        return false;
    }
    for (const char c : castleToken) {
        if (c != '-' && c != 'K' && c != 'Q' && c != 'k' && c != 'q') {
            std::cerr << "Invalid FEN castling field\n";
            return false;
        }
    }

    // Validation completed: only now replace the current position.
    resetBoard();
    int rank{7}, file{};
    for (const char c : boardConfig) {
        if (c == '/') {
            rank--;
            file = 0;
        } else if (c >= '1' && c <= '8') {
            file += (c - '0'); // Skip empty squares, offseeting the char by position of '0' in ASCII
        } else {
            const Piece piece { charToPiece(static_cast<unsigned char>(c)) };
            SET_BIT(bitboards[piece], rank * 8 + file);
            mailbox[rank * 8 + file] = piece;
            file++;
        }
    }

    side = (sideToken == "w") ? WHITE : BLACK;

    for (const char c : castleToken) {
        switch (c) {
            case 'K': history[gamePly].castle |= WK; break;
            case 'Q': history[gamePly].castle |= WQ; break;
            case 'k': history[gamePly].castle |= BK; break;
            case 'q': history[gamePly].castle |= BQ; break;
            default: break;
        }
    }

    if(enPassantToken == "-") {
        history[gamePly].enPassSq = 64; // the 64th index represents the 'outside the board' square
    } else {
        const int fileIndex { enPassantToken[0] - 'a'};
        const int rankIndex { enPassantToken[1] - '1'};
        history[gamePly].enPassSq = rankIndex * 8 + fileIndex;
    }


    // Lastly populate the white and black occupancy bitboards
    for (int bbPiece=0; bbPiece < 6; bbPiece++) {
        bitboards[WHITE_OCC] |= bitboards[bbPiece]; // for white
        bitboards[BLACK_OCC] |= bitboards[bbPiece + 6]; // for black
        bitboards[BOTH_OCC] |= (bitboards[bbPiece] | bitboards[bbPiece + 6]); // for both
    }

    // Now we initialize the zobrist hash key
    hashKey = generateHashKey(*this);
    return true;
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


