//
// Created by Federico Saitta on 26/07/2024.
//

#pragma once

// SEARCH PARAMETERS //
constexpr double LMR_BASE = 0.75;
constexpr double LMR_DIVISION = 3.0;

constexpr int LMR_MIN_MOVES { 4 }; // searching the first 4 moves at the full depth
constexpr int LMR_MIN_DEPTH { 3 };

constexpr int windowWidth{ 50 }; // the aspritation window, the width is 100

static int stopSearch { 0 };
static int timePerMove { 0 };

constexpr int MAX_HISTORY_SCORE{ 16'384 };