#pragma once
#include "Character.h"
#include <SFML/Network.hpp>
#include <SFML/Graphics.hpp>

class AccountMenu {
    sf::RenderWindow& window;
    sf::Font& font;
    sf::Text playerNameText, portText, labelText, infoText;
    std::string& userName;
    Characters::type& character;
    bool enteringName, done;

    void set_values();
    void loop_events();
    void draw_all();
public:
    AccountMenu(sf::RenderWindow& win, std::string& userName, Characters::type& character);
    //~EnterIpMenu();
    bool run_menu();
};