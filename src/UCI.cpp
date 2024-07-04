//
// Created by Federico Saitta on 04/07/2024.
//
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

#include "globals.h"
#include "constants.h"
#include "inline_functions.h"
#include "misc.h"
#include "tests.h"


// Helper function to split a string by space
std::vector<std::string> split(const std::string &str) {
    std::vector<std::string> tokens;
    std::istringstream iss(str);
    std::string token;
    while (iss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}
static int parseMove(const std::string& move) {

    const int startSquare = (move[0] - 'a') + (move[1] - '0') * 8 - 8;
    const int endSquare = (move[2] - 'a') + (move[3] - '0') * 8 - 8;

    MoveList moveList;
    generateMoves(moveList, 0);

    for (int i=0; i< moveList.count; i++) {

        if ( (getMoveStartSQ(moveList.moves[i]) == startSquare) &&  (getMoveTargettSQ(moveList.moves[i]) == endSquare)  ){
            const int promotedPiece{ getMovePromPiece(moveList.moves[i]) };

            if (promotedPiece) {
                if ( ((promotedPiece % 6) == Queen) && (move[4] == 'q') ) return moveList.moves[i];
                if ( ((promotedPiece % 6) == Rook) && (move[4] == 'r') ) return moveList.moves[i];
                if ( ((promotedPiece % 6) == Bishop) && (move[4] == 'b') ) return moveList.moves[i];
                if ( ((promotedPiece % 6) == Knight) && (move[4] == 'n') ) return moveList.moves[i];

            } else {
                return moveList.moves[i];
            }
        }
    }

    return 0;
}

static void handleUci() {
    std::cout << "id name MyChessEngine" << std::endl;
    std::cout << "id author Federico Saitta" << std::endl;
    std::cout << "uciok" << std::endl;
}
static void handleIsReady() { std::cout << "readyok" << std::endl; }
static void handlePosition(const std::vector<std::string>& tokens) {
    // Example: position startpos moves e2e4 e7e5
    if ( tokens[1] != "startpos" ) {
        // then we have to set up the new FEN
        // also need to check validity of the FEN
        FEN = tokens[1]; //this does not work for now
    }
    parseFEN(FEN);

    if ( tokens.size() > 3) {
        int move{};

        for (int index=3; index < tokens.size(); index++) {

            move = parseMove(tokens[index]);

            if (move) { //so if the move is != 0
                if (!makeMove(move, 0)) { std::cout << "Move is illegal \n"; }
            } else { std::cout << "Move is not on the board \n"; }

        }
    }
}
static void handleGo(const std::vector<std::string>& tokens) {

    if (tokens[1] == "perft") {
        const int depth = std::stoi(tokens[2]);

        if (FEN != '') {
            Test::perft(depth);
            std::cout << '\n';
        } else std::cout << "FEN has not been set \n";

    } else if (tokens [1] == "wtime") {
        // then we have to run our search move algorithm
        std::cout << "bestmove ...... \n";
    }
}

void UCI() {
    std::string line;

    while (std::getline(std::cin, line)) {
        std::vector<std::string> tokens = split(line);
        if (tokens.empty()) { continue; }

        const std::string command = tokens[0];

        if ( command == "uci") handleUci();
        else if ( command == "isready") handleIsReady();
        else if ( command == "position") handlePosition(tokens);
        else if ( command == "go") handleGo(tokens);
        else if ( command == "display" ) printBoardFancy();

        else if ( command == "quit") break;
        else if ( command == "newgame") {
        }

    }
}


