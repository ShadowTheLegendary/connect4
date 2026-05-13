#pragma once

#ifndef CONNECT4_HPP
#define CONNECT4_HPP

//////////////////////////////////
// 0x00000000
// lsb:
// 0 - empty
// 1 - full
//
// 2lsb:
// 0 - red
// 1 - yellow
//
// 4 tokens per bit
//
//////////////////////////////////

#define RED 0
#define YELLOW 1

#define EMPTY 0
#define FULL 1

#define OCCUPANCY_MASK 0b01
#define COLOR_MASK 0b10

#define TOKEN_MASK 0b00000011

#define COLUMNS 7
#define ROWS 6

#include <iostream>
#include <limits>
#include <vector>
#include <array>
#include <chrono>

#include "src/transposition_table/transposition_table.hpp"

class Connect4 {
public:
    Connect4();

    Connect4(const Connect4& other) {
        for (int i = 0; i < 11; ++i) {
            grid[i] = other.grid[i];
        }
        red_win = other.red_win;
        yellow_win = other.yellow_win;
    }   

    int play(int column, int color);

    void output(std::ostream& output);

    std::vector<int> get_valid_moves() const {
        std::vector<int> moves;

        for (int c = 0; c < COLUMNS; c++) {
            // Check if there's an empty spot in this column by starting from the bottom
            int row = ROWS - 1;
            while (row >= 0 && get_token_occupancy(c, row)) {
                row--;
            }
            // If we found an empty spot, column is valid
            if (row >= 0) {
                moves.push_back(c+1);
            }
        }

        return moves;
    }

    int get_number_of_n_in_a_rows(int n, int color) const;

    unsigned char get_token_occupancy(int column, int row) const;

    unsigned char get_token_color(int column, int row) const;

    bool red_win = false;
    bool yellow_win = false;

    friend class Connect4Bot;

private:

    std::array<unsigned char, 11> grid;

    unsigned char get_token(int column, int row) const;

    void set_token(int column, int row, unsigned char token);

    unsigned char make_token(int color) const;

    void update_win(int last_col, int last_row);

};

class Connect4Bot {
public:
    int evaluate(const Connect4& position) const;

    int minimax(const Connect4& position, unsigned int depth, int alpha, int beta, bool maximizing_player, std::chrono::steady_clock::time_point deadline) const;

    int get_best_move(const Connect4& position, bool maximizing_player, std::chrono::milliseconds time_limit);

private:
    mutable TranspositionTable tt;

};

#endif