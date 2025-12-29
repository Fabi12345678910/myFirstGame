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

    done = false;

    if (!font.openFromFile("../assets/fonts/PressStart2P-Regular.ttf"))
        std::cout << "failed loading font\n";

    portText.setPosition({400.f, 350.f});
    labelText.setPosition({400.f, 180.f});
    infoText.setPosition({400.f, 420.f});

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

unsigned short EnterPortMenu::run_menu() {
    set_values();

    while (window.isOpen() && !done) {
        loop_events();
        draw_all();
    }

    unsigned short port = 0;
    std::istringstream(portBuffer) >> port;
    return port;
}
