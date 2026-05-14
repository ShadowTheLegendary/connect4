#include <cmath>
#include <climits>
#include <algorithm>
#include <chrono>
#include <iostream>

#include "connect4bot.hpp"

int Connect4Bot::evaluate(const Connect4& position) const {
    if (position.red_win) {
        return INT_MAX;
    }
    else if (position.yellow_win) {
        return INT_MIN;
    }
   
    int score = 0;
    
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLUMNS; c++) {
            for (int d = 0; d < 4; d++) {
                if (position.get_token_occupancy(r, c)) {
                    score += ((position.get_token_color(r, c) == RED) ? 1 : -1) * (10 - ((4-c) * (4-c)));
                }
            }
        }
    }
    
    return score;
}

int Connect4Bot::minimax(Connect4& position, unsigned int depth, int alpha, int beta, bool maximizing_player, std::chrono::steady_clock::time_point deadline) const {
    const int original_alpha = alpha;
    const uint64_t hash = hash_board(position.grid);

    // --- TT Probe ---
    TTEntry* tt_entry = tt.probe(hash);
    if (tt_entry && tt_entry->depth >= depth) {
        if (tt_entry->flag == TTFlag::EXACT) { 
            return tt_entry->score; 
        }
        else if (tt_entry->flag == TTFlag::LOWER_BOUND) {
            alpha = std::max(alpha, tt_entry->score);
        }
        else if (tt_entry->flag == TTFlag::UPPER_BOUND) {
            beta  = std::min(beta,  tt_entry->score);
        }
        if (alpha >= beta) {
            return tt_entry->score;
        }
    }

    std::vector<int> moves = position.get_valid_moves();

    if (depth == 0 || position.red_win || position.yellow_win || moves.empty()) {
        return evaluate(position);
    }

    if (tt_entry && tt_entry->best_move != 0xFF) {
        auto it = std::find(moves.begin(), moves.end(), (int)tt_entry->best_move);
        if (it != moves.end()) std::rotate(moves.begin(), it, it + 1);
    }

    int  best_score = maximizing_player ? INT_MIN : INT_MAX;
    uint8_t best_move = 0xFF;

    for (const int move : moves) {
        position.play(move, maximizing_player ? RED : YELLOW);
        int eval = minimax(position, depth - 1, alpha, beta, not maximizing_player, deadline);
        position.undo(move);

        if (std::chrono::steady_clock::now() >= deadline) {
            return 0;
        }

        if ((maximizing_player and (eval > best_score)) or (not maximizing_player and (eval < best_score))) { 
            best_score = eval; 
            best_move = move; 
        }
        alpha = maximizing_player ? std::max(alpha, best_score) : alpha;
        beta = (not maximizing_player) ? std::min(beta, best_score) : beta;
        if (beta <= alpha) {
            break;
        }
    }

    TTFlag flag = (best_score <= original_alpha) ? TTFlag::UPPER_BOUND : (best_score >= beta) ? TTFlag::LOWER_BOUND : TTFlag::EXACT;

    tt.store(hash, best_score, depth, flag, best_move);

    return best_score;
}

int Connect4Bot::get_best_move(Connect4& position, bool maximizing_player, std::chrono::milliseconds time_limit) {
    auto deadline = std::chrono::steady_clock::now() + time_limit;
    
    std::vector<int> moves = position.get_valid_moves();
    int best_move = moves[0];
    int best_eval = maximizing_player ? INT_MIN : INT_MAX;
    int best_index = 0;

    int depth = 5;

    while (true) {
        for (int i = 0; i < moves.size(); i++) {
            position.play(moves[i], maximizing_player ? RED : YELLOW);
            int eval = minimax(position, depth, INT_MIN, INT_MAX, !maximizing_player, deadline);
            position.undo(moves[i]);

            if (eval == (maximizing_player ? INT_MAX : INT_MIN)) { // return if we find a winning move
                return moves[i];
            }

            if (std::chrono::steady_clock::now() >= deadline) {
                break;
            }

            if ((maximizing_player && eval > best_eval) || (!maximizing_player && eval < best_eval)) {
                best_eval = eval;
                best_index = i;
                best_move = moves[i];
            }
        }

        if (std::chrono::steady_clock::now() >= deadline) {
            break;
        }

        if (best_index >= 0 && best_index < moves.size()) {
            std::rotate(moves.begin(), moves.begin() + best_index, moves.end());
        }

        depth += 2;
    }

    return best_move;
}