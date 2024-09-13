#ifndef CONFIG_H
#define CONFIG_H 1

#include<cstdint>

#include "engines/myeval.h"

using TuneEval = myEval::myEvaluation;

constexpr int32_t data_load_thread_count = 4;
constexpr int32_t thread_count = 4;
constexpr static bool print_data_entries = false;
constexpr static int32_t data_load_print_interval = 10000;


#endif // !CONFIG_H
