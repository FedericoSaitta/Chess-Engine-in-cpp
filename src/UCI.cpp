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
    memset(repetitionTable, 0, sizeof(repetitionTable));
}

static void handleUci() {
    std::cout << "id name Aramis v1.3.0 \n";
    std::cout << "id author Federico Saitta\n";
    std::cout << "uciok\n";
}
static void handleIsReady() {
    std::cout << "readyok\n";
}

static void handlePosition(std::istringstream& inputStream) {
    std::string token;
    inputStream >> std::skipws >> token;

    std::string FEN{};

    if ( token == "fen" ) {
        while (inputStream >> token) {
            if (token != "moves") FEN += token + ' ';

            else {
                board.parseFEN(FEN);
                std::string moveString;

                while(inputStream >> moveString){
                    const Move move = parseMove(moveString);

                    if (! ((move.from() == 0) && (move.to() == 0)) ) { //so if the move inputStream != 0
                        repetitionIndex++;
                        repetitionTable[repetitionIndex] = hashKey;
                        if (board.makeMove(move, 0) == 0) { LOG_ERROR("Move inputStream illegal " + token ); };
                    } else { LOG_ERROR("Move inputStream Null " + token ); }
                }
                goto no_re_parsing;
                // once we are done making the moves, we dont want to re-parse the board as that would nullify the moves
            }
        }

    } else if (token == "startpos") {
        FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    }
    board.parseFEN(FEN);

    no_re_parsing:
}



static void handleGo(std::istringstream& inputStream) {
    std::string token;
    movesToGo = 0; // need to reset to zero to ensure that if uci switches behaviour we dont use previous time controls
    while (inputStream >> token) {
        if (token == "perft") {
            if (inputStream >> token) Test::BenchMark::perft(std::stoi(token));
            std::cout << '\n';
            goto end_of_function;
        }
        if (token == "depth") {
            if (inputStream >> token) iterativeDeepening(std::stoi(token), false);
            goto end_of_function;
        }

        if (token == "wtime") { if (inputStream >> token) whiteClockTime = std::stoi(token); }
        else if (token == "btime") { if (inputStream >> token) blackClockTime = std::stoi(token); }

        else if (token == "winc") { if (inputStream >> token) whiteIncrementTime = std::stoi(token); }
        else if (token == "binc") { if (inputStream >> token) blackIncrementTime = std::stoi(token); }

        else if (token == "movestogo") { if (inputStream >> token) movesToGo = std::stoi(token); }
        else if (token == "movetime") {
            movesToGo = 1; // as we will only need to make a singular move
            if (inputStream >> token) {
                if (board.side == WHITE) whiteClockTime = std::stoi(token);
                else blackClockTime = std::stoi(token);
            }
        }
        else LOG_ERROR("Unrecognized go input " + token);
    }

    if (isNewGame) {
        gameLengthTime = whiteClockTime;
        isNewGame = false;
    }
    iterativeDeepening(MAX_PLY, true);

    end_of_function:
}

static void handleOption(std::istringstream& inputStream) {
    // again this is not the best, could use a while loop or some other way
    std::string token;
    inputStream >> std::skipws >> token;
    if (token == "name") {
        if ((inputStream >> token) && (token == "Hash")) {
            if ((inputStream >> token) && (token == "value")) {
                if ((inputStream >> token)) initTranspositionTable( std::stoi(token) );
            }
        }
    }
}

static void cleanUp() {
    if (transpositionTable != nullptr) free(transpositionTable);
}

void UCI(const std::string_view fileName) {
    std::string line{};
    std::ifstream file{};
    std::istream* input = &std::cin;

    if (fileName != "") {
        file.open(fileName.data());

        if (file.is_open()) {
            input = &file; // Use file as input stream
            LOG_INFO(("Reading from file " + static_cast<std::string>(fileName)));
        } else {
            std::cerr << "Error: Could not open file " << fileName << std::endl;
            return; // Exit if the file cannot be opened
        }
    } else LOG_INFO(("Reading from standard input (std::cin):"));

    std::string command;
    std::string token;
    
    while (std::getline(*input, command)) {
        std::istringstream inputStream(command);
        token.clear();

        inputStream >> std::skipws >> token;

        LOG_INFO(line);

        // UCI COMMANDS
        if (token == "uci") handleUci();
        else if (token == "isready") handleIsReady();
        else if (token == "position") handlePosition(inputStream); // though this seems expensive because of al lthe checks,80 move game in 235 microsec


        else if (token == "go") handleGo(inputStream);

        else if (token == "setoption") handleOption(inputStream);
        else if (token == "ucinewgame") resetGameVariables();
        else if (token == "quit") { // we clean up allocated memory and exit the program
            cleanUp();
            break;
        }

        // NON-UCI COMMANDS
        else if (token == "bench")  Test::BenchMark::staticSearch();
        else if (token == "bench-eval") Test::BenchMark::staticEval();
        else if (token == "display" ) printBoardFancy();
        else if (token == "moveOrdering") Test::Debug::printMoveOrdering();
        else if (token == "hashfull") std::cout << checkHashOccupancy() << "/1000\n";
    }
}

void sendCommand(const std::string_view line) {
    std::cout << line;
    UCI();
}


