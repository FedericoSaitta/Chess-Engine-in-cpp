//
// Created by Federico Saitta on 05/08/2024.
//
#include "../debug_tests/debug_tests.h"

#include <iostream>
#include <random>
#include <sstream>
#include <fstream>
#include <cmath>

#include "uci.h"
#include "../chess/board.h"
#include "../chess/macros.h"
#include "../../chess/misc.h"
#include "../../engine/search/search.h"


namespace Test::Debug {

    void gameScenario() {
        const std::string fileName = "/Users/federicosaitta/CLionProjects/ChessEngine/test/debug_tests/game_log.txt";
        UCI(fileName);
    }


    void boardInCheck() {
        bool testPassed{ true };
        Board board;
        // No need to test this heavily as we know isSquareAttacked function is correct because of perft
        board.parseFEN("rnbqkbnr/ppp1pppp/3p4/1B6/4P3/8/PPPP1PPP/RNBQK1NR b KQkq - 1 2"); // black is in check
        if (board.currentlyInCheck() != true) testPassed=false;

        board.parseFEN("rnb1kb1r/pp1qpppp/2pp4/2Q5/3P2P1/2NB1n2/PPP1N3/R1B1K2R w KQkq - 0 11"); // white is in check
        if (board.currentlyInCheck() != true) testPassed=false;

        board.parseFEN("rnb1kb1r/pp1qpppp/2pp4/2Q5/3P2P1/2NB1n2/PPP1NK2/R1B4R b kq - 1 11"); // black is in check
        if (board.currentlyInCheck() != false) testPassed=false;

        printTestOutput(testPassed, "Board In Check");

    }

    void boardNonPawnMat() {
        bool testPassed{ true };
        Board board;
        board.parseFEN("rnb1kb1r/pp1qpppp/2pp4/2Q5/3P2P1/2NB1n2/PPP1NK2/R1B4R b kq - 1 11"); // plenty of pieces
        if (board.nonPawnMaterial() != true) testPassed=false;

        board.parseFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/4K3 w kq - 0 1"); // white only pawns
        if (board.nonPawnMaterial() != false) testPassed=false;

        board.parseFEN("4k3/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQ - 0 1"); // black only pawns
        if (board.nonPawnMaterial() != false) testPassed=false;

        printTestOutput(testPassed, "Board Non-Pawn Material");
    }
}