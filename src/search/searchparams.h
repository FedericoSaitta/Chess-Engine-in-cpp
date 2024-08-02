//
// Created by Federico Saitta on 26/07/2024.
//

#pragma once

// SEARCH PARAMETERS //
constexpr int LMR_MIN_MOVES { 4 }; // searching the first 4 moves at the full depth
constexpr int LMR_MIN_DEPTH { 3 };

constexpr int windowWidth{ 50 }; // the aspritation window, the width is 100

static int stopSearch { 0 };
static int timePerMove { 0 };

constexpr double historyAgeRatio = 0.125;
constexpr int maxHistoryScore{ 1'600 };