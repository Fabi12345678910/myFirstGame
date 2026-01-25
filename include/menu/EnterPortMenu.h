#pragma once
#include "WindowMessages.h"
#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include <optional>

class EnterPortMenu {
    sf::RenderWindow& window;
    sf::Font font;
    sf::Text portText, labelText, infoText;
    std::string portBuffer;
    WindowMessages& msgs;
    bool done;

    void set_values();
    void loop_events();
    void draw_all();
public:
    EnterPortMenu(sf::RenderWindow& win, WindowMessages& msgs);
    std::optional<unsigned short> run_menu();
};