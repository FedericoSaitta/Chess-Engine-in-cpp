#include "see.h"

#include "types.h"
#include "board.h"
#include "inline_functions.h"

#include <assert.h>
#include <mach/machine.h>

// using ethereal values
static const int SEEPieceValues[] = {
    100,  300,  300,  500, 900,    0,    0,    0,
};

int moveEstimatedValue(const Move move) {


    // Start with the value of the piece on the target square
    int value = SEEPieceValues[ board.mailbox[move.to()] % 6];

    // Factor in the new piece's value and remove our promoted pawn
    if (move.isPromotion())
        value += SEEPieceValues[move.promotionPiece()] - SEEPieceValues[PAWN];

    // Target square is encoded as empty for enpass moves
    else if (move.isEnPassant())
        value = SEEPieceValues[PAWN];

    return value;
}



int see(const Move move, const int threshold) {

    const int from {move.from()};
    const int to{move.to()};

    // Next victim is moved piece or promotion type
    int nextVictim = board.mailbox[from] % 6;

    // Balance is the value of the move minus threshold. Function
    // call takes care for Enpass, Promotion and Castling moves.
    int value = moveEstimatedValue(move) - threshold;

    if (value < 0) return 0; // in the case of a non-capture

    value -= SEEPieceValues[nextVictim];

    // if the the difference between the nextVictim and the target is larger than the threshold,
    // the move is a good enough capture regardless of future recaptures
    if (value >= 0) return 1;

    const U64 whiteOcc { board.bitboards[WHITE_OCC] };
    const U64 blackOcc { board.bitboards[BLACK_OCC] };
    const U64 allOcc { board.bitboards[BOTH_OCC] };

    U64 occupied { (allOcc ^ (1ULL << from)) | (1ULL << to ) };
    if (move.isEnPassant()) occupied ^= (1ULL << board.history[board.gamePly].enPassSq);

    U64 attackers = board.allAttackers(to , occupied) & occupied;

    const U64 queens = board.bitboards[WHITE_QUEEN] | board.bitboards[BLACK_QUEEN];
    const U64 bishops = board.bitboards[WHITE_BISHOP] | board.bitboards[BLACK_BISHOP] | queens;
    const U64 rooks   = board.bitboards[WHITE_ROOK] | board.bitboards[BLACK_ROOK] | queens;

    int color { GET_BIT(board.bitboards[WHITE_OCC], from) ? BLACK : WHITE };

    while (true) {
        attackers &= occupied;
        U64 myAttackers = attackers & (color == WHITE ? whiteOcc : blackOcc);

        if (!myAttackers) break;

        int piecetype;
        for (piecetype = 5; piecetype >= 0; piecetype--){
            if (myAttackers & (board.bitboards[piecetype] | board.bitboards[piecetype + 6]))
                break;
        }

        color ^= 1; // swap the colour
        value = -value - 1 - SEEPieceValues[piecetype];

        if (value >= 0)
        {
            if (piecetype == KING && (attackers & (color == WHITE ? whiteOcc : blackOcc)))
                color ^= 1; // swap the colour

            break;
        }


        occupied ^= (1ULL << ( bsf(myAttackers & (board.bitboards[piecetype] | board.bitboards[piecetype + 6]) ) ) );

        if (piecetype == PAWN || piecetype == BISHOP || piecetype == QUEEN)
            attackers |= getBishopAttacks(to, occupied) & bishops;
        if (piecetype == ROOK || piecetype == QUEEN)
            attackers |= getRookAttacks(to, occupied) & rooks;
    }
    // Side to move after the loop loses
    return board.side != color;
}