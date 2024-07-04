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

    int shiftIndex{};
    // Example: position startpos moves e2e4 e7e5
    if ( tokens[1] == "fen" ) {
        shiftIndex = 6;
        FEN = tokens[2] + ' ' + tokens[3] + ' ' + tokens[4] + ' ' + tokens[5] + ' ' + tokens[6] + ' ' + tokens[7];

    } else if (tokens[1] == "startpos") {
        FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    } else { std::cout << "could not recognize input \n"; }

    parseFEN(FEN);

    if ( (tokens.size() - shiftIndex) > 3) {
        int move{};

        for (int index=(3 + shiftIndex); index < tokens.size(); index++) {

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

        Test::perft(depth);
        std::cout << '\n';

    } else {
        // then we have to run our search move algorithm
        searchPosition( 3 );
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
        // yes ofc it can be improved though clearly should not be priority, also the time alloted should be after the position is already parsed etc.
        else if ( command == "position") handlePosition(tokens); // though this seems expensive because of al lthe checks, 80 move game in 235 microsec
        else if ( command == "go") handleGo(tokens);
        else if ( command == "display" ) printBoardFancy();

        else if ( command == "quit") break;
        else if ( command == "newgame") {
        }

    }
}


