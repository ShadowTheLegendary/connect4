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
    
    /*while (true) {
        std::cout << "\n\n" << ((color == RED) ? "red" : "yellow") << "\n";
        game.output(std::cout);

        if (color == YELLOW) {
            std::string input;
            std::cin >> input;

            if (input == "quit") {
                break;
            }

            int column = std::stoi(input);
            if (game.play(column, color) == -1) {
                std::cout << "fail\n";
                continue;
            }
            color = RED;
        }
        else if (color == RED) {
            int move = bot.get_best_move(game, true, std::chrono::seconds(10));
            if (game.play(move, color) == -1) {
                std::cout << "WHAT\n";
                std::cout << move << "\n";
                return -1;
            }
            color = YELLOW;
        }

        if (game.red_win) {
            std::cout << "Red wins!\n";
            return 0;
        }
        if (game.yellow_win) {
            std::cout << "Yellow wins!\n";
            return 0;
        }
    }*/

    return 0;
}
