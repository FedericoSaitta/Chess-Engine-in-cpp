//
// Created by Federico Saitta on 10/07/2024.
//

#ifndef UCI_H
#define UCI_H

#include <fstream>

extern std::ofstream logFile;

// All are in milliseconds
extern int gameLengthTime;
extern int whiteClockTime;
extern int blackClockTime;
extern int whiteIncrementTime;
extern int blackIncrementTime;

void UCI();


#endif //UCI_H
