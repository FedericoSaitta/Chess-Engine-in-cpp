//
// Created by Federico Saitta on 03/07/2024.
//
#include "../benchmark_tests/benchmark_tests.h"

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

        parseFEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
        MoveList moveList{};
        generateMoves(moveList);

        giveScores(moveList, 0);
        for (int count=0; count < moveList.count; count++) {
            const Move move { moveList.moves[count].first };
            const int score { moveList.moves[count].second };

            printMove( move );
            std::cout << " score: " << score << '\n';
        }


        std::cout << "\nSorted moves\n";
        for (int count=0; count < moveList.count; count++) {
            const Move move{ pickBestMove(moveList, count) };
            const int score { moveList.moves[count].second };

            printMove( move );
            std::cout << " score: " << score << '\n';
        }
    }


    static void printHistoryTable(const int piece) {
        for (int square=0; square < 64; square++) {
            std::cout << historyScores[piece][square] << ' ';
            if ( (square + 1) % 8 == 0) std::cout << '\n';
        }
    }


    void printMoveOrdering() {
        MoveList moveList{};
        generateMoves(moveList);

        generateMoves(moveList);
        giveScores(moveList, 0);
        std::cout << "\nSorted moves\n";
        for (int count=0; count < moveList.count; count++) {
            const Move move{ pickBestMove(moveList, count) };
            const int score { moveList.moves[count].second };

            printMove( move );
            std::cout << ": " << unicodePieces[board.mailbox[move.from()]] << " score: " << score
                      << " flag: " << getStringFlags(move.flags()) << '\n';
        }
    }

    void historyScores() {
        //resetGameVariables();
        parseFEN("rnbqkb1r/pppppppp/5n2/8/8/4P3/PPPP1PPP/RNBQKBNR w KQkq - 0 3");

        printMoveOrdering();
        iterativeDeepening(3, false);

        std::cout << "AGED HISTORY TABLE:\n";
        for (int piece=0; piece < 12; piece++) {
            std::cout << "History Table: " << unicodePieces[piece] << '\n';
            printHistoryTable(piece);
        }

        // now we check how the move scores have changed and display move ordering
        printMoveOrdering();
    }
}

