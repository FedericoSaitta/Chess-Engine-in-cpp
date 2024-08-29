#pragma once
#include <iostream>

void resetGameVariables();
void UCI(std::string_view fileName="");
void sendCommand(std::string_view fileName);
