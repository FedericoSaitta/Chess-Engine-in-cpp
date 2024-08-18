#pragma once

static int stopSearch { 0 };
static int timePerMove { 0 };

constexpr int MAX_HISTORY_SCORE{ 16'384 };


// SEARCH PARAMETERS //
constexpr double LMR_BASE = 0.75;
constexpr double LMR_DIVISION = 3.0;

/*
inline extern int SEE_PRUNING_THRESHOLD = 9;
inline extern int SEE_CAPTURE_MARGIN = -35;
inline extern int SEE_QUIET_MARGIN = -80;

// agrressive had -15 and -65
inline extern int seeMargins[] {SEE_CAPTURE_MARGIN, SEE_QUIET_MARGIN };
*/