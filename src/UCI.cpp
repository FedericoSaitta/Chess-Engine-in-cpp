#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <functional>
#include <charconv>
#include <cstring>
#include <optional>
#include <cctype>
#include <iomanip>

#include "uci.h"
#include "config.h"

#include "hashtable.h"
#include "search/search.h"
#include "board.h"
#include "macros.h"
#include "misc.h"
#include "benchmark_tests.h"
#include "debug_tests.h"
#include "eval/nnue.h"

#include "logger/logger.h"

#include "search.h"

Searcher thread;
static bool isNewGame{true};

void resetGameVariables() {
    isNewGame = true;
    thread.resetGame();
}

static void handleUci() {
    std::cout << "id name Aramis v1.4.0 \n";
    std::cout << "id author Federico Saitta\n";

    std::cout << "option name Hash type spin default 256 min 1 max 256\n";
    std::cout << "option name Threads type spin default 1 min 1 max 1\n";
    std::cout << "option name UseNNUE type check default true\n";
    std::cout << "option name EvalFile type string default " << nnue::DEFAULT_FILE << "\n";

    // TUNABLE SEARCH PARAMETERS

    std::cout << "option name LMR_MIN_MOVES type spin default 5 min 2 max 6\n";
    std::cout << "option name LMR_DEPTH type spin default 2 min 2 max 6\n";

    std::cout << "option name LMR_BASE type spin default 79 min 25 max 125\n";
    std::cout << "option name LMR_DIVISION type spin default 287 min 200 max 400\n";

    std::cout << "option name LMP_DEPTH type spin default 9 min 2 max 10\n";
    std::cout << "option name LMP_MULTIPLIER type spin default 3 min 2 max 6\n";

    std::cout << "option name ASP_WINDOW_WIDTH type spin default 43 min 10 max 100\n";

    std::cout << "option name RFP_MARGIN type spin default 75 min 50 max 100\n";
    std::cout << "option name RFP_DEPTH type spin default 10 min 7 max 11\n";

    std::cout << "option name NMP_DEPTH type spin default 2 min 1 max 4\n";
    std::cout << "option name NMP_BASE type spin default 336 min 300 max 500\n";
    std::cout << "option name NMP_DIVISION type spin default 448 min 300 max 600\n";

    std::cout << "option name SEE_QS_THRESHOLD type spin default -105 min -125 max -80\n";
    std::cout << "option name SEE_PRUNING_DEPTH type spin default 8 min 6 max 11\n";
    std::cout << "option name SEE_CAPTURE_MARGIN type spin default -31 min -100 max -5\n";
    std::cout << "option name SEE_QUIET_MARGIN type spin default -64 min -120 max -20\n";

    if (nnue::networkAvailable()) {
        std::cout << "info string NNUE loaded " << nnue::loadedNetworkPath()
                  << " checksum " << std::hex << nnue::loadedNetworkChecksum()
                  << std::dec << '\n';
    } else {
        std::cout << "info string NNUE unavailable; using classical evaluation\n";
    }

    std::cout << "uciok\n";
}

static bool handleSpinOption(const std::string& name, const int value) {
    // Thanks ChatGPT for this solution
    static std::unordered_map<std::string, std::function<void(int)>> optionsMap = {
        {"Hash", [](int v) { initTranspositionTable(std::clamp(v, 1, 256)); }},
        {"Threads", [](int) {}},

        {"LMR_MIN_MOVES", [](int v) { thread.LMR_MIN_MOVES = std::clamp(v, 2, 6); }},
        {"LMR_DEPTH", [](int v) { thread.LMR_DEPTH = std::clamp(v, 2, 6); }},

        {"LMR_BASE", [](int v) {
            thread.LMR_BASE = std::clamp(v, 25, 125);
            initSearchTables(thread.LMR_BASE, thread.LMR_DIVISION);
        }},
        {"LMR_DIVISION", [](int v) {
            thread.LMR_DIVISION = std::clamp(v, 200, 400);
            initSearchTables(thread.LMR_BASE, thread.LMR_DIVISION);
        }},

        {"LMP_DEPTH", [](int v) { thread.LMP_DEPTH = std::clamp(v, 2, 10); }},
        {"LMP_MULTIPLIER", [](int v) { thread.LMP_MULTIPLIER = std::clamp(v, 2, 6); }},

        {"ASP_WINDOW_WIDTH", [](int v) { thread.ASP_WINDOW_WIDTH = std::clamp(v, 10, 100); }},

        {"RFP_MARGIN", [](int v) { thread.RFP_MARGIN = std::clamp(v, 50, 100); }},
        {"RFP_DEPTH", [](int v) { thread.RFP_DEPTH = std::clamp(v, 7, 11); }},

        {"NMP_DEPTH", [](int v) { thread.NMP_DEPTH = std::clamp(v, 1, 4); }},
        {"NMP_BASE", [](int v) { thread.NMP_BASE = std::clamp(v, 300, 500); }},
        {"NMP_DIVISION", [](int v) { thread.NMP_DIVISION = std::clamp(v, 300, 600); }},

        {"SEE_QS_THRESHOLD", [](int v) { thread.SEE_QS_THRESHOLD = std::clamp(v, -125, -80); }},
        {"SEE_PRUNING_DEPTH", [](int v) { thread.SEE_PRUNING_DEPTH = std::clamp(v, 6, 11); }},
        {"SEE_CAPTURE_MARGIN", [](int v) { thread.SEE_CAPTURE_MARGIN = std::clamp(v, -100, -5); }},
        {"SEE_QUIET_MARGIN", [](int v) { thread.SEE_QUIET_MARGIN = std::clamp(v, -120, -20); }}
    };

    auto it = optionsMap.find(name);
    if (it != optionsMap.end()) {
        it->second(value);
        return true;
    }
    return false;
}

