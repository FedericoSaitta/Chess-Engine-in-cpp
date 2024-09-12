#include "move.h"
#include "board.h"
#include "misc.h"

#include <iostream>
#include <fstream>
#include <string>


std::ostream& operator<<(std::ostream& os, const Move& move) {
    // Assuming chessBoard[] and promotedPieces[] are std::string arrays
    return os << chessBoard[move.from()]
              << chessBoard[move.to()]
              << promotedPieces[move.promotionPiece()];
}
