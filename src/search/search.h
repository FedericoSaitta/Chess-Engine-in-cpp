#pragma once

#include "../../include/hashtable.h"
#include "../include/types.h"
#include "board.h"
#include "timer.h"

enum NodeType {
    DONT_NULL=0,
    DO_NULL,
};

#define MAX_PLY 64

#define INF 50'000
#define MATE_VALUE 49'000
#define MATE_SCORE 48'000 // this is more like a boundary to the scores

void initSearchTables();
void clearHistoryTable();

class Searcher {

public:
    Board pos;

    // Search Tables:          //
    Move killerMoves[2][MAX_PLY];
    int historyScores[64][64];

    Move pvTable[MAX_PLY][MAX_PLY];
    int pvLength[MAX_PLY];

    U64 repetitionTable[512];
    int repetitionIndex;

    // Search Variables:       //
    int searchPly;

    int scorePV;
    int followPV; // if it is true then we follow the principal variation

    // Time Control variables: //
    Timer searchTimer;
    Timer singleDepthTimer;

    int gameLengthTime;
    int time;
    int increment;

    int movesToGo;

    bool stopSearch;
    int timePerMove;

    //                         //

    void parseFEN(const std::string& fenString) {
        repetitionIndex = 0;
        memset(repetitionTable, 0, sizeof(repetitionTable));
        pos.parseFEN(fenString);
    }

    void resetGame(){
        // resetting all the time-controls just in case
        gameLengthTime = 0;
        time = 0;
        increment = 0;

        movesToGo = 0;

        // reset the hash Table
        clearTranspositionTable();
        memset(historyScores, 0, sizeof(historyScores));

        repetitionIndex = 0;
        memset(repetitionTable, 0, sizeof(repetitionTable));
    }

    void resetSearchStates() {
        memset(killerMoves, 0, sizeof(killerMoves));
        memset(pvLength, 0, sizeof(pvLength));
        memset(pvTable, 0, sizeof(pvTable));

        followPV = 0;
        scorePV = 0;
        searchPly = 0;

        stopSearch = false;
    }


    int quiescenceSearch(int alpha, int beta);
    int aspirationWindow(int currentDepth, int previousScore);
    int negamax(int alpha, int beta, int depth, NodeType canNull);
    void iterativeDeepening(int depth, bool timeConstraint=false);

    void sendUciInfo(int score, int depth, int nodes, const Timer& depthTimer) const;

    void updateHistory(Move bestMove, int depth, const Move* quiets, int quietMoveCount);

    void updateKillers(Move bestMove);

    bool isKiller(Move move) const;
    bool isRepetition() const;
    void isTimeUp();

    void calculateMoveTime(bool timeConstraint);
    void enablePVscoring(const MoveList& moveList);

    int scoreMove(Move move, const Board& board);

    void giveScores(MoveList& moveList, Move bestMove, const Board& board);

    std::pair<Move, int> pickBestMove(MoveList& moveList, int start);

    // TUNABLE PARAMETERS //
    int LMR_MIN_MOVES { 5 };
    int LMR_DEPTH { 2 };

    // these are here but once tuned should be constexpr and done at compile time
    int LMR_BASE { 79 };
    int LMR_DIVISION { 287 };

    int LMP_DEPTH{ 9 };
    int LMP_MULTIPLIER{ 3 };

    int ASP_WINDOW_WIDTH{ 43 };

    int RFP_MARGIN { 75 };
    int RFP_DEPTH { 10 };

    int NMP_DEPTH { 2 };
    int NMP_BASE { 336 };
    int NMP_DIVISION{ 448 };

    int SEE_QS_THRESHOLD{ -105 };

    int SEE_PRUNING_DEPTH = 8;
    int SEE_CAPTURE_MARGIN = -31;
    int SEE_QUIET_MARGIN = -64;
};
