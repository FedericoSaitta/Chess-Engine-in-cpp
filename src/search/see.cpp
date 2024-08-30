#include "see.h"

#include "types.h"
#include "board.h"
#include "inline_functions.h"

#include <assert.h>

static const int SEEPieceValues[] = {
    161,  446,  464,  705, 1322,    0,    0,    0,
};


int see(const Move move, const int threshold, const Board& board) {
    // Unpack move information
    const int from  = move.from();
    const int to    = move.to();

    if (move.isCastling()) return true;

    // Next victim is moved piece or promotion type
    int nextVictim = move.isPromotion() ? move.promotionPiece() : pieceType(board.mailbox[from]);

    // Balance is the value of the move minus threshold. Function
    // call takes care for Enpass, Promotion and Castling moves.
    int value {};
    if (move.isCapture()) {
        if (move.isEnPassant()) value = SEEPieceValues[PAWN];
        else value = SEEPieceValues[pieceType(board.mailbox[ to ])];
    }

    if (move.isPromotion()) value += SEEPieceValues[move.promotionPiece()] - SEEPieceValues[PAWN];

    int balance = value - threshold;

    // Best case still fails to beat the threshold
    if (balance < 0) return false;

    // Worst case is losing the moved piece
    balance -= SEEPieceValues[nextVictim];

    // If the balance is positive even if losing the moved piece,
    // the exchange is guaranteed to beat the threshold.
    if (balance >= 0) return true;

    // Grab sliders for updating revealed attackers
    U64 bishops = board.getPieceTypeBitBoard(BISHOP) | board.getPieceTypeBitBoard(QUEEN);
    U64 rooks   = board.getPieceTypeBitBoard(ROOK) | board.getPieceTypeBitBoard(QUEEN);

    // Let occupied suppose that the move was actually made
    U64 occupied = board.bitboards[BOTH_OCC];

    // Now our opponents turn to recapture
    int color = !board.side;

    occupied = (occupied ^ (1ull << from)) | (1ull << to);
    if (move.isEnPassant()) {
        if (color == BLACK) { occupied ^= (1ull << (to - 8)); } // the move is white captures en-pass
        else { occupied ^= (1ull << (to + 8));}
    }

    // Get all pieces which attack the target square. And with occupied
    // so that we do not let the same piece attack twice
    U64 attackers = board.allAttackers(to, occupied) & occupied;

    while ( true ) {

        // If we have no more attackers left we lose
        U64 myAttackers = attackers & (color == WHITE ? board.bitboards[WHITE_OCC] : board.bitboards[BLACK_OCC]);
        if (myAttackers == 0ULL) break;

        // Find our weakest piece to attack with
        for (nextVictim = PAWN; nextVictim <= QUEEN; nextVictim++)
            if (myAttackers & board.getPieceTypeBitBoard( nextVictim ) )
                break;

        // Remove this attacker from the occupied
        occupied ^= (1ull << bsf(myAttackers & board.getPieceTypeBitBoard( nextVictim )) );

        // A diagonal move may reveal bishop or queen attackers
        if (nextVictim == PAWN || nextVictim == BISHOP || nextVictim == QUEEN)
            attackers |= getBishopAttacks(to, occupied) & bishops;

        // A vertical or horizontal move may reveal rook or queen attackers
        if (nextVictim == ROOK || nextVictim == QUEEN)
            attackers |= getRookAttacks(to, occupied) & rooks;

        // Make sure we did not add any already used attacks
        attackers &= occupied;

        // Swap the turn
        color = !color;

        // Negamax the balance and add the value of the next victim
        balance = -balance - 1 - SEEPieceValues[nextVictim];

        // If the balance is non negative after giving away our piece then we win
        if (balance >= 0) {

            // As a slight speed up for move legality checking, if our last attacking
            // piece is a king, and our opponent still has attackers, then we've
            // lost as the move we followed would be illegal
            if (nextVictim == KING && (attackers & (color == WHITE ? board.bitboards[WHITE_OCC] : board.bitboards[BLACK_OCC])))
                color = !color;

            break;
        }
    }

    // Side to move after the loop loses
    return board.side != color;
}