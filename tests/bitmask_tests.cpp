//
// Tests are designed to be fully contained so that user only needs to call the test from the main
// function without the need of any helper functions or previous input

#include "tests.h"

#include <bit>
#include <iostream>

#include "init.h"
#include "misc.h"
#include "macros.h"


namespace Test{

    void countPawnAttacks() {

        // Range is restricted as pawns can only captures between rank 1 and 7 inclusive
        for(int i=8; i < 56; i++) {

            const int whiteCount{ std::popcount(pawnAttacks[0][i]) };
            const int blackCount{ std::popcount(pawnAttacks[1][i]) };

            // checks that pawn is not on A or H file
            if ( (i % 8 == 0) | (i % 8 == 7)) {
                if ( (whiteCount != blackCount) || (whiteCount != 1) ) {
                    std::cerr << "**Index " << i << ": wrong** \n";
                }

            } else if ( (whiteCount != blackCount) || (whiteCount != 2) ){
                std::cerr << "**Index " << i << ": wrong** \n";
            }
        }

        std::cout << "countPawnAttacks Test successfull" << '\n';
    }

    void countKnightMoves() {

        if ( ( std::popcount(bitKnightAttacks[A1]) != std::popcount(bitKnightAttacks[A8]) )
          || ( std::popcount(bitKnightAttacks[A8]) != 2 ) ){
            std::cerr << "**Index " << A1 << " or " << A8 << ": wrong** \n";
        }

        if ( ( std::popcount(bitKnightAttacks[H8]) != std::popcount(bitKnightAttacks[H1]) )
          || ( std::popcount(bitKnightAttacks[H8]) != 2) ) {
            std::cerr << "**Index " << H8 << " or " << H1 << ": wrong** \n";
        }

        if (std::popcount(bitKnightAttacks[E4]) != 8) {
            std::cerr << "**Index " << E4 << ": wrong** \n";
        }

        // Total number of moves of a knight is 336:
        // https://chess.stackexchange.com/questions/33329/how-many-possible-chess-moves-are-there
        int totalMoves{};
        for (int i=0; i < 64; i++) { totalMoves += (std::popcount(bitKnightAttacks[i])); }

        if (totalMoves != 336) {
            std::cerr << "**Total number of moves is wrong** \n";
        }

        std::cout << "countKnightMoves Test successfull" << '\n';
    }

    void countKingMoves() {
        if ( ( std::popcount(bitKingAttacks[A1]) != std::popcount(bitKingAttacks[A8]) )
          || ( std::popcount(bitKingAttacks[A8]) != 3 ) ){
            std::cerr << "**Index " << A1 << " or " << A8 << ": wrong** \n";
          }

        if ( ( std::popcount(bitKingAttacks[H8]) != std::popcount(bitKingAttacks[H1]) )
          || ( std::popcount(bitKingAttacks[H8]) != 3) ) {
            std::cerr << "**Index " << H8 << " or " << H1 << ": wrong** \n";
          }

        if (std::popcount(bitKingAttacks[E4]) != 8) {
            std::cerr << "**Index " << E4 << ": wrong** \n";
        }

        // Total number of moves of a knight is 336:
        // https://chess.stackexchange.com/questions/33329/how-many-possible-chess-moves-are-there
        int totalMoves{};
        for (int i=0; i < 64; i++) { totalMoves += (std::popcount(bitKingAttacks[i])); }

        if (totalMoves != 420) {
            std::cerr << "**Total number of moves is wrong** \n";
        }

        std::cout << "countKingMoves Test successfull" << '\n';
    } // This doesnt consider castling

    void countBishopMoves_noEdges() {
        int totalMoves{};
        for (int i=0; i < 64; i++) { totalMoves += (std::popcount(bitBishopAttacks[i])); }

        if (totalMoves != 364) {
            std::cerr << "**countBishopMoves Test failed** \n";
        } else {
            std::cout << "countBishopMoves Test successfull" << '\n';
        }
    }

    void countRookMoves_noEdges() {
        int totalMoves{};
        for (int i=0; i < 64; i++) { totalMoves += (std::popcount(bitRookAttacks[i])); }

        if (totalMoves != 672) {
            std::cerr << "**countRookMoves Test failed** \n";
        } else {
            std::cout << "countRookMoves Test successfull" << '\n';
        }

    }

}