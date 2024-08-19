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
    int whiteClockTime;
    int blackClockTime;
    int whiteIncrementTime;
    int blackIncrementTime;

    int movesToGo;
    //                         //

    void parseFEN(const std::string& fenString) {
        repetitionIndex = 0;
        memset(repetitionTable, 0, sizeof(repetitionTable));
        pos.parseFEN(fenString);
    }

    void resetGame(){
        // resetting all the time-controls just in case
        gameLengthTime = 0;
        whiteClockTime = 0;
        blackClockTime = 0;
        whiteIncrementTime = 0;
        blackIncrementTime = 0;

        movesToGo = 0;

        // reset the hash Table
        clearTranspositionTable();
        memset(historyScores, 0, sizeof(historyScores));

        repetitionIndex = 0;
        memset(repetitionTable, 0, sizeof(repetitionTable));
    }
    int quiescenceSearch(int alpha, int beta);
    int aspirationWindow(int currentDepth, int previousScore);
    int negamax(int alpha, int beta, int depth, NodeType canNull);
    void iterativeDeepening(int depth, bool timeConstraint=false);

    void sendUciInfo(int score, int depth, int nodes, const Timer& depthTimer);

    void updateHistory(Move bestMove, int depth, const Move* quiets, int quietMoveCount);
    bool isKiller(Move move);
    void updateKillers(Move bestMove);
    int isRepetition();

    void isTimeUp();

    int getMoveTime(bool timeConstraint, int turn);
    void enablePVscoring(const MoveList& moveList);

    void resetSearchStates();


    int scoreMove(Move move, const Board& board);

    void giveScores(MoveList& moveList, Move bestMove, const Board& board);

    std::pair<Move, int> pickBestMove(MoveList& moveList, int start);


    int LMR_MIN_MOVES { 4 }; // searching the first 4 moves at the full depth
    int LMR_MIN_DEPTH { 2 };

    int windowWidth{ 46 }; // the aspritation window, the width is 100

    int SEE_THRESHOLD{ 105 };

    int RFP_MARGIN { 76 };
    int RFP_DEPTH { 9 };

    int NMP_DEPTH { 2 };
    float NMP_BASE { 3.38 };
    float NMP_DIVISION{ 4.48 };

    // Now only tuning these
    int SEE_PRUNING_THRESHOLD = 9;
    int SEE_CAPTURE_MARGIN = 50;
    int SEE_QUIET_MARGIN = 100;

    //int seeMargins[] {SEE_CAPTURE_MARGIN, SEE_QUIET_MARGIN };
};
