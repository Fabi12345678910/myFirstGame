#pragma once
#include "FontManager.h"
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>
#include <set>
#include <string>

struct Message{
    std::string msg;
    enum Style{
        WARNING,
        INFO
    } style;
    sf::Clock timer;
    sf::Time displayTime;
    Message();
    Message(std::string msg, Message::Style style, sf::Time displayTime): msg(msg), style(style), displayTime(displayTime){
        timer.start();
    };

    bool operator<(Message const& other) const
    {
        if (style != other.style)
            return style < other.style;

        return msg < other.msg;
    }
};

class WindowMessages
{
private:
    std::set<Message> storedMessages;
public:
    WindowMessages(){};
    ~WindowMessages(){};
    void storeMessage(const std::string & msg, Message::Style style, sf::Time&& displayTime){
        storedMessages.emplace(msg, style, displayTime);
    }

    void renderStoredMessages(sf::RenderWindow& window){
        for (auto it = storedMessages.begin(); it != storedMessages.end(); it++) {
            if(it->timer.getElapsedTime() > it->displayTime){
                renderSingleMessage(window, it->msg, it->style);
            }
            else{
                storedMessages.erase(it);
            };
        }
    }
    static void renderSingleMessage(sf::RenderWindow& window, std::string const & msg, Message::Style style){
    constexpr float padding      = 10.f;
    constexpr float margin        = 15.f;
    constexpr unsigned int charSz = 20;

    // Configure text
    sf::Text text(FontManager::getDefaultFont(), msg, charSz);

    // Style-dependent colors
    sf::Color backgroundColor;
    sf::Color textColor;

    switch (style)
    {
        case Message::WARNING:
            backgroundColor = sf::Color(160, 40, 40, 255); // opaque red
            textColor       = sf::Color::White;
            break;

        case Message::INFO:
        default:
            backgroundColor = sf::Color(40, 40, 40, 255);  // opaque dark gray
            textColor       = sf::Color::White;
            break;
    }

    text.setFillColor(textColor);

    // Compute text bounds
    const sf::FloatRect textBounds = text.getLocalBounds();

    // Background rectangle (opaque overlay)
    sf::RectangleShape background;
    background.setSize(sf::Vector2f(
        textBounds.size.x + padding * 2.f,
        textBounds.size.y + padding * 2.f
    ));
    background.setFillColor(backgroundColor);

    // Top-right positioning (respecting current view)
    const sf::View& view = window.getView();
    const sf::Vector2f viewSize   = view.getSize();
    const sf::Vector2f viewCenter = view.getCenter();

    const float rightEdge = viewCenter.x + viewSize.x * 0.5f;
    const float topEdge   = viewCenter.y - viewSize.y * 0.5f;

    const sf::Vector2f boxPosition(
        rightEdge - background.getSize().x - margin,
        topEdge + margin
    );

    background.setPosition(boxPosition);

    // Adjust text position (account for glyph bounds offset)
    text.setPosition(
        sf::Vector2f(
            boxPosition.x + padding - textBounds.position.x,
            boxPosition.y + padding - textBounds.position.y
        )
    );

    // Draw
    window.draw(background);
    window.draw(text);
    }
};