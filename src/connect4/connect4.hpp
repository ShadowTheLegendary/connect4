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

#include <vector>
#include <array>
#include <stack>

#include "src/transposition_table/transposition_table.hpp"

class Connect4 {
public:
    Connect4();

    Connect4(const Connect4& other);

    int play(int column, int color);

    int undo(int column);

    void output(std::ostream& output);

    std::vector<int> get_valid_moves() const;

    int get_number_of_n_in_a_rows(int n, int color) const;

    unsigned char get_token_occupancy(int column, int row) const;

    unsigned char get_token_color(int column, int row) const;

    bool red_win = false;
    bool yellow_win = false;

    friend class Connect4Bot;

private:

    std::array<unsigned char, 11> grid;
    std::stack<std::pair<bool, bool>> win_state_stack;

    unsigned char get_token(int column, int row) const;

    void set_token(int column, int row, unsigned char token);

    unsigned char make_token(int color) const;

    void update_win(int last_col, int last_row);

};

#endif