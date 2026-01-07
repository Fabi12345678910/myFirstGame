#include "menu/EnterPortMenu.h"
#include <cctype>
#include <sstream>
#include <iostream>

EnterPortMenu::EnterPortMenu(sf::RenderWindow& win)
: window(win),
  portText(font, "", 32),
  labelText(font, "Enter Port", 28),
  infoText(font, "Press Enter to confirm", 20)
{
    set_values();
}

void EnterPortMenu::set_values() {
    portBuffer.clear();

    float w = window.getSize().x;
    float h = window.getSize().y;
    float topMargin = 0.25f * h; // start menu 20% from top
    float spacing   = 0.125f * h; // 10% of window height between items
    float textScale = 0.05f;

    done = false;

    if (!font.openFromFile("../assets/fonts/PressStart2P-Regular.ttf"))
        std::cout << "failed loading font\n";

    // Set text sizes first
    portText.setCharacterSize(textScale*h);
    labelText.setCharacterSize(textScale*h);
    infoText.setCharacterSize(textScale*h*0.5);

    // Set origins to left edge, vertical center
    sf::FloatRect labelBounds = labelText.getLocalBounds();
    labelText.setOrigin(sf::Vector2f{labelBounds.position.x + labelBounds.size.x/2, labelBounds.position.y + labelBounds.size.y/2});
    sf::FloatRect portBounds = portText.getLocalBounds();
    portText.setOrigin(sf::Vector2f{portBounds.position.x + labelBounds.size.x/2, portBounds.position.y + portBounds.size.y/2});
    sf::FloatRect infoBounds = infoText.getLocalBounds();
    infoText.setOrigin(sf::Vector2f{infoBounds.position.x + infoBounds.size.x/2, infoBounds.position.y + infoBounds.size.y/2});

    // Place so the vertical center is at the middle, left-aligned at window center
    portText.setPosition({w*0.5f, topMargin + 1 * spacing});
    labelText.setPosition({w*0.5f, topMargin + 0 * spacing});
    infoText.setPosition({w*0.5f, topMargin + 5 * spacing});

    portText.setFillColor(sf::Color::White);
    labelText.setFillColor(sf::Color::Yellow);
    infoText.setFillColor(sf::Color::Cyan);

    portText.setOutlineColor(sf::Color::Black);
    labelText.setOutlineColor(sf::Color::Black);
    infoText.setOutlineColor(sf::Color::Black);

    portText.setOutlineThickness(2);
}

void EnterPortMenu::loop_events() {
    while (auto ev = window.pollEvent()) {
        const sf::Event& event = *ev;

        if (event.is<sf::Event::Closed>())
            window.close();

        if (auto text = event.getIf<sf::Event::TextEntered>()) {
            char c = static_cast<char>(text->unicode);

            if (c == '\b') {
                if (!portBuffer.empty()) portBuffer.pop_back();
            }
            else if (c == '\r' || c == '\n') {
                if (!portBuffer.empty()) {
                    done = true;
                }
            }
            else if (std::isdigit(c) && portBuffer.size() < 5) {
                portBuffer += c;
            }
        }

        if (auto key = event.getIf<sf::Event::KeyPressed>()) {
            if (key->code == sf::Keyboard::Key::Escape) {
                done = true;
                portBuffer.clear();
            }
        }
    }
}

void EnterPortMenu::draw_all() {
    portText.setString(std::string("Port: ") + portBuffer + "_");

    window.clear(sf::Color(30, 30, 30));
    window.draw(labelText);
    window.draw(portText);
    window.draw(infoText);
    window.display();
}

std::optional<unsigned short> EnterPortMenu::run_menu() {
    set_values();

    while (window.isOpen() && !done) {
        loop_events();
        draw_all();
    }

    if (portBuffer.empty())
        return std::nullopt;
    unsigned short port = 0;
    std::istringstream(portBuffer) >> port;
    return port;
}
