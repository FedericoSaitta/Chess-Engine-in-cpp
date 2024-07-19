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

    void moveEncodingAndDecoding() {

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> distrib(0.0, 1.0);

        for (int i=0; i < 10'000; i++) {
            double num{ distrib(gen) };

            int start{ static_cast<int>(std::ceil(num * 63))};
            int target{ static_cast<int>(std::ceil(num * 63))};
            int piece{ static_cast<int>(std::ceil(num * 11))};
            int promPiece{ static_cast<int>(std::ceil(num * 11))};
            int capture{ (num > 0.5) ? 1 : 0 };
            int doublePush{ (num > 0.5) ? 1 : 0 };
            int enPassant{ (num > 0.5) ? 1 : 0 };
            int castling{ (num > 0.5) ? 1 : 0 };

            int move = ENCODE_MOVE(start, target, piece, promPiece, capture, doublePush, enPassant, castling);

            if ( getMoveStartSQ(move) != start ) {
                std::cerr << "wrong startSQ" << '\n';
                break;
            }
            if ( getMoveTargetSQ(move) != target ) {
                std::cerr << "wrong targetSQ" << '\n';
                break;
            }
            if ( getMovePiece(move) != piece ) {
                std::cerr << "wrong piece" << '\n';
                break;
            }
            if ( getMovePromPiece(move) != promPiece ) {
                std::cerr << "wrong promPiece" << '\n';
                break;
            }
            if ( (getMoveCapture(move) >> 20) != capture ) {
                std::cerr << "wrong capture" << '\n';
                break;
            }
            if ( (getMoveDoublePush(move) >> 21) != doublePush ) {
                std::cerr << "wrong doublePush" << '\n';
                break;
            }
            if ( (getMoveEnPassant(move) >> 22) != enPassant ) {
                std::cerr << "wrong enPassant" << '\n';
                break;
            }
            if ( (getMoveCastling(move) >> 23) != castling ) {
                std::cerr << "wrong castling" << '\n';
                break;
            }
        }

        std::cout << "moveEncodingAndDecoding Test successful" << '\n';
    }


    void moveSorting() {

        parseFEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
        MoveList moveList{};
        generateMoves(moveList);

        for (int count=0; count < moveList.count; count++) {
            printMove(moveList.moves[count]);
            std::cout << " score: " << scoreMove(moveList.moves[count]) << '\n';
        }

        sortMoves(moveList, 0);
        std::cout << "\nSorted moves\n";
        for (int count=0; count < moveList.count; count++) {
            printMove(moveList.moves[count]);
            std::cout << " score: " << scoreMove(moveList.moves[count])<< '\n';
        }
    }



    static void printHistoryTable(const int a) {
        for (int square=0; square < 64; square++) {
            std::cout << historyMoves[a][square] << ' ';
            if ( (square + 1) % 8 == 0) std::cout << '\n';
        }
    }

    static void ageHistoryTable() {
        for (int a=0; a < 64; a++) {
            for (int b=0; b<64; b++) {
                // make sure we dont go over the limit
                historyMoves[a][b] = std::min(180, historyMoves[a][b] / 8);
            }
        }
    }

    void historyScores() {
        //resetGameVariables();
        parseFEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
        MoveList moveList{};
        generateMoves(moveList);

        iterativeDeepening(10, false);

        for (int a=0; a < 64; a++) {
            std::cout << "History Table Square: " << chessBoard[a] << '\n';
            printHistoryTable(a);
        }

        ageHistoryTable();

        for (int piece=0; piece < 12; piece++) {
            std::cout << "History Table: " << piece << '\n';
            printHistoryTable(piece);
        }
    }
}

