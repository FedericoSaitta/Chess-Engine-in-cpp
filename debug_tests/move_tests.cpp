//
// Created by Federico Saitta on 03/07/2024.
//
#include "../benchmark_tests/benchmark_tests.h"
#include "../debug_tests/debug_tests.h"

#include <iostream>
#include <random>
#include <cmath>

#include "uci.h"
#include "../include/board.h"
#include "../include/macros.h"
#include "../include/misc.h"
#include "../src/search/search.h"


namespace Test::Debug{

    void moveSorting() {

        Searcher thread;

        thread.parseFEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
        MoveList moveList{};
        thread.pos.generateMoves(moveList);

        thread.giveScores(moveList, 0, thread.pos);
        for (int count=0; count < moveList.count; count++) {
            const Move move { moveList.moves[count].first };
            const int score { moveList.moves[count].second };

            printMove( move );
            std::cout << " score: " << score << '\n';
        }


        std::cout << "\nSorted moves\n";
        for (int count=0; count < moveList.count; count++) {
            std::pair<Move, int> scoredPair { thread.pickBestMove(moveList, count ) };
            const Move move { scoredPair.first };
            const int score { scoredPair.second };

            printMove( move );
            std::cout << " score: " << score << '\n';
        }
    }


    void printMoveOrdering(Searcher& thread) {
        MoveList moveList{};
        thread.pos.generateMoves(moveList);

        thread.pos.generateMoves(moveList);
        thread.giveScores(moveList, 0, thread.pos);
        std::cout << "\nSorted moves\n";
        for (int count=0; count < moveList.count; count++) {

            std::pair<Move, int> scoredPair { thread.pickBestMove(moveList, count ) };
            const Move move { scoredPair.first };
            const int score { scoredPair.second };

            COPY_HASH()
            if( !thread.pos.makeMove(move, 0) ) continue;

            printMove( move );
            std::cout << ": " << unicodePieces[thread.pos.mailbox[move.to()]] << " score: " << score
                      << " is capture: " << move.isCapture() << " is promotion: " << move.isPromotion()
                      << " promotion piece: " << promotedPieces[move.promotionPiece()]
                      << " gives check: " << thread.pos.currentlyInCheck() << '\n';

            thread.pos.undo(move);
            RESTORE_HASH()
        }

    }

}

