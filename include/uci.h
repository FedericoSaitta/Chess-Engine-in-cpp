//
// Created by Federico Saitta on 10/07/2024.
//

#pragma once
#include <fstream>


// All are in milliseconds
extern int gameLengthTime;
extern int whiteClockTime;
extern int blackClockTime;
extern int whiteIncrementTime;
extern int blackIncrementTime;

extern int movesToGo;

void resetGameVariables();
void UCI();

