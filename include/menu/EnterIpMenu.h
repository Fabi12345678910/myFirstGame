#pragma once
#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>
#include <optional>

class EnterIpMenu {
    sf::RenderWindow& window;
    sf::Font font;
    sf::Text ipText, portText, labelText, infoText;
    std::optional<sf::IpAddress> ip;
    std::string ipBuffer;
    std::string portBuffer;
    bool enteringIp, done;

    void set_values();
    void loop_events();
    void draw_all();
public:
    EnterIpMenu(sf::RenderWindow& win);
    //~EnterIpMenu();
    std::optional<std::pair<sf::IpAddress, unsigned short>> run_menu();
};