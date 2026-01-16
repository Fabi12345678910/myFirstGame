#pragma once

#include <SFML/Graphics/Font.hpp>
#include <stdexcept>


class FontManager{
public:
    //not implemented yet 
    //sf::Font& getFont(std::string path);

    FontManager(const FontManager&) = delete;
    FontManager& operator=(const FontManager&) = delete;
    FontManager(FontManager&&) = delete;
    FontManager& operator=(FontManager&&) = delete;

    static sf::Font& getDefaultFont(){
        return instance().defaultFont;
    }

private:
    static FontManager& instance(){
        static thread_local FontManager instance{};
        return instance;
    }
    sf::Font defaultFont;
    FontManager(){
        if(!defaultFont.openFromFile("../assets/fonts/PressStart2P-Regular.ttf")) {
            throw std::runtime_error("failed loading font");
        }};
    ~FontManager(){};
};