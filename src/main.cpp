#include <SFML/Graphics.hpp>

#include <iostream>
#include <string>
#include <cstdio>
#include <chrono>

#include "transposition_table/transposition_table.hpp"
#include "connect4/connect4.hpp"
#include "connect4bot/connect4bot.hpp"
#include "connect4gui/connect4gui.hpp"

void comms_protocol() {
    register_zobrist();

    Connect4 game;
    int color = YELLOW;
    Connect4Bot bot;

    std::string input;
    int last_move;

    while (std::getline(std::cin, input)) {
        if (input == "readyup") {
            std::cout << "ready\n";
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

            int move = bot.get_best_move(game, maximizing_player, std::chrono::seconds(1));
            game.play(move, color);
            std::cout << move << "\n";
            last_move = move;
            color = (color==RED) ? YELLOW : RED;
        }
        else if (input == "output") {
            game.output(std::cout);
        }
        else if (input == "undo") {
            if (game.undo(last_move) == -1) {
                std::cout << "noundo\n";
                continue;
            }
            std::cout << "undone\n";
        }
        else {
            try {
                int column = std::stoi(input);
                if (game.play(column, color) == -1) {
                    std::cout << "badmove\n";
                    continue;
                }
                last_move = column;
                color = (color==RED) ? YELLOW : RED;
                std::cout << "okaymove\n";
            }
            catch (const std::invalid_argument&) {
                std::cout << "badinput\n";
            }
        }

        if (game.red_win) {
            std::cout << "rwin\n";
        }
        else if (game.yellow_win) {
            std::cout << "ywin\n";
        }
        else if (game.get_valid_moves().size() == 0) {
            std::cout << "tie\n";
        }
    };
}

void gui() {
    register_zobrist();

    sf::RenderWindow window(sf::VideoMode({800, 600}), "My window");

    sf::Texture turn_indicator("assets/token.png");
    sf::Sprite turn_indicator_sprite(turn_indicator);
    turn_indicator_sprite.setPosition({650, 25});

    sf::Texture win_indicator("assets/token_crown.png");
    sf::Sprite win_indicator_sprite(win_indicator);
    win_indicator_sprite.setPosition({725, 25});

    Connect4 game;
    Connect4GUI gui(game);
    int color = YELLOW;
    Connect4Bot bot;

    bool wait = true;

    while (window.isOpen()) {
        sf::Vector2f mouse_position = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        int column = static_cast<int>(((mouse_position.x - 50.f) * 2) / 100) + 1;

        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->code == sf::Keyboard::Key::Enter) {
                    color = RED;
                }
            }
            else if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseButtonPressed->button == sf::Mouse::Button::Left) {
                    if ((column > 0) and (column < 8) and (color == YELLOW)) {
                        if (game.play(column, YELLOW) != -1) {
                            color = RED;
                        }
                    }
                }
            }
        }

        if (game.red_win) {
            win_indicator_sprite.setColor(sf::Color::Red);
        }
        else if (game.yellow_win) {
            win_indicator_sprite.setColor(sf::Color::Yellow);
        }

        switch (color) {
            case RED:
            turn_indicator_sprite.setColor(sf::Color::Red);
            break;
            case YELLOW:
            turn_indicator_sprite.setColor(sf::Color::Yellow);
            break;
        }

        if (color == RED) {
            if (wait) {
                wait = false;
            }
            else {
                int move = bot.get_best_move(game, true, std::chrono::seconds(5));
                game.play(move, RED);
                color = YELLOW;
                wait = true;
            }
        }

        window.clear(sf::Color(200, 200, 200));

        gui.draw(window);
        window.draw(win_indicator_sprite);
        window.draw(turn_indicator_sprite);

        window.display();
    }
}

int main() {
    gui();
}