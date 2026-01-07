#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

class Menu {
  int pos;
  bool pressed, theselect;

  sf::RenderWindow& window;
  sf::RectangleShape winclose;
  sf::Font font;

  sf::Vector2i pos_mouse;
  sf::Vector2f mouse_coord;

  std::vector<const char *> options;
  std::vector<sf::Vector2f> coords;
  std::vector<sf::Text> texts;
  std::vector<std::size_t> sizes;

  protected:
    void set_values();
    void loop_events();
    void draw_all();

  public:
    Menu(sf::RenderWindow& win);
    ~Menu();
    std::string run_menu(); // returns selected option
    // Prompts for IP and port in menu style, returns pair<ip, port>
};