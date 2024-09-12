#include "../debug_tests/debug_tests.h"

#include <iostream>
#include <random>

#include "uci.h"
#include "../chess/board.h"
#include "../../chess/misc.h"
#include "../../engine/search/search.h"


namespace Test::Debug{

    void moveSorting() {

        Searcher thread;

        thread.parseFEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
        MoveList moveList{};
        thread.pos.generateMoves(moveList);

        thread.giveScores(moveList, Move::Null, thread.pos);
        for (int count=0; count < moveList.count; count++) {
            const Move move { moveList.moves[count].first };
            const int score { moveList.moves[count].second };

            std::cout << move;
            std::cout << " score: " << score << '\n';
        }


        std::cout << "\nSorted moves\n";
        for (int count=0; count < moveList.count; count++) {
            std::pair<Move, int> scoredPair { thread.pickBestMove(moveList, count ) };
            const Move move { scoredPair.first };
            const int score { scoredPair.second };

            std::cout << move << getStringFlags(move.flags());;
            std::cout << " score: " << score << '\n';
        }
    }


    void printMoveOrdering(Searcher& thread) {
        MoveList moveList{};
        thread.pos.generateMoves(moveList);

        thread.pos.generateMoves(moveList);
        thread.giveScores(moveList, Move::Null, thread.pos);
        std::cout << "\nSorted moves\n";
        for (int count=0; count < moveList.count; count++) {

            std::pair<Move, int> scoredPair { thread.pickBestMove(moveList, count ) };
            const Move move { scoredPair.first };
            const int score { scoredPair.second };

            COPY_HASH()
            const Board copyBoard = thread.pos;

            if( !thread.pos.makeMove(move, 0) ) continue;

            std::cout << move << getStringFlags(move.flags());
            std::cout << ": " << unicodePieces[thread.pos.mailbox[move.to()]] << " score: " << score
                      << " is capture: " << move.isCapture() << " is promotion: " << move.isPromotion()
                      << " promotion piece: " << promotedPieces[move.promotionPiece()]
                      << " gives check: " << thread.pos.currentlyInCheck() << '\n';

            thread.pos = copyBoard;
            RESTORE_HASH()
        }

    }

}

