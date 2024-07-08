#include "macros.h"
#include "globals.h"
#include "inline_functions.h"

#include <vector>
#include <sstream>
#include <cstring>


static constexpr int charPieces[] = {
    ['P'] = 0, ['N'] = 1, ['B'] = 2, ['R'] = 3, ['Q'] = 4, ['K'] = 5,   // white
    ['p'] = 6, ['n'] = 7, ['b'] = 8, ['r'] = 9, ['q'] = 10, ['k'] = 11  // black
};

// think about using string view as you only need to read from the FEN string i think??
void parseFEN(const std::string& fenString) {

    // re-setting the board state each time a new FEN is parsed
    memset(bitboards, 0ULL, sizeof(bitboards));
    memset(occupancies, 0ULL, sizeof(occupancies));
    side = 0;
    castle = 0;
    enPassantSQ = 0;

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
            setBit(bitboards[charPieces[static_cast<unsigned char>(c)]], rank * 8 + file);
            file++;
        }
    }

    side = (parts[1][0] == 'w') ? White : Black;

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
        enPassantSQ = 64; // the 64th index represents the 'outside the board' square
    } else {
        const int col { parts[3][0] - 'a'};
        const int row { 8 - (parts[3][1] - '0') };
        enPassantSQ = 56 - 8 * row + col;
    }


    // Lastly populate the white and black occupancy bitboards
    for (int bbPiece=0; bbPiece < 6; bbPiece++) {
        occupancies[0] |= bitboards[bbPiece]; // for white
        occupancies[1] |= bitboards[bbPiece + 6]; // for black
        occupancies[2] |= (bitboards[bbPiece] | bitboards[bbPiece + 6]); // for both
    }

    // Now we initialize the zobrist hash key
    hashKey = generateHashKey();
}