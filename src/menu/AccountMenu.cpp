#include "menu/AccountMenu.h"
#include "FontManager.h"
#include <SFML/Window/Keyboard.hpp>
#include <cctype>
#include "Character.h"

AccountMenu::AccountMenu(sf::RenderWindow& win, std::string& userName, Characters::type& character)
: window(win),
  userName(userName),
  character(character),
  font(FontManager::getDefaultFont()),
  playerNameText(font, "", 32),
  portText(font, "", 32),
  labelText(font, "Account Info", 28),
  infoText(font, "Press Enter to confirm, Tab to switch", 20)
{
    set_values();
}

void AccountMenu::set_values() {

    float w = window.getSize().x;
    float h = window.getSize().y;
    float topMargin = 0.25f * h; // start menu 20% from top
    float spacing   = 0.125f * h; // 10% of window height between items
    float textScale = 0.05f;

    enteringName = true;
    done = false;

    int maxCharactes = Characters::size();

    labelText.setCharacterSize(textScale*h);
    playerNameText.setCharacterSize(textScale*h);
    portText.setCharacterSize(textScale*h);
    infoText.setCharacterSize(textScale*h*0.5);

    // Set origins to left edge, vertical center
    sf::FloatRect labelBounds = labelText.getLocalBounds();
    labelText.setOrigin(sf::Vector2f{labelBounds.position.x + labelBounds.size.x/2, labelBounds.position.y + labelBounds.size.y/2});
    sf::FloatRect ipBounds = playerNameText.getLocalBounds();
    playerNameText.setOrigin(sf::Vector2f{ipBounds.position.x + labelBounds.size.x/2, ipBounds.position.y + ipBounds.size.y/2});
    sf::FloatRect portBounds = portText.getLocalBounds();
    portText.setOrigin(sf::Vector2f{portBounds.position.x + labelBounds.size.x/2, portBounds.position.y + portBounds.size.y/2});
    sf::FloatRect infoBounds = infoText.getLocalBounds();
    infoText.setOrigin(sf::Vector2f{infoBounds.position.x + infoBounds.size.x/2, infoBounds.position.y + infoBounds.size.y/2});

    labelText.setPosition({w*0.5f, topMargin + 0 * spacing});
    playerNameText.setPosition({w*0.5f, topMargin + 1 * spacing});
    portText.setPosition({w*0.5f, topMargin + 2 * spacing});
    infoText.setPosition({w*0.5f, topMargin + 5 * spacing});

    playerNameText.setFillColor(sf::Color::White);
    portText.setFillColor(sf::Color::White);
    labelText.setFillColor(sf::Color::Yellow);
    infoText.setFillColor(sf::Color::Cyan);

    playerNameText.setOutlineColor(sf::Color::Black);
    portText.setOutlineColor(sf::Color::Black);
    labelText.setOutlineColor(sf::Color::Black);
    infoText.setOutlineColor(sf::Color::Black);

    playerNameText.setOutlineThickness(2);
    portText.setOutlineThickness(2);
}

void AccountMenu::loop_events() {
    while (auto ev = window.pollEvent()) {
        const sf::Event& event = *ev;

        if (event.is<sf::Event::Closed>())
            window.close();

        if (auto text = event.getIf<sf::Event::TextEntered>()) {
            char c = static_cast<char>(text->unicode);

            if (c == '\b') {
                if (enteringName && !userName.empty()) userName.pop_back();
            }
            else if (c == '\r' || c == '\n') {
                enteringName = false;
            }
            else if (std::isprint(c)) {
                if (enteringName && userName.size() < 64)
                    userName += c;
            }
        }

        if (auto key = event.getIf<sf::Event::KeyPressed>()) {
            if (key->code == sf::Keyboard::Key::Tab)
                enteringName = !enteringName;
            else if (key->code == sf::Keyboard::Key::Escape){
                done = true;
            }
            else if(key->code == sf::Keyboard::Key::Down){
                if(enteringName){
                    enteringName = false;
                }
            }
            else if(key->code == sf::Keyboard::Key::Up){
                if(!enteringName){
                    enteringName = true;
                }
            }
            else if(key->code == sf::Keyboard::Key::Left){
                if(!enteringName){
                    character = Characters::fromInt((Characters::toInt(character) - 1 + Characters::size()) % Characters::size());
                }
            }
            else if(key->code == sf::Keyboard::Key::Right){
                if(!enteringName){
                    character = Characters::fromInt((Characters::toInt(character) + 1) % Characters::size());
                }
            }
        }
    }
}

void AccountMenu::draw_all() {
    playerNameText.setString(std::string("Name: ") + userName + (enteringName ? "_" : ""));
    portText.setString(std::string("Character: ") + (!enteringName ? "<" : "") + std::to_string(Characters::toInt(character)) + (!enteringName ? ">" : ""));

    window.clear(sf::Color(30, 30, 30));
    window.draw(labelText);
    window.draw(playerNameText);
    window.draw(portText);
    window.draw(infoText);
    window.display();
}

bool AccountMenu::run_menu() {
    set_values();

    while (window.isOpen() && !done) {
        loop_events();
        draw_all();
    }

    return true;
}
