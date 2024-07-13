//
// Created by Federico Saitta on 29/06/2024.
//

#ifndef TESTS_H
#define TESTS_H

#include <iostream>

namespace Test::BenchMark {

    void perftDriver(int depth);
    std::uint32_t perft(int depth);
    void standardizedPerft();


    struct Puzzle {
        std::string FEN;
        std::string matingMove;
        int depth;

        Puzzle(const std::string& f, const std::string& m, int d)
            : FEN(f), matingMove(m), depth(d) {}
    };
    void matingPuzzles();
    void branchingFactor();
}


#endif //TESTS_H
