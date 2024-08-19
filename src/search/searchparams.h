#pragma once

static int stopSearch { 0 };
static int timePerMove { 0 };

constexpr int MAX_HISTORY_SCORE{ 16'384 };


// SEARCH PARAMETERS //
constexpr double LMR_BASE = 0.75;
constexpr double LMR_DIVISION = 3.0;
