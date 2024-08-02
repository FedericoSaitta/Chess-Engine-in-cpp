//
//
//
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

#include "uci.h"
#include "config.h"

#include "movegen/update.h"
#include "hashtable.h"
#include "search/search.h"
#include "board.h"
#include "macros.h"
#include "misc.h"
#include "benchmark_tests.h"
#include "debug_tests.h"

#include "logger/logger.h"


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
                if (board.makeMove(move, 0) == 0) { LOG_ERROR("Move is illegal " + tokens[index] ); };
            } else { LOG_ERROR("Move is Null " + tokens[index] ); }

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

    else { LOG_ERROR("Unrecognized go input " + tokens[1]);
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

static void cleanUp() {
    if (transpositionTable != nullptr) free(transpositionTable);
}


void UCI() {
    std::string line{};

    while (std::getline(std::cin, line)) {
        std::vector<std::string> tokens = split(line);
        if (tokens.empty()) { continue; }
        const std::string command = tokens[0];

        LOG_INFO(line);

        // UCI COMMANDS
        if ( command == "uci") handleUci();
        else if ( command == "isready") handleIsReady();
        else if ( command == "position") handlePosition(tokens); // though this seems expensive because of al lthe checks, 80 move game in 235 microsec
        else if ( command == "go") handleGo(tokens);

        else if (command == "setoption") handleOption(tokens);
        else if ( command == "ucinewgame") resetGameVariables();
        else if ( command == "quit") { // we clean up allocated memory and exit the program
            cleanUp(); break;
        }

        // NON-UCI COMMANDS
        else if (command == "bench")  Test::BenchMark::staticSearch();
        else if (command == "display" ) printBoardFancy();
        else if (command == "moveOrdering") Test::Debug::printMoveOrdering();
        else if (command == "hashfull") std::cout << checkHashOccupancy() << "/1000\n";
    }
}


