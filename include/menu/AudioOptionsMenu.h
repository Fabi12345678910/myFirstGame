#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Options.h"
#include <string>
#include <optional>

class AudioOptionsMenu {
    sf::RenderWindow& window;
    sf::Font font;
    sf::Text labelText, infoText, volumeText;
    Options& opts;
    bool done = false;

    void set_values();
    void loop_events();
    void draw_all();
public:
    AudioOptionsMenu(sf::RenderWindow& win, Options& opts);
    void run_menu(sf::Music* music = nullptr);
};
