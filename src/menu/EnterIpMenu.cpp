#include "menu/EnterIpMenu.h"
#include "Config.h"
#include "WindowMessages.h"
#include <SFML/System/Time.hpp>
#include <cctype>
#include <sstream>
#include <stdexcept>
#include <string>

EnterIpMenu::EnterIpMenu(sf::RenderWindow& win, WindowMessages& msgs)
: window(win),
  msgs(msgs),
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
    portBuffer=std::to_string(DEFAULT_PORT);
    ip.reset();

    float w = window.getSize().x;
    float h = window.getSize().y;
    float topMargin = 0.25f * h; // start menu 20% from top
    float spacing   = 0.125f * h; // 10% of window height between items
    float textScale = 0.05f;

    enteringIp = true;
    done = false;

    if (!font.openFromFile("../assets/fonts/PressStart2P-Regular.ttf")){
        throw std::runtime_error("failed loading font\n");
    }

    labelText.setCharacterSize(textScale*h);
    ipText.setCharacterSize(textScale*h);
    portText.setCharacterSize(textScale*h);
    infoText.setCharacterSize(textScale*h*0.5);

    // Set origins to left edge, vertical center
    sf::FloatRect labelBounds = labelText.getLocalBounds();
    labelText.setOrigin(sf::Vector2f{labelBounds.position.x + labelBounds.size.x/2, labelBounds.position.y + labelBounds.size.y/2});
    sf::FloatRect ipBounds = ipText.getLocalBounds();
    ipText.setOrigin(sf::Vector2f{ipBounds.position.x + labelBounds.size.x/2, ipBounds.position.y + ipBounds.size.y/2});
    sf::FloatRect portBounds = portText.getLocalBounds();
    portText.setOrigin(sf::Vector2f{portBounds.position.x + labelBounds.size.x/2, portBounds.position.y + portBounds.size.y/2});
    sf::FloatRect infoBounds = infoText.getLocalBounds();
    infoText.setOrigin(sf::Vector2f{infoBounds.position.x + infoBounds.size.x/2, infoBounds.position.y + infoBounds.size.y/2});

    labelText.setPosition({w*0.5f, topMargin + 0 * spacing});
    ipText.setPosition({w*0.5f, topMargin + 1 * spacing});
    portText.setPosition({w*0.5f, topMargin + 2 * spacing});
    infoText.setPosition({w*0.5f, topMargin + 5 * spacing});

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
    const auto confirm = [&]() {
        if (ipBuffer.empty() || portBuffer.empty())
            return;

        auto resolved = sf::IpAddress::resolve(ipBuffer);
        if (resolved.has_value()) {
            ip = resolved.value();
            done = true;
        } else {
            msgs.storeMessage("could not resolve host", Message::WARNING, sf::seconds(1));
        }
    };

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
                confirm();
            }
            else if (std::isprint(c)) {
                if (enteringIp && ipBuffer.size() < 64)
                    ipBuffer += c;
                else if (!enteringIp && portBuffer.size() < 5 && std::isdigit(c))
                    portBuffer += c;
            }
        }

        if (auto key = event.getIf<sf::Event::KeyPressed>()) {
            switch (key->code) {
                case sf::Keyboard::Key::Tab: {
                    enteringIp = !enteringIp;
                    break;
                }
                case sf::Keyboard::Key::Escape: {
                    done = true;
                    ip.reset();
                    break;
                }
                case sf::Keyboard::Key::Enter: {
                    confirm();
                    break;
                }
                default: {
                    break;
                }

            }
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
    msgs.renderStoredMessages(window);
    window.display();
}

std::optional<std::pair<sf::IpAddress, unsigned short>> EnterIpMenu::run_menu() {
    set_values();

    while (window.isOpen() && !done) {
        loop_events();
        draw_all();
    }

    if (!ip.has_value()){
        return std::nullopt;
    }
    unsigned short port = 0;
    std::istringstream(portBuffer) >> port;
    return std::make_pair(ip.value(), port);
}
