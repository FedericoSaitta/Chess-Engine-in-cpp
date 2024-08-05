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
#include "../src/search/movesort.h"
#include "../include/board.h"
#include "../include/macros.h"
#include "../include/misc.h"
#include "../src/search/search.h"


namespace Test::Debug {

    void gameScenario() {
        const std::string fileName = "/Users/federicosaitta/CLionProjects/ChessEngine/debug_tests/game_log.txt";
        UCI(fileName);
    }
}