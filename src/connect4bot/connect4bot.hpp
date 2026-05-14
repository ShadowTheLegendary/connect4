#pragma once

#ifndef CONNECT4BOT_HPP
#define CONNECT4BOT_HPP

#include "src/connect4/connect4.hpp"
#include <chrono>

class Connect4Bot {
public:
    int evaluate(const Connect4& position) const;

    int minimax(Connect4& position, unsigned int depth, int alpha, int beta, bool maximizing_player, std::chrono::steady_clock::time_point deadline) const;

    int get_best_move(Connect4& position, bool maximizing_player, std::chrono::milliseconds time_limit);

private:
    mutable TranspositionTable tt;

};

#endif