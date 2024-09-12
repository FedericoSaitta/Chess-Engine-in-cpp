//
// Created by Federico Saitta on 29/06/2024.
//

#pragma once

#include <iostream>
#include "../chess/board.h"


class Searcher; // forward declaration

namespace Test::BenchMark {

    void perftDriver(int depth, Board& board);
    std::int64_t perft(int depth, Board& board, bool printInfo=true);
    void standardPerft();


    struct Puzzle {
        std::string FEN;
        std::string matingMove;
        int depth;

        Puzzle(const std::string& f, const std::string& m, int d)
            : FEN(f), matingMove(m), depth(d) {}
    };
    void matingPuzzles();

    void staticSearch(Searcher& thread, int depth=14);

    void staticEval();
}
