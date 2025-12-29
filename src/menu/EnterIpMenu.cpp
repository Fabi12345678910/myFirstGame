#include "menu/EnterIpMenu.h"
#include <cctype>
#include <sstream>
#include <iostream>

EnterIpMenu::EnterIpMenu(sf::RenderWindow& win)
: window(win),
  ipText(font, "", 32),
  portText(font, "", 32),
  labelText(font, "Enter IP and Port", 28),
  infoText(font, "Press Enter to confirm, Tab to switch", 20)
{
    set_values();
}

void EnterIpMenu::set_values() {
    ipBuffer.clear();
    portBuffer.clear();
    ip.reset();

    enteringIp = true;
    done = false;

    if (!font.openFromFile("../assets/fonts/PressStart2P-Regular.ttf"))
        std::cout << "failed loading font\n";

    ipText.setPosition({400.f, 250.f});
    portText.setPosition({400.f, 350.f});
    labelText.setPosition({400.f, 180.f});
    infoText.setPosition({400.f, 420.f});

    ipText.setFillColor(sf::Color::White);
    portText.setFillColor(sf::Color::White);
    labelText.setFillColor(sf::Color::Yellow);
    infoText.setFillColor(sf::Color::Cyan);

    ipText.setOutlineColor(sf::Color::Black);
    portText.setOutlineColor(sf::Color::Black);
    labelText.setOutlineColor(sf::Color::Black);
    infoText.setOutlineColor(sf::Color::Black);

    ipText.setOutlineThickness(2);
    portText.setOutlineThickness(2);
}

void EnterIpMenu::loop_events() {
    while (auto ev = window.pollEvent()) {
        const sf::Event& event = *ev;

        if (event.is<sf::Event::Closed>())
            window.close();

        if (auto text = event.getIf<sf::Event::TextEntered>()) {
            char c = static_cast<char>(text->unicode);

            if (c == '\b') {
                if (enteringIp && !ipBuffer.empty()) ipBuffer.pop_back();
                else if (!enteringIp && !portBuffer.empty()) portBuffer.pop_back();
            }
            else if (c == '\r' || c == '\n') {
                if (!ipBuffer.empty() && !portBuffer.empty()) {
                    auto resolved = sf::IpAddress::resolve(ipBuffer);
                    if (resolved.has_value()) {
                        ip = resolved.value();
                        done = true;
                    }
                }
            }
            else if (std::isprint(c)) {
                if (enteringIp && ipBuffer.size() < 64)
                    ipBuffer += c;
                else if (!enteringIp && portBuffer.size() < 5 && std::isdigit(c))
                    portBuffer += c;
            }
        }

        if (auto key = event.getIf<sf::Event::KeyPressed>()) {
            if (key->code == sf::Keyboard::Key::Tab)
                enteringIp = !enteringIp;
        }
    }
}

void EnterIpMenu::draw_all() {
    ipText.setString(std::string("IP: ") + ipBuffer + (enteringIp ? "_" : ""));
    portText.setString(std::string("Port: ") + portBuffer + (!enteringIp ? "_" : ""));

    window.clear(sf::Color(30, 30, 30));
    window.draw(labelText);
    window.draw(ipText);
    window.draw(portText);
    window.draw(infoText);
    window.display();
}

std::pair<sf::IpAddress, unsigned short> EnterIpMenu::run_menu() {
    set_values();

    while (window.isOpen() && !done) {
        loop_events();
        draw_all();
    }

    unsigned short port = 0;
    std::istringstream(portBuffer) >> port;
    return { ip.value(), port };
}
