//
// Created by Federico Saitta on 04/07/2024.
//
#pragma once

// these will be used once ill decide to improve upon eval function, one used by WEISS doesnt look bad at all
// note that WEISS is very strong about 3000... and still uses hand crafted eval so it is definetely possible :)
#define S(mg, eg) ((int)((unsigned int)(eg) << 16) + (mg))
#define MgScore(s) ((int16_t)((uint16_t)((unsigned)((s)))))
#define EgScore(s) ((int16_t)((uint16_t)((unsigned)((s) + 0x8000) >> 16)))

void init_tables();
int evaluate();


