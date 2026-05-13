#include <iostream>
#include <string>
#include <cstdio>
#include <chrono>

#include "transposition_table/transposition_table.hpp"
#include "connect4/connect4.hpp"

int main() {
    register_zobrist();

    Connect4 game;
    int color = YELLOW;
    Connect4Bot bot;

    std::string input;
    while (std::getline(std::cin, input)) {
        if (input == "readyup") {
            std::cout << "> ready\n";
            game = Connect4();
            color = YELLOW;
        }
        else if (input == "move") {
            bool maximizing_player;
            switch (color) {
                case YELLOW:
                maximizing_player = false;
                break;
                case RED:
                maximizing_player = true;
                break;
            }

            int move = bot.get_best_move(game, maximizing_player, std::chrono::seconds(30));
            game.play(move, color);
            std::cout << "> " << move << "\n";
            color = (color==RED) ? YELLOW : RED;
        }
        else if (input == "output") {
            game.output(std::cout);
        }
        else {
            try {
                int column = std::stoi(input);
                if (game.play(column, color) == -1) {
                    std::cout << "> badmove\n";
                    continue;
                }
                color = (color==RED) ? YELLOW : RED;
                std::cout << "> okaymove\n";
            }
            catch (const std::invalid_argument&) {
                std::cout << "> badinput\n";
            }
        }

        if (game.red_win) {
            std::cout << "Red wins!\n";
        }
        else if (game.yellow_win) {
            std::cout << "Yellow wins!\n";
        }
        else if (game.get_valid_moves().size() == 0) {
            std::cout << "Tie\n";
        }
    };

    return 0;
}
