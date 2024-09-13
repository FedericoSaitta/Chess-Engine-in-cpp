#include "search.h"

constexpr double bestmoveScale[5] {2.43, 1.35, 1.09, 0.88, 0.68};

// From Alexandria :) thanks
void Searcher::calculateMoveTime(const bool timeConstraint) {
    const int safetyOverhead = std::min(100.0, time / 2);
    time -= safetyOverhead;

    if (!timeConstraint) {
        // If not given a time constraint search for one minute
        maxTime = 60'000;
    } else {
        // case of wtime btime command
        if (movesToGo == 0) {
            // Never use more than 76% of the total time left for a single move
            const double basetime = (time * 0.054 + increment * 0.85);

            maxTime = 0.76 * time;

            // Time we use to stop if we just cleared a depth
            softBoundTime = std::min(0.76 * basetime, maxTime);

            // Absolute maximum time we can spend on a search (unless it is bigger than the bound)
            maxTime = std::min(3.04 * basetime, maxTime);


        } else {
            maxTime = time / movesToGo;
        }
    }

    // for convenience we cast them into ints as out timer works with ints
    maxTime = static_cast<int>(maxTime);
    softBoundTime = static_cast<int>(softBoundTime);

    assert((timePerMove > 0) && "getMoveTime: movetime is zero/negative");
}



void Searcher::scaleTimeControl(const int bestMoveStabilityFactor) {
    softBoundTime = static_cast<int>( bestmoveScale[bestMoveStabilityFactor] * softBoundTime );
}

void Searcher::isTimeUp() {
    if ( searchTimer.elapsed() > maxTime) stopSearch = true;
}