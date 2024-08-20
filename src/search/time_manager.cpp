#include "search.h"


void Searcher::calculateMoveTime(const bool timeConstraint) {
    if (!timeConstraint) {
        timePerMove = 180'000; // maximum searching time of 3 minutes
    } else {

        if (movesToGo == 0) {
            timePerMove = time / 30 + increment;
            if ( (increment > 0) && (time < (5 * increment) ) ) timePerMove = static_cast<int>(0.75 * increment);
        } else {
            timePerMove = time / movesToGo;
        }
    }


    assert((timePerMove > 0) && "getMoveTime: movetime is zero/negative");
}

void Searcher::isTimeUp() {
    if ( searchTimer.elapsed() > timePerMove) stopSearch = true;
}