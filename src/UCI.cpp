#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <functional>

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

    // TUNABLE SEARCH PARAMETERS

    std::cout << "option name LMR_MIN_MOVES type spin default 4 min 2 max 6\n";
    std::cout << "option name LMR_DEPTH type spin default 3 min 2 max 6\n";

    std::cout << "option name LMP_DEPTH type spin default 8 min 2 max 6\n";
    std::cout << "option name LMP_MULTIPLIER type spin default 4 min 2 max 6\n";

    std::cout << "option name ASP_WINDOW_WIDTH type spin default 50 min 10 max 100\n";

    std::cout << "option name RFP_MARGIN type spin default 80 min 60 max 120\n";
    std::cout << "option name RFP_DEPTH type spin default 9 min 7 max 11\n";

    std::cout << "option name NMP_DEPTH type spin default 3 min 2 max 4\n";
    std::cout << "option name NMP_BASE type spin default 4 min 3 max 5\n";
    std::cout << "option name NMP_DIVISION type spin default 4 min 3 max 6\n";

    std::cout << "option name SEE_QS_THRESHOLD type spin default 105 min 80 max 125\n";
    std::cout << "option name SEE_PRUNING_DEPTH type spin default 9 min 6 max 11\n";
    std::cout << "option name SEE_CAPTURE_MARGIN type spin default 35 min 5 max 100\n";
    std::cout << "option name SEE_QUIET_MARGIN type spin default 80 min 20 max 120\n";

    std::cout << "uciok\n";
}

static void handleOption(const std::string& name, const int value) {
    // Thanks ChatGPT for this solution
    static std::unordered_map<std::string, std::function<void(int)>> optionsMap = {
        {"Hash", [](int v) { initTranspositionTable(v); }},

        {"LMR_MIN_MOVES", [](int v) { thread.LMR_MIN_MOVES = v; }},
        {"LMR_DEPTH", [](int v) { thread.LMR_DEPTH = v; }},

        {"LMP_DEPTH", [](int v) { thread.LMP_DEPTH = v; }},
        {"LMP_MULTIPLIER", [](int v) { thread.LMP_MULTIPLIER = v; }},

        {"ASP_WINDOW_WIDTH", [](int v) { thread.ASP_WINDOW_WIDTH = v; }},

        {"RFP_MARGIN", [](int v) { thread.RFP_MARGIN = v; }},
        {"RFP_DEPTH", [](int v) { thread.RFP_DEPTH = v; }},

        {"NMP_DEPTH", [](int v) { thread.NMP_DEPTH = v; }},
        {"NMP_BASE", [](int v) { thread.NMP_BASE = v; }},
        {"NMP_DIVISION", [](int v) { thread.NMP_DIVISION = v; }},

        {"SEE_QS_THRESHOLD", [](int v) { thread.SEE_QS_THRESHOLD = v; }},
        {"SEE_PRUNING_DEPTH", [](int v) { thread.SEE_PRUNING_DEPTH = v; }},
        {"SEE_CAPTURE_MARGIN", [](int v) { thread.SEE_CAPTURE_MARGIN = v; }},
        {"SEE_QUIET_MARGIN", [](int v) { thread.SEE_QUIET_MARGIN = v; }}
    };

    auto it = optionsMap.find(name);
    if (it != optionsMap.end()) {
        it->second(value);
    } else {
        std::cerr << "Unknown option: " << name << std::endl;
    }
}

void handleOption(std::istringstream& inputStream) {
    std::string token, optionName;
    int value;

    inputStream >> std::skipws >> token;
    if (token == "name") {
        inputStream >> optionName >> token;
        if (token == "value" && (inputStream >> value)) {
            handleOption(optionName, value);
        } else {
            std::cerr << "Invalid option format." << std::endl;
        }
    }
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

                    if (!move.isNone() ) { //so if the move inputStream != 0
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

        if (token == "wtime" && (thread.pos.side == WHITE) ) { if (inputStream >> token) thread.time = std::stoi(token); }
        else if (token == "btime" && (thread.pos.side == BLACK) ) { if (inputStream >> token) thread.time = std::stoi(token); }

        else if (token == "winc" && (thread.pos.side == WHITE) ) { if (inputStream >> token) thread.increment = std::stoi(token); }
        else if (token == "binc" && (thread.pos.side == BLACK) ) { if (inputStream >> token) thread.increment = std::stoi(token); }

        else if (token == "movestogo") { if (inputStream >> token) thread.movesToGo = std::stoi(token); }
        else if (token == "movetime") {
            thread.movesToGo = 1; // as we will only need to make a singular move
            if (inputStream >> token) {
                thread.time = std::stoi(token);
            }
        }
        else LOG_ERROR("Unrecognized go input " + token);
    }

    if (isNewGame) {
        thread.gameLengthTime = thread.time;
        isNewGame = false;
    }
    thread.iterativeDeepening(MAX_PLY, true);

    end_of_function:
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

        // NON-UCI COMMANDS used for debugging
        else if (token == "bench")  Test::BenchMark::staticSearch(thread);
        else if (token == "bench-eval") Test::BenchMark::staticEval();
        else if (token == "bench-game") Test::Debug::gameScenario();

        else if (token == "display" ) thread.pos.printBoardFancy();
        else if (token == "moveOrdering") Test::Debug::printMoveOrdering(thread);
        else if (token == "hashfull") std::cout << checkHashOccupancy() << "/1000\n";
    }
}



