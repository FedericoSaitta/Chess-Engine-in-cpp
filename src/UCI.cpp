//
//
//
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

#include "uci.h"

#include "movegen/update.h"
#include "hashtable.h"
#include "search/search.h"
#include "board.h"
#include "macros.h"
#include "misc.h"
#include "benchmark_tests.h"
#include "debug_tests.h"


int gameLengthTime{};
int whiteClockTime{};
int blackClockTime{};
int whiteIncrementTime{};
int blackIncrementTime{};

int movesToGo{};
static bool isNewGame{true};

// Helper function to split a string by space

void resetGameVariables() {
    isNewGame = true;
    // resetting all the time-controls just in case
    gameLengthTime = 0;
    whiteClockTime = 0;
    blackClockTime = 0;
    whiteIncrementTime = 0;
    blackIncrementTime = 0;

    movesToGo = 0;

    // reset the hash Table
    clearTranspositionTable();
    memset(historyScores, 0, sizeof(historyScores));

    repetitionIndex = 0;
    repetitionTable[150] = 0;
}


static Move parseMove(const std::string_view move) {

    const int startSquare = (move[0] - 'a') + (move[1] - '0') * 8 - 8;
    const int endSquare = (move[2] - 'a') + (move[3] - '0') * 8 - 8;

    MoveList moveList;
    generateMoves(moveList);

    for (int count=0; count< moveList.count; count++) {

        if ( ((moveList.moves[count].first).from() == startSquare) &&  ((moveList.moves[count].first).to() == endSquare) ){
            const int promotedPiece{ moveList.moves[count].first.promotionPiece() };

            if (promotedPiece) {

                if ( ((promotedPiece % 6) == QUEEN) && (move[4] == 'q') ) return moveList.moves[count].first;
                if ( ((promotedPiece % 6) == ROOK) && (move[4] == 'r') ) return moveList.moves[count].first;
                if ( ((promotedPiece % 6) == BISHOP) && (move[4] == 'b') ) return moveList.moves[count].first;
                if ( ((promotedPiece % 6) == KNIGHT) && (move[4] == 'n') ) return moveList.moves[count].first;

            } else { return moveList.moves[count].first; }
        }
    }
    return 0; // returns null move
}

static void handleUci() {
    std::cout << "id name Aramis v1.3.0 \n";
    std::cout << "id author Federico Saitta\n";
    std::cout << "uciok\n";
}
static void handleIsReady() {
    std::cout << "readyok\n";
}

static void handlePosition(const std::vector<std::string>& tokens) {

    // Two valid and equivalent inputs that could be received
    // position startpos moves e2e4 e7e5
    // position rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 moves e2e4 e7e5
    int shiftIndex{};
    std::string FEN{};

    if ( tokens[1] == "fen" ) {
        shiftIndex = 6;
        // Horrible line of code, do not know how to make it better though
        FEN = tokens[2] + ' ' + tokens[3] + ' ' + tokens[4] + ' ' + tokens[5] + ' ' + tokens[6] + ' ' + tokens[7];

    } else if (tokens[1] == "startpos") {
        FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    } else {  }

    parseFEN(FEN);

    if ( (tokens.size() - shiftIndex) > 3) {

        for (int index=(3 + shiftIndex); index < static_cast<int>(tokens.size()); index++) {

            const Move move = parseMove(tokens[index]);

            if (! ((move.from() == 0) && (move.to() == 0)) ) { //so if the move is != 0
                repetitionIndex++;
                repetitionTable[repetitionIndex] = hashKey;
                if (!makeMove(move, 0)) {  }

            } else {
            }

        }
    }
}


static void handleGo(const std::vector<std::string>& tokens) {

    if (tokens[1] == "perft") {
        Test::BenchMark::perft(std::stoi(tokens[2]));
        std::cout << '\n';
    }
    else if (tokens[1] == "depth") iterativeDeepening(std::stoi(tokens[2]), false);

    else if ( ((tokens[1] == "wtime") && (tokens[3] == "btime")) ) {
        whiteClockTime = std::stoi(tokens[2]);
        blackClockTime = std::stoi(tokens[4]);
        if (tokens.size() > 5) {
            if ((tokens[5] == "winc") && (tokens[7] == "binc")) {
                whiteIncrementTime = std::stoi(tokens[6]);
                blackIncrementTime = std::stoi(tokens[8]);
            }

            else if ( tokens[5] == "movestogo" ) {
                movesToGo = std::stoi(tokens[6]);
            }
        }

        if (isNewGame) {
            gameLengthTime = whiteClockTime;
            isNewGame = false;
        }
        iterativeDeepening(64, true);
    }
    else if (tokens[1] == "movetime") {

        if (board.side == WHITE) whiteClockTime = std::stoi(tokens[2]);
        else blackClockTime = std::stoi(tokens[2]);

        if (isNewGame) {
            gameLengthTime = (board.side == WHITE) ? whiteClockTime : blackClockTime;
            isNewGame = false;
        }

        // you should write about this once you have got the time and version 1 is already on the way
        movesToGo = 1;

        iterativeDeepening(64, true);
    }

    else { // also look at how your GUI tells you the time
    //    std::cout << "Input could not be recognised\n";
    //    logFile << "Unrecognized input " << tokens[1] << '\n';
    }
}

// for now we only handle hash size changes
static void handleOption(const std::vector<std::string>& tokens) {

    if (tokens[1] == "name") {
        if (tokens[2] == "Hash") {
            if ( (tokens [3] == "value") && (tokens.size() > 4) ){
                initTranspositionTable( std::stoi(tokens[4]) );
            }
        }
    }
}


void UCI() {
    std::string line{};

    while (std::getline(std::cin, line)) {
        std::vector<std::string> tokens = split(line);
        if (tokens.empty()) { continue; }
        const std::string command = tokens[0];

        // UCI COMMANDS
        if ( command == "uci") handleUci();
        else if ( command == "isready") handleIsReady();
        else if ( command == "position") handlePosition(tokens); // though this seems expensive because of al lthe checks, 80 move game in 235 microsec
        else if ( command == "go") handleGo(tokens);

        else if (command == "setoption") handleOption(tokens);
        else if ( command == "quit") break;
        else if ( command == "ucinewgame") resetGameVariables();

        // NON-UCI COMMANDS
        else if (command == "bench")  Test::BenchMark::staticSearch();
        else if (command == "display" ) printBoardFancy();
        else if (command == "moveOrdering") Test::Debug::printMoveOrdering();
        else if (command == "hashfull") std::cout << checkHashOccupancy() << "/1000\n";
    }
}


