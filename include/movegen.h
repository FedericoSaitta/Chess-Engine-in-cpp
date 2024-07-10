//
// Created by Federico Saitta on 10/07/2024.
//

#ifndef MOVEGEN_H
#define MOVEGEN_H

struct MoveList {
    int moves[218];
    int count;
};

void generateMoves(MoveList& moveList);

#endif //MOVEGEN_H
