#include "menu/OptionsMenu.h"
#include "Options.h"
#include <iostream>
#include <sstream>
#include <algorithm>

OptionsMenu::OptionsMenu(sf::RenderWindow& win, Options& options, WindowMessages& msgs)
    : window(win), opts(options), msgs(msgs)/*
      labelText(font, "Options", 32), infoText(font, "Left/Right: Volume  Enter: Mute/Unmute  Esc: Back", 18), volumeText(font, "", 32)*/
{
    set_values();
}

void OptionsMenu::set_values() {

    float w = window.getSize().x;
    float h = window.getSize().y;
    float topMargin = 0.25f * h; // start menu 20% from top
    float spacing   = 0.125f * h; // 10% of window height between items
    float textScale = 0.05f;
    optionsSize = h*textScale;
    bufferWidth = opts.width;
    bufferHeight = opts.height;
    pos = 0;
    pressed = theselect = false;

    if (!font.openFromFile("../assets/fonts/PressStart2P-Regular.ttf")){
                throw std::runtime_error("failed loading font");
    }

    options = {"Music", "Resolution", "Fullscreen"};
    coords.resize(options.size());

    coords.clear();
    for (std::size_t i = 0; i < options.size(); ++i) {
            coords.push_back({ w * 0.5f, topMargin + i * spacing });
    }

    texts.clear();
    for (std::size_t i{}; i < options.size(); ++i){
        sf::Text text(font, options[i], optionsSize);
        text.setOutlineColor(sf::Color::Black);
        sf::FloatRect bounds = text.getLocalBounds();
        // Right-align in the middle: set origin to right edge, center vertically
        text.setOrigin(sf::Vector2f{bounds.position.x + bounds.size.x, bounds.position.y + bounds.size.y / 2});
        // Place at center X, appropriate Y
        text.setPosition(sf::Vector2f{w * 0.5f, topMargin + i * spacing});
        texts.push_back(text);
    }
    if (!texts.empty())
            texts[0].setOutlineThickness(2);
}

void OptionsMenu::loop_events() {
    static const std::vector<sf::VideoMode> resolutions = sf::VideoMode::getFullscreenModes();
    while (auto ev = window.pollEvent()) {
        const sf::Event& event = *ev;
        if (event.is<sf::Event::Closed>())
            window.close();
        if (auto key = event.getIf<sf::Event::KeyPressed>()) {
            switch (key->code) {
                case sf::Keyboard::Key::Up:
                    if (pos > 0) {
                        texts[pos].setOutlineThickness(0);
                        pos--;
                        texts[pos].setOutlineThickness(2);
                    }
                    break;
                case sf::Keyboard::Key::Down:
                    if (pos + 1 < options.size()) {
                        texts[pos].setOutlineThickness(0);
                        pos++;
                        texts[pos].setOutlineThickness(2);
                    }
                    break;
                case sf::Keyboard::Key::Left:
                    if (pos == 0) { // Music
                        if (opts.music_volume > 0 && !opts.music_muted) {
                            opts.music_volume = opts.music_volume - 5.f;
                        }
                    } else if (pos == 1) { // Resolution
                        // Find current resolution index
                        auto it = std::find_if(resolutions.begin(), resolutions.end(), [&](const sf::VideoMode& vm) {
                            return static_cast<int>(vm.size.x) == bufferWidth && static_cast<int>(vm.size.y) == bufferHeight;
                        });
                        int idx = (it != resolutions.end()) ? std::distance(resolutions.begin(), it) : 0;
                        idx = (idx + 1) % resolutions.size(); // LEFT = smaller
                        bufferWidth = resolutions[idx].size.x;
                        bufferHeight = resolutions[idx].size.y;
                        resolutionChanged = true;
                    } else if (pos == 2) { // Fullscreen
                        opts.fullscreen = false;
                    }
                    break;
                case sf::Keyboard::Key::Right:
                    if (pos == 0) { // Music
                        if (opts.music_volume < 100 && !opts.music_muted) {
                            opts.music_volume = opts.music_volume + 5.f;
                        }
                    } else if (pos == 1) { // Resolution
                        auto it = std::find_if(resolutions.begin(), resolutions.end(), [&](const sf::VideoMode& vm) {
                            return static_cast<int>(vm.size.x) == bufferWidth && static_cast<int>(vm.size.y) == bufferHeight;
                        });
                        int idx = (it != resolutions.end()) ? std::distance(resolutions.begin(), it) : 0;
                        idx = (idx - 1 + resolutions.size()) % resolutions.size(); // RIGHT = bigger
                        bufferWidth = resolutions[idx].size.x;
                        bufferHeight = resolutions[idx].size.y;
                        resolutionChanged = true;
                    } else if (pos == 2) { // Fullscreen
                        opts.fullscreen = true;
                    }
                    break;
                case sf::Keyboard::Key::Enter:
                    if (pos == 0) {
                        opts.music_muted = !opts.music_muted;
                    } else if (pos == 1 && resolutionChanged) {
                        opts.width = bufferWidth;
                        opts.height = bufferHeight;
                        window.create(sf::VideoMode(sf::Vector2u{static_cast<unsigned int>(opts.width), static_cast<unsigned int>(opts.height)}), "AdvancedCPP Game", opts.fullscreen ? sf::State::Fullscreen : sf::State::Windowed);
                        set_values();
                        resolutionChanged = false;
                    } else if (pos == 2) {
                        opts.fullscreen = !opts.fullscreen;
                        window.create(sf::VideoMode(sf::Vector2u{static_cast<unsigned int>(opts.width), static_cast<unsigned int>(opts.height)}), "AdvancedCPP Game", opts.fullscreen ? sf::State::Fullscreen : sf::State::Windowed);
                        set_values();
                    }
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

void OptionsMenu::draw_all() {
    window.clear(sf::Color(25, 25, 28));
    for (std::size_t i = 0; i < texts.size(); ++i) {
        std::ostringstream value;
        if (i == 0) {
            if (opts.music_muted) {
                value << ": Muted";
            } else {
                value << ": " << static_cast<int>(opts.music_volume) << "%";
            }
        } else if (i == 1) {
            value << ": " << bufferWidth << "x" << bufferHeight;
        } else if (i == 2) {
            value << ": [" << (opts.fullscreen ? "X" : " ") << "]";
        }
        sf::Text t = texts[i];
        t.setString(std::string(options[i]) + value.str());
        if (i == pos) {
            t.setFillColor(sf::Color::Red);
            t.setOutlineThickness(1);
        } else {
            t.setFillColor(sf::Color::White);
        }
        window.draw(t);
    }
    // Show hint if resolution was changed but not yet applied
    if (pos == 1 && resolutionChanged) {
        sf::Text hint(font, "Press Enter to apply", 20);
        hint.setFillColor(sf::Color::Cyan);
        hint.setOutlineColor(sf::Color::Black);
        hint.setOutlineThickness(1);
        hint.setPosition(sf::Vector2f{window.getSize().x * 0.5f, window.getSize().y * 0.9f});
        sf::FloatRect bounds = hint.getLocalBounds();
        hint.setOrigin(sf::Vector2f{bounds.position.x + bounds.size.x / 2, bounds.position.y + bounds.size.y / 2});
        window.draw(hint);
    }

    msgs.renderStoredMessages(window);

    window.display();
}

void OptionsMenu::run_menu(sf::Music* music) {
    done = false;
    bool cancelled = false;
    while (window.isOpen() && !done) {
        loop_events();
        if (music) {
            if (opts.music_muted || opts.music_volume == 0.f) {
                music->setVolume(0.f);
            } else {
                music->setVolume(opts.music_volume);
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
