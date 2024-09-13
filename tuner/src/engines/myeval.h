#pragma once

#define TAPERED 1

#include "../base.h"
#include "../external/chess.hpp"

#include <vector>

namespace myEval
{
    class myEvaluation
    {
    public:
        constexpr static bool includes_additional_score = false;
        constexpr static bool supports_external_chess_eval = false;
        constexpr static bool retune_from_zero = false;
        constexpr static tune_t preferred_k = 0;
        constexpr static int32_t max_epoch = 5'000;
        constexpr static bool enable_qsearch = false;
        constexpr static bool filter_in_check = true;
        constexpr static tune_t initial_learning_rate = 1;
        constexpr static int32_t learning_rate_drop_interval = 10'000;
        constexpr static tune_t learning_rate_drop_ratio = 1;
        constexpr static bool print_data_entries = false;
        constexpr static int32_t data_load_print_interval = 100'000;

        static parameters_t get_initial_parameters();
        static EvalResult get_fen_eval_result(const std::string& fen);
        static void print_parameters(const parameters_t& parameters);

        static EvalResult get_external_eval_result(const chess::Board& board);
    };
}
