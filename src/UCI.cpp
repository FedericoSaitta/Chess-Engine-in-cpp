#include <iostream>
#include <string>
#include <vector>
#include <sstream>

#include "uci.h"
#include "config.h"

#include "hashtable.h"
#include "search/search.h"
#include "board.h"
#include "macros.h"
#include "misc.h"
#include "benchmark_tests.h"
#include "debug_tests.h"

#include "logger/logger.h"

#include "search.h"
#include "search/searchparams.h"

Searcher thread;
static bool isNewGame{true};

void resetGameVariables() {
    isNewGame = true;
    thread.resetGame();
}

static void handleUci() {
    std::cout << "id name Aramis v1.3.0 \n";
    std::cout << "id author Federico Saitta\n";

    std::cout << "option name Hash type spin default 64 min 1 max 256\n";

    // all the variable search parameters
    std::cout << "option name LMR_MIN_MOVES type spin default 4 min 2 max 6\n";
    std::cout << "option name LMR_MIN_DEPTH type spin default 3 min 2 max 6\n";

    std::cout << "option name windowWidth type spin default 50 min 10 max 100\n";
    std::cout << "option name SEE_THRESHOLD type spin default 105 min 80 max 125\n";

    std::cout << "option name RFP_MARGIN type spin default 80 min 60 max 120\n";
    std::cout << "option name RFP_DEPTH type spin default 9 min 7 max 11\n";

    std::cout << "option name NMP_DEPTH type spin default 3 min 2 max 4\n";
    std::cout << "option name NMP_BASE type spin default 4 min 3 max 5\n";
    std::cout << "option name NMP_DIVISION type spin default 4 min 3 max 6\n";

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
                thread.parseFEN(FEN);
                std::string moveString;

                while(inputStream >> moveString){
                    const Move move {parseMove(moveString, thread.pos)};

                    std::cout << moveString << '\n';

                    if (!move.isNone() ) { //so if the move inputStream != 0
                        std::cout << "making a move\n";
                        thread.repetitionIndex++;
                        thread.repetitionTable[thread.repetitionIndex] = hashKey;
                        if (thread.pos.makeMove(move, 0) == 0) {
                            std::cerr << "Could not find the move" << std::endl;
                            LOG_ERROR("Move inputStream illegal " + token );
                        }
                    } else {
                        std::cerr << "Move is Null" << std::endl;
                        LOG_ERROR("Move inputStream Null " + token );
                    }
                }
                goto no_re_parsing;
                // once we are done making the moves, we dont want to re-parse the thread.pos as that would nullify the moves
            }
        }

    } else if (token == "startpos") {
        FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    }
    thread.parseFEN(FEN);

    no_re_parsing:
}


static void handleGo(std::istringstream& inputStream) {
    std::string token;
    thread.movesToGo = 0; // need to reset to zero to ensure that if uci switches behaviour we dont use previous time controls
    while (inputStream >> token) {
        if (token == "perft") {
            if (inputStream >> token) Test::BenchMark::perft(std::stoi(token), thread.pos);
            std::cout << '\n';
            goto end_of_function;
        }
        if (token == "depth") {
            if (inputStream >> token) thread.iterativeDeepening(std::stoi(token), false);
            goto end_of_function;
        }

        if (token == "wtime") { if (inputStream >> token) thread.whiteClockTime = std::stoi(token); }
        else if (token == "btime") { if (inputStream >> token) thread.blackClockTime = std::stoi(token); }

        else if (token == "winc") { if (inputStream >> token) thread.whiteIncrementTime = std::stoi(token); }
        else if (token == "binc") { if (inputStream >> token) thread.blackIncrementTime = std::stoi(token); }

        else if (token == "movestogo") { if (inputStream >> token) thread.movesToGo = std::stoi(token); }
        else if (token == "movetime") {
            thread.movesToGo = 1; // as we will only need to make a singular move
            if (inputStream >> token) {
                if (thread.pos.side == WHITE)thread. whiteClockTime = std::stoi(token);
                else thread.blackClockTime = std::stoi(token);
            }
        }
        else LOG_ERROR("Unrecognized go input " + token);
    }

    if (isNewGame) {
        thread.gameLengthTime = thread.whiteClockTime;
        isNewGame = false;
    }
    thread.iterativeDeepening(MAX_PLY, true);

    end_of_function:
}


static void handleOption(std::istringstream& inputStream) {
    // again this is not the best, could use a while loop or some other way
    std::string token;
    inputStream >> std::skipws >> token;
    if (token == "name") {
        inputStream >> token;

        if ((token == "Hash")) {
            if (inputStream >> token && (token == "value")) {
                if ((inputStream >> token)) initTranspositionTable( std::stoi(token) );
            }
        }

        if ((token == "LMR_MIN_MOVES")) {
            if ((inputStream >> token) && (token == "value")) {
                if ((inputStream >> token)) thread.LMR_MIN_MOVES = ( std::stoi(token) );
            }
        }
        if ((token == "LMR_MIN_DEPTH")) {
            if ((inputStream >> token) && (token == "value")) {
                if ((inputStream >> token)) thread.LMR_MIN_DEPTH = ( std::stoi(token) );
            }
        }

        if ((token == "windowWidth")) {
            if ((inputStream >> token) && (token == "value")) {
                if ((inputStream >> token)) thread.windowWidth = ( std::stoi(token) );
            }
        }

        if ((token == "SEE_THRESHOLD")) {
            if ((inputStream >> token) && (token == "value")) {
                if ((inputStream >> token)) thread.SEE_THRESHOLD = ( std::stoi(token) );
            }
        }
        if ((token == "RFP_MARGIN")) {
            if ((inputStream >> token) && (token == "value")) {
                if ((inputStream >> token)) thread.RFP_MARGIN = ( std::stoi(token) );
            }
        }
        if ((token == "RFP_DEPTH")) {
            if ( (token == "value")) {
                if ((inputStream >> token)) thread.RFP_DEPTH = ( std::stoi(token) );
            }
        }

        if ((token == "NMP_DEPTH")) {
            if ((token == "value")) {
                if ((inputStream >> token)) thread.NMP_DEPTH = ( std::stoi(token) );
            }
        }
        if ((token == "NMP_BASE")) {
            if ((inputStream >> token) && (token == "value")) {
                if ((inputStream >> token)) thread.NMP_BASE = ( std::stoi(token) );
            }
        }

        if ( (token == "NMP_DIVISION")) {
            if ((inputStream >> token) && (token == "value")) {
                if ((inputStream >> token)) thread.NMP_DIVISION = ( std::stoi(token) );

            }
        }
    }
}

static void cleanUp() {
    if (transpositionTable != nullptr) free(transpositionTable);
}

//option name TEST type spin default 100 min 50 max 150
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
        else if (token == "display" ) thread.pos.printBoardFancy();
        else if (token == "moveOrdering") Test::Debug::printMoveOrdering(thread);
        else if (token == "hashfull") std::cout << checkHashOccupancy() << "/1000\n";
    }
}

void sendCommand(const std::string_view line) {
    std::cout << line;
    UCI();
}


