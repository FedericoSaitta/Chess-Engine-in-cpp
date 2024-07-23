//
// Created by Federico Saitta on 29/06/2024.
//

#ifndef TESTS_H
#define TESTS_H

#include <iostream>

namespace Test::BenchMark {

    void perftDriver(int depth);
    std::int64_t perft(int depth, bool printInfo=true);
    void standardPerft();


    struct Puzzle {
        std::string FEN;
        std::string matingMove;
        int depth;

        Puzzle(const std::string& f, const std::string& m, int d)
            : FEN(f), matingMove(m), depth(d) {}
    };
    void matingPuzzles();

    void staticSearch(int depth=10);
}


#endif //TESTS_H
