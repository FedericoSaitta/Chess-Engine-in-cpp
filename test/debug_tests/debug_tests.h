#pragma once

#include "../../chess/search.h"

// ANSI escape codes for colored output
#define RESET   "\033[0m"       // Reset the text formatting
#define RED     "\033[31m"      // Red text
#define GREEN   "\033[32m"      // Green text

inline void printTestOutput(const bool testPassed, const std::string& testName) {
    if (testPassed) {
        std::cout << GREEN << testName << RESET << std::endl;
    } else {
        std::cout << RED << testName << RESET << std::endl;
    }
}

namespace Test::Debug {

    void countPawnAttacks();
    void countKnightMoves();
    void countKingMoves();

    void moveSorting();

    void printMoveOrdering(Searcher& thread);

    void mirrorEval();
    void equalEvalTuner();

    void gameScenario();
    void boardInCheck();
    void boardNonPawnMat();

    void see_test();
}