static std::optional<int> readInteger(std::istringstream& inputStream) {
    std::string token;
    if (!(inputStream >> token)) return std::nullopt;

    int value{};
    const auto [end, error] = std::from_chars(token.data(), token.data() + token.size(), value);
    if (error != std::errc{} || end != token.data() + token.size()) return std::nullopt;
    return value;
}

static std::string trim(std::string value) {
    const auto notSpace = [](const unsigned char c) { return !std::isspace(c); };
    value.erase(value.begin(), std::find_if(value.begin(), value.end(), notSpace));
    value.erase(std::find_if(value.rbegin(), value.rend(), notSpace).base(), value.end());
    return value;
}

void handleOption(std::istringstream& inputStream) {
    std::string token;
    inputStream >> std::skipws >> token;
    if (token != "name") {
        std::cerr << "Invalid option format." << std::endl;
        return;
    }

    std::string remainder;
    std::getline(inputStream, remainder);
    const std::size_t valuePosition = remainder.find(" value ");
    if (valuePosition == std::string::npos) {
        std::cerr << "Invalid option format." << std::endl;
        return;
    }
    const std::string optionName = trim(remainder.substr(0, valuePosition));
    const std::string optionValue = trim(remainder.substr(valuePosition + 7));

    if (optionName == "UseNNUE") {
        std::string normalized = optionValue;
        std::transform(normalized.begin(), normalized.end(), normalized.begin(),
                       [](const unsigned char c) { return static_cast<char>(std::tolower(c)); });
        const bool enabled = normalized == "true" || normalized == "1" || normalized == "on";
        thread.setUseNNUE(enabled);
        clearTranspositionTable();
        std::cout << "info string NNUE " << (enabled ? "enabled" : "disabled") << '\n';
        return;
    }

    if (optionName == "EvalFile" || optionName == "NNUEPath") {
        std::string error;
        if (nnue::loadNetwork(optionValue, error)) {
            thread.refreshNNUE();
            clearTranspositionTable();
            std::cout << "info string NNUE loaded " << nnue::loadedNetworkPath()
                      << " checksum " << std::hex << nnue::loadedNetworkChecksum()
                      << std::dec << '\n';
        } else {
            std::cout << "info string NNUE load failed: " << error
                      << "; keeping "
                      << (nnue::networkAvailable() ? nnue::loadedNetworkPath()
                                                   : "classical fallback")
                      << '\n';
        }
        return;
    }

    int value{};
    const auto* begin = optionValue.data();
    const auto* end = begin + optionValue.size();
    const auto [parsedEnd, error] = std::from_chars(begin, end, value);
    if (error != std::errc{} || parsedEnd != end) {
        std::cerr << "Invalid value for option " << optionName << std::endl;
        return;
    }
    if (!handleSpinOption(optionName, value)) {
            std::cerr << "Unknown option: " << optionName << std::endl;
    }
}

