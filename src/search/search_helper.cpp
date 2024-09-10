#include "search.h"
#include "misc.h"


void Searcher::enablePVscoring(const MoveList& moveList) {
    followPV = 0;

    for (int count=0; count < moveList.count; count++) {

        if ( moveList.moves[count].first == pvTable[0][searchPly] ) {
            assert(!pvTable[0][searchPly].isNone() && "enablePVscoring: pv is following a null move");
            assert(!moveList.moves[count].first.isNone() && "enablePVscoring: pv is following a null move");

            scorePV = 1; // if we do find a move
            followPV = 1; // we are in principal variation so we want to follow it
            break;
        }
    }
}

bool Searcher::isKiller(const Move move) const {
    assert(!move.isNone());
    if ( (move == killerMoves[0][searchPly]) || (move == killerMoves[1][searchPly]) ) return true;
    return false;
}

bool Searcher::isRepetition() const {
    // look if up until our repetition we have already encountered this position, asssuming the opponent
    // plays optimally they (just like us) will avoid repeting even once unless the position is drawn.
    for (int index=0; index < repetitionIndex; index+= 1) {
        // looping backwards over our previous keys
        if (repetitionTable[index] == hashKey) {
            return true; // repetition found
        }
    }
    return false; // no repetition
}


void Searcher::sendUciInfo(const int score, const int depth, const int nodes, const Timer& depthTimer) const {
    // Extracting the PV line and printing out in the terminal and logging file
    std::string pvString{};
    for (int count = 0; count < pvLength[0]; count++) { pvString += algebraicNotation(pvTable[0][count]) + ' '; }

    const double nps { nodes / depthTimer.elapsed() };

    std::string scoreType = "cp";
    int adjustedScore = score;

    if (score > -MATE_VALUE && score < -MATE_SCORE) {
        scoreType = "mate";
        adjustedScore = -(score + MATE_VALUE) / 2 - 1;
    } else if (score > MATE_SCORE && score < MATE_VALUE) {
        scoreType = "mate";
        adjustedScore = (MATE_VALUE - score) / 2 + 1;
    }

    // Print the information
    std::cout << "info score " << scoreType << " " << adjustedScore
              << " depth " << depth
              << " nodes " << nodes
              << " nps " << 1'000 * static_cast<std::int64_t>(nps)
              << " time " << depthTimer.roundedElapsed()
              << " pv " << pvString << std::endl;
}
