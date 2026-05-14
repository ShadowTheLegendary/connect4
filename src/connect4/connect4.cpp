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

Connect4::Connect4(const Connect4& other) {
    grid = other.grid;
    red_win = other.red_win;
    yellow_win = other.yellow_win;
    win_state_stack = other.win_state_stack;
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

    win_state_stack.push({red_win, yellow_win});

    set_token(column-1, row, make_token(color));

    update_win(column-1, row);

    return row;
}

int Connect4::undo(int column) {
    if (column > COLUMNS || column < 1) {
        return -1;
    }

    if (win_state_stack.empty()) {
        return -1;
    }

    int row = 0;
    while (row < ROWS && !get_token_occupancy(column - 1, row)) {
        row++;
    }

    if (row >= ROWS) {
        return -1;
    }

    set_token(column - 1, row, 0);

    auto [saved_red_win, saved_yellow_win] = win_state_stack.top();
    win_state_stack.pop();
    red_win = saved_red_win;
    yellow_win = saved_yellow_win;

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

std::vector<int> Connect4::get_valid_moves() const {
    std::vector<int> moves;

    for (int c = 0; c < COLUMNS; c++) {
        int row = ROWS - 1;
        while (row >= 0 && get_token_occupancy(c, row)) {
            row--;
        }
        if (row >= 0) {
            moves.push_back(c+1);
        }
    }

    return moves;
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