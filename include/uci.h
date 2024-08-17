//
// Created by Federico Saitta on 10/07/2024.
//

#pragma once
#include <iostream>

void resetGameVariables();
void UCI(std::string_view fileName="");
void sendCommand(std::string_view fileName);
