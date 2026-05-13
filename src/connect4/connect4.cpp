#include "connect4.hpp"

#include <cmath>
#include <climits>
#include <algorithm>
#include <chrono>

Connect4::Connect4() {
    for (int c = 0; c < COLUMNS; c++) {
        for (int r = 0; r < ROWS; r++) {
            set_token(c, r, 0);
        }
    }
}

int Connect4::play(int column, int color) {
    if (red_win or yellow_win) {
        return -1;
    }

    if (column > COLUMNS or column < 1) {
        return -1;
    }
    
    int row = ROWS - 1;
    while (row >= 0 && get_token_occupancy(column - 1, row)) {
        row--;
    }
    if (row < 0) {
        return -1;
    }

    set_token(column-1, row, make_token(color));

    update_win(column-1, row);

    return row;
}

void Connect4::output(std::ostream& output) {
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLUMNS; c++) {
            if (!get_token_occupancy(c, r)) {
                output << "- ";
            } else {
                switch (get_token_color(c, r)) {
                    case YELLOW: output << "y "; break;
                    case RED:    output << "r "; break;
                    default:     output << "? "; break;
                }
            }
        }
        output << "\n";
    }
}

unsigned char Connect4::get_token(int column, int row) const {
    int index = (column * ROWS + row) / 4;
    int shift = ((column * ROWS + row) % 4) * 2;

    unsigned char token = grid[index] >> shift;
    return token & TOKEN_MASK;
}

void Connect4::set_token(int column, int row, unsigned char token) {
    int index = (column * ROWS + row) / 4;
    int shift = ((column * ROWS + row) % 4) * 2;
    
    grid[index] &= ~(TOKEN_MASK << shift);
    grid[index] |= (token << shift);
}   

unsigned char Connect4::get_token_occupancy(int column, int row) const {
    unsigned char token = get_token(column, row);
    return token & OCCUPANCY_MASK;
}

unsigned char Connect4::get_token_color(int column, int row) const {
    unsigned char token = get_token(column, row);
    return (token & COLOR_MASK) >> 1;
}

unsigned char Connect4::make_token(int color) const {
    unsigned char token = 1;
    if (color == YELLOW) {
        token |= (1 << 1);
    }
    return token;
}

void Connect4::update_win(int last_col, int last_row) {
    static const int dr[] = {1, 0, 1, 1};
    static const int dc[] = {0, 1, 1, -1};

    unsigned char color = get_token_color(last_col, last_row);

    for (int d = 0; d < 4; d++) {
        int count = 1;
        for (int s = 1; s < 4; s++) {
            int r = last_row + s * dr[d];
            int c = last_col + s * dc[d];
            if (c < 0 || c >= COLUMNS || r < 0 || r >= ROWS ||
                !get_token_occupancy(c, r) || get_token_color(c, r) != color)
                break;
            count++;
        }
        for (int s = 1; s < 4; s++) {
            int r = last_row - s * dr[d];
            int c = last_col - s * dc[d];
            if (c < 0 || c >= COLUMNS || r < 0 || r >= ROWS ||
                !get_token_occupancy(c, r) || get_token_color(c, r) != color)
                break;
            count++;
        }
        if (count >= 4) {
            if (color == RED) {
                red_win = true;
            }
            else if (color == YELLOW) {
                yellow_win = true;
            }
        }
    }
    return;
}

int Connect4::get_number_of_n_in_a_rows(int n, int color) const {
    int count = 0;
    static const int dr[] = {0, 1, 1, 1};
    static const int dc[] = {1, 0, 1, -1};
    
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLUMNS; c++) {
            for (int d = 0; d < 4; d++) {
                int consecutive = 0;
                
                for (int i = 0; i < n; i++) {
                    int nr = r + i * dr[d];
                    int nc = c + i * dc[d];
                    
                    if (nr < 0 || nr >= ROWS || nc < 0 || nc >= COLUMNS) {
                        break;
                    }
                    
                    if (get_token_occupancy(nc, nr) && get_token_color(nc, nr) == color) {
                        consecutive++;
                    } else {
                        break;
                    }
                }
                
                if (consecutive == n) {
                    count++;
                }
            }
        }
    }
    
    return count;
}

int Connect4Bot::evaluate(const Connect4& position) const {
    if (position.red_win) {
        return 1000;
    }
    else if (position.yellow_win) {
        return -1000;
    }
   
    int score = 0;

    score += 50 * position.get_number_of_n_in_a_rows(3, RED);
    score -= 50 * position.get_number_of_n_in_a_rows(3, YELLOW);

    return score;
}

int Connect4Bot::minimax(const Connect4& position, unsigned int depth, int alpha, int beta, bool maximizing_player, std::chrono::steady_clock::time_point deadline) const {
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
        Connect4 child(position);
        child.play(move, maximizing_player ? RED : YELLOW);
        int eval = minimax(child, depth - 1, alpha, beta, not maximizing_player, deadline);

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

int Connect4Bot::get_best_move(const Connect4& position, bool maximizing_player, std::chrono::milliseconds time_limit) {
    auto deadline = std::chrono::steady_clock::now() + time_limit;
    
    std::vector<int> moves = position.get_valid_moves();
    int best_move = moves[0];
    int best_eval = maximizing_player ? INT_MIN : INT_MAX;
    int best_index = 0;

    int depth = 5;

    while (true) {
        for (int i = 0; i < moves.size(); i++) {
            Connect4 child(position);
            child.play(moves[i], maximizing_player ? RED : YELLOW);
            int eval = minimax(child, depth, INT_MIN, INT_MAX, !maximizing_player, deadline);

            if (std::chrono::steady_clock::now() >= deadline) {
                break;
            }

            if ((maximizing_player && eval > best_eval) || (!maximizing_player && eval < best_eval)) {
                best_eval = eval;
                best_index = i;
                best_move = moves[i];
            }

            // std::cout << "move: " << moves[i] << " eval: " << eval << "\n"; 
        }

        if (std::chrono::steady_clock::now() >= deadline) {
            break;
        }

        if (best_index >= 0 && best_index < moves.size()) {
            std::rotate(moves.begin(), moves.begin() + best_index, moves.end());
        }

        depth += 2;
    }

    std::cout << "chose: " << best_move << " eval: " << best_eval << " depth: " << depth << "\n";
    return best_move;
}