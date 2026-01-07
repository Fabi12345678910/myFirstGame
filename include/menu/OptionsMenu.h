#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Options.h"
#include <string>
#include <optional>

class OptionsMenu {
    sf::RenderWindow& window;
    std::vector<sf::VideoMode> resolutions = sf::VideoMode::getFullscreenModes();
    sf::Font font;
    // sf::Text labelText, /*infoText,*/ volumeText;
    std::vector<const char *> options;
    std::vector<sf::Text> texts;
    std::vector<sf::Vector2f> coords;
    size_t optionsSize;

    Options& opts;
    unsigned short pos;
    bool pressed;
    bool theselect;
    bool done = false;
    bool resolutionChanged = false;
    int bufferWidth = opts.width;
    int bufferHeight = opts.height;


    void set_values();
    void loop_events();
    void draw_all();
public:
    OptionsMenu(sf::RenderWindow& win, Options& opts);
    void run_menu(sf::Music* music = nullptr);
};
