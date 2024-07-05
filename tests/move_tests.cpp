//
// Created by Federico Saitta on 03/07/2024.
//
#include "tests.h"

#include <iostream>
#include <random>
#include <cmath>
#include <chrono>

#include "globals.h"
#include "macros.h"
#include "inline_functions.h"
#include "misc.h"


namespace Test{

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

            int move = encodeMove(start, target, piece, promPiece, capture, doublePush, enPassant, castling);

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
}

