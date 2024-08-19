#include "debug_tests.h"

#include "board.h"
#include "../src/search/see.h"
#include "uci.h"
#include "types.h"
#include "misc.h"

#include <fstream>




namespace Test::Debug {

    void see_test() {

        const std::string fileName = "../debug_tests/see_suite.txt";
        std::ifstream file(fileName);

        if (!file.is_open()) { std::cerr << "Error opening file: " << fileName << std::endl; }

        Board board;

        std::istream* input = &file;

        std::string command;
        std::string token;

        while (std::getline(*input, command)) {
            std::istringstream inputStream(command);
            token.clear();
            inputStream >> std::skipws >> token;

            std::string FEN{ token + ' '};

            while (inputStream >> token) {
                if (token != "|") {
                    FEN += token + ' ';
                } else {
                    break;
                }
            }

            board.parseFEN(FEN);

            inputStream >> token;

            std::string moveString { token };
            const Move move {parseMove(moveString, board)};

            inputStream >> token;
            inputStream >> token;

            int threshold { std::stoi(token) };

            inputStream >> token;
            inputStream >> token;

            int correctResult { (token == "true"? 1 : 0) };
            int score { see(move, threshold, board) };

            std::cout << correctResult << ' ' << score << '\n';

        }
    }

}