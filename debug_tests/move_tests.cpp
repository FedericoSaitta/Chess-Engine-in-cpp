//
// Created by Federico Saitta on 03/07/2024.
//
#include "../benchmark_tests/benchmark_tests.h"
#include "../debug_tests/debug_tests.h"

#include <iostream>
#include <random>
#include <cmath>

#include "uci.h"
#include "../src/search/movesort.h"
#include "../include/board.h"
#include "../include/macros.h"
#include "../include/misc.h"
#include "../src/search/search.h"


namespace Test::Debug{

    void moveSorting() {

        board.parseFEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
        MoveList moveList{};
        board.generateMoves(moveList);

        giveScores(moveList, 0, board);
        for (int count=0; count < moveList.count; count++) {
            const Move move { moveList.moves[count].first };
            const int score { moveList.moves[count].second };

            printMove( move );
            std::cout << " score: " << score << '\n';
        }


        std::cout << "\nSorted moves\n";
        for (int count=0; count < moveList.count; count++) {
            std::pair<Move, int> scoredPair { pickBestMove(moveList, count ) };
            const Move move { scoredPair.first };
            const int score { scoredPair.second };

            printMove( move );
            std::cout << " score: " << score << '\n';
        }
    }


    static void printHistoryTable(const int sq) {
        for (int square=0; square < 64; square++) {
            std::cout << historyScores[sq][square] << ' ';
            if ( (square + 1) % 8 == 0) std::cout << '\n';
        }
    }

    void printMoveOrdering() {
        MoveList moveList{};
        board.generateMoves(moveList);

        board.generateMoves(moveList);
        giveScores(moveList, 0, board);
        std::cout << "\nSorted moves\n";
        for (int count=0; count < moveList.count; count++) {

            std::pair<Move, int> scoredPair { pickBestMove(moveList, count ) };
            const Move move { scoredPair.first };
            const int score { scoredPair.second };

            COPY_HASH()
            if( !board.makeMove(move, 0) ) continue;

            printMove( move );
            std::cout << ": " << unicodePieces[board.mailbox[move.to()]] << " score: " << score
                      << " is capture: " << move.isCapture() << " is promotion: " << move.isPromotion()
                      << " promotion piece: " << promotedPieces[move.promotionPiece()]
                      << " gives check: " << board.currentlyInCheck() << '\n';

            board.undo(move);
            RESTORE_HASH()
        }

    }

}

