#pragma once
#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include <optional>

class EnterPortMenu {
    sf::RenderWindow& window;
    sf::Font font;
    sf::Text portText, labelText, infoText;
    std::string portBuffer;
    bool done;

    void set_values();
    void loop_events();
    void draw_all();
public:
    EnterPortMenu(sf::RenderWindow& win);
    std::optional<unsigned short> run_menu();
};