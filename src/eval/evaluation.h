//
// Created by Federico Saitta on 04/07/2024.
//
#pragma once

#include "board.h"
extern int mg_value[6];
extern int eg_value[6];

void init_tables();
int evaluate(const Board& pos);
