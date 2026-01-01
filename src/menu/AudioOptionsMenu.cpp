#include "menu/AudioOptionsMenu.h"
#include "Options.h"
#include <iostream>
#include <sstream>

AudioOptionsMenu::AudioOptionsMenu(sf::RenderWindow& win, Options& options)
    : window(win), opts(options),
      labelText(font, "Audio Options", 32), infoText(font, "Left/Right: Volume  Enter: Mute/Unmute  Esc: Back", 18), volumeText(font, "", 32)
{
    set_values();
}

void AudioOptionsMenu::set_values() {
    if (!font.openFromFile("../assets/fonts/PressStart2P-Regular.ttf"))
        std::cout << "failed loading font\n";

    labelText.setFillColor(sf::Color::Yellow);
    labelText.setOutlineColor(sf::Color::Black);
    labelText.setOutlineThickness(2);
    labelText.setPosition({400.f, 180.f});

    infoText.setFillColor(sf::Color::Cyan);
    infoText.setOutlineColor(sf::Color::Black);
    infoText.setOutlineThickness(1);
    infoText.setPosition({200.f, 420.f});

    volumeText.setFillColor(sf::Color::White);
    volumeText.setOutlineColor(sf::Color::Black);
    volumeText.setOutlineThickness(2);
    volumeText.setPosition({400.f, 300.f});
}

void AudioOptionsMenu::loop_events() {
    while (auto ev = window.pollEvent()) {
        const sf::Event& event = *ev;
        if (event.is<sf::Event::Closed>())
            window.close();
        if (auto key = event.getIf<sf::Event::KeyPressed>()) {
            switch (key->code) {
                case sf::Keyboard::Key::Left:
                    if (opts.volume > 0) {
                        opts.volume = opts.volume - 5.f;
                    }
                    break;
                case sf::Keyboard::Key::Right:
                    if (opts.volume < 100) {
                        opts.volume = opts.volume + 5.f;
                    }
                    break;
                case sf::Keyboard::Key::Enter:
                    opts.muted = !opts.muted;
                    break;
                case sf::Keyboard::Key::Escape:
                    done = true;
                    break;
                default:
                    break;
            }
        }
    }
}

void AudioOptionsMenu::draw_all() {
    window.clear(sf::Color(25, 25, 28));
    window.draw(labelText);
    window.draw(infoText);
    std::ostringstream oss;
    if (opts.muted || opts.volume == 0.f) {
        oss << "Volume: Muted";
    } else {
        oss << "Volume: " << static_cast<int>(opts.volume) << "%";
    }
    volumeText.setString(oss.str());
    window.draw(volumeText);
    window.display();
}

void AudioOptionsMenu::run_menu(sf::Music* music) {
    done = false;
    bool cancelled = false;
    while (window.isOpen() && !done) {
        loop_events();
        if (music) {
            if (opts.muted || opts.volume == 0.f) {
                music->setVolume(0.f);
            } else {
                music->setVolume(opts.volume);
            }
        }
        draw_all();
        sf::sleep(sf::milliseconds(16));
        if (done && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) {
            cancelled = true;
        }
    }
    if (cancelled) return;
}