static bool applyPositionMoves(std::istringstream& inputStream) {
    const Board originalBoard = thread.pos;
    const int originalRepetitionIndex = thread.repetitionIndex;
    U64 originalRepetitions[512];
    std::memcpy(originalRepetitions, thread.repetitionTable, sizeof(originalRepetitions));
    const U64 originalHash = hashKey;

    std::string moveString;
    while (inputStream >> moveString) {
        const Move move {parseMove(moveString, thread.pos)};
        if (move.isNone() || thread.repetitionIndex >= 512) {
            thread.pos = originalBoard;
            thread.repetitionIndex = originalRepetitionIndex;
            std::memcpy(thread.repetitionTable, originalRepetitions, sizeof(originalRepetitions));
            hashKey = originalHash;
            std::cerr << "Invalid position move: " << moveString << std::endl;
            return false;
        }

        thread.repetitionTable[thread.repetitionIndex++] = hashKey;
        if (!thread.pos.makeMove(move, 0)) {
            thread.pos = originalBoard;
            thread.repetitionIndex = originalRepetitionIndex;
            std::memcpy(thread.repetitionTable, originalRepetitions, sizeof(originalRepetitions));
            hashKey = originalHash;
            std::cerr << "Illegal position move: " << moveString << std::endl;
            return false;
        }
    }
    return true;
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
                if (thread.tryParseFEN(FEN)) applyPositionMoves(inputStream);

                goto no_re_parsing;
                // once we are done making the moves, we dont want to re-parse the thread.pos as that would nullify the moves
            }
        }

    } else if (token == "startpos") {

        FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
        thread.parseFEN(FEN);
        inputStream >> token;

        if (token == "moves") {
            applyPositionMoves(inputStream);
        }

        goto no_re_parsing;
    }

    // this part is needed in the case we dont receive a moves command
    thread.parseFEN(FEN);

    no_re_parsing:;
}


static void handleGo(std::istringstream& inputStream) {
    std::string token;
    thread.time = 0;
    thread.increment = 0;
    thread.movesToGo = 0; // do not inherit a previous time control

    while (inputStream >> token) {
        if (token == "perft") {
            if (const auto depth = readInteger(inputStream); depth && *depth >= 0) {
                Test::BenchMark::perft(*depth, thread.pos);
            } else {
                std::cerr << "Invalid perft depth\n";
            }
            std::cout << '\n';
            goto end_of_function;
        }
        if (token == "depth") {
            if (const auto depth = readInteger(inputStream); depth && *depth > 0) {
                thread.iterativeDeepening(std::min(*depth, MAX_PLY), false);
            } else {
                std::cerr << "Invalid search depth\n";
            }
            goto end_of_function;
        }

        if (token == "wtime" || token == "btime") {
            const auto value = readInteger(inputStream);
            const bool isOurClock = (token == "wtime") == (thread.pos.side == WHITE);
            if (value && isOurClock) thread.time = std::max(*value, 0);
        }
        else if (token == "winc" || token == "binc") {
            const auto value = readInteger(inputStream);
            const bool isOurIncrement = (token == "winc") == (thread.pos.side == WHITE);
            if (value && isOurIncrement) thread.increment = std::max(*value, 0);
        }
        else if (token == "movestogo") {
            if (const auto value = readInteger(inputStream)) thread.movesToGo = std::max(*value, 0);
        }
        else if (token == "movetime") {
            thread.movesToGo = 1; // as we will only need to make a singular move
            if (const auto value = readInteger(inputStream)) thread.time = std::max(*value, 1);
        }
        else if (token == "infinite") {
            thread.movesToGo = 1;
            thread.time = 180'000;
        }
        else { LOG_ERROR("Unrecognized go input " + token); }
    }

    if (thread.time <= 0) thread.time = 1;

    if (isNewGame) {
        thread.gameLengthTime = thread.time;
        isNewGame = false;
    }
    thread.iterativeDeepening(MAX_PLY, true);

    end_of_function:;
}

static void cleanUp() {
    freeTranspositionTable();
}

//option name TEST type spin default 100 min 50 max 150
void UCI(const std::string_view fileName) {
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
    } else { LOG_INFO(("Reading from standard input (std::cin):")); }

    std::string command;
    std::string token;
    
    while (std::getline(*input, command)) {
        std::istringstream inputStream(command);
        token.clear();

        inputStream >> std::skipws >> token;

        LOG_INFO(command);

        // UCI COMMANDS
        if (token == "uci") handleUci();
        else if (token == "isready") handleIsReady();
        else if (token == "position") handlePosition(inputStream); // though this seems expensive because of al lthe checks,80 move game in 235 microsec
        else if (token == "go") handleGo(inputStream);

        else if (token == "setoption") handleOption(inputStream);
        else if (token == "ucinewgame") resetGameVariables();
        else if (token == "quit") break;

        // NON-UCI COMMANDS used for debugging
        else if (token == "bench")  Test::BenchMark::staticSearch(thread);
        else if (token == "bench-eval") Test::BenchMark::staticEval();
        else if (token == "bench-game") Test::Debug::gameScenario();

        else if (token == "display" ) thread.pos.printBoardFancy();
        else if (token == "moveOrdering") Test::Debug::printMoveOrdering(thread);
        else if (token == "hashfull") std::cout << checkHashOccupancy() << "/1000\n";
    }
    cleanUp();
}
