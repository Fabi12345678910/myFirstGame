#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>

class AudioOptionsMenu {
    sf::RenderWindow& window;
    sf::Font font;
    sf::Text labelText, infoText, volumeText;
    float volume = 100.f;
    bool muted = false;
    bool done = false;

    void set_values();
    void loop_events();
    void draw_all();
public:
    AudioOptionsMenu(sf::RenderWindow& win);
    void setVolume(float v);
    float getVolume() const;
    bool isMuted() const;
    void setMuted(bool m);
    void run_menu(sf::Music* music = nullptr);
};
