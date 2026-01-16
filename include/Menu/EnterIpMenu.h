#pragma once

#include <SFML/Graphics.hpp>

class EnterIpMenu {
    sf::RenderWindow& window;
    sf::Font font;
    sf::Text ipText, portText, labelText, infoText;
    std::string ip, port;
    bool enteringIp, done;

    void set_values();
    void loop_events();
    void draw_all();
public:
    EnterIpMenu(sf::RenderWindow& win);
    ~EnterIpMenu();
    std::pair<std::string, unsigned short> run_menu();
};