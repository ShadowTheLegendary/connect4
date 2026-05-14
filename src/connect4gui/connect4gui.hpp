#pragma once

#ifndef CONNECT4GUI_HPP
#define CONNECT4GUI_HPP

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include "src/connect4/connect4.hpp"

class Connect4GUI {
public:
    Connect4GUI(Connect4& game) : game(&game), connect4board("assets/connect4board.png"), token("assets/token.png") {}

    void draw(sf::RenderTarget& target) {
        sf::Sprite connect4board_sprite(connect4board);
        sf::Sprite token_sprite(token);

        for (int r = 0; r < ROWS; r++) {
            for (int c = 0; c < COLUMNS; c++) {
                if (game->get_token_occupancy(c, r)) {
                    switch (game->get_token_color(c, r)) {
                        case RED:
                        token_sprite.setColor(sf::Color::Red);
                        break;
                        case YELLOW:
                        token_sprite.setColor(sf::Color::Yellow);
                        break;
                    }

                    token_sprite.setPosition(
                        sf::Vector2f(c, r) * 50.f + sf::Vector2f(50, 100)
                    );

                    target.draw(token_sprite);
                }
            }
        }

        target.draw(connect4board_sprite);
    }

private:
    Connect4* game;
    sf::Texture connect4board;
    sf::Texture token;

};

#endif