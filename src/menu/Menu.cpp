#include "menu/Menu.h"
#include "plog/Log.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <stdexcept>

Menu::Menu(sf::RenderWindow& win, WindowMessages& msgs) : window(win), msgs(msgs){
  set_values();
}

Menu::~Menu(){
  // No manual delete needed for member variables
}

void Menu::set_values(){
  
  float w = window.getSize().x;
  float h = window.getSize().y;
  float topMargin = 0.25f * h; // start menu 20% from top
  float spacing   = 0.125f * h; // 10% of window height between items
  float textScale = 0.05f;

  pos = 0;
  pressed = theselect = false;
  if(!font.openFromFile("../assets/fonts/PressStart2P-Regular.ttf")) {
    throw std::runtime_error("failed loading font");
  }

  pos_mouse = {0,0};
  mouse_coord = {0, 0};

  options = {"Join public", "Host", "Join", "Options", "Quit"};
  coords = {{w*0.5f,150},{w*0.5f,250},{w*0.5f,350},{w*0.5f,450},{w*0.5f,550}};
  sizes = {40,220,40,40,40};

  coords.clear();
  for (std::size_t i = 0; i < options.size(); ++i) {
    coords.push_back({ w * 0.5f, topMargin + i * spacing });
  }

  sizes.clear();
  for (std::size_t i = 0; i < options.size(); ++i) {
    sizes.push_back(h*textScale);
  }

  texts.clear();
  for (std::size_t i{}; i < options.size(); ++i){
    sf::Text text(font, options[i], sizes[i]);
    text.setOutlineColor(sf::Color::Black);
    sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(sf::Vector2f(bounds.position.x + bounds.size.x / 2, bounds.position.y + bounds.size.y / 2));
    text.setPosition(coords[i]);
    texts.push_back(text);
  }
  texts[1].setOutlineThickness(4);
  pos = 1;

  winclose.setSize(sf::Vector2f(23,26));
  winclose.setPosition(sf::Vector2f(1178,39));
  winclose.setFillColor(sf::Color::Transparent);
}

void Menu::loop_events(){
  while(const std::optional<sf::Event> maybeEvent = window.pollEvent()){
    const sf::Event& event = *maybeEvent;
    if(event.is<sf::Event::Closed>()){
      window.close();
    }

    if(event.is<sf::Event::Resized>()) {
      set_values();
    }


    if (auto key = event.getIf<sf::Event::KeyPressed>()) {
      switch (key->code) {
        case sf::Keyboard::Key::Down:{
          if( pos < 4){
            ++pos;
            pressed = true;
            texts[pos].setOutlineThickness(4);
            texts[pos - 1].setOutlineThickness(0);
            pressed = false;
            theselect = false;
          }
          break;
        }
        case sf::Keyboard::Key::Up:{
          if( pos > 0){
            --pos;
            pressed = true;
            texts[pos].setOutlineThickness(4);
            texts[pos + 1].setOutlineThickness(0);
            pressed = false;
            theselect = false;
          }
          break;
        }
        case sf::Keyboard::Key::Enter:{
          theselect = true;
          PLOG_DEBUG << options[pos] << '\n';
          break;
        }
        default:
          break;
      }
    }
    // pos_mouse = sf::Mouse::getPosition(window);
    // mouse_coord = window.mapPixelToCoords(pos_mouse);

    // if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)){
    //   if(winclose.getGlobalBounds().contains(mouse_coord)){
    //     window.close();
    //   }
    // }

    // highlight selected
    for (std::size_t i = 0; i < texts.size(); ++i) {
        if (i == pos)
            texts[i].setFillColor(sf::Color::Red);
        else
            texts[i].setFillColor(sf::Color::White);
    }
  }
}

void Menu::draw_all(){
  window.clear();
  for(auto t : texts){
   window.draw(t); 
  }
  msgs.renderStoredMessages(window);
  window.display();
}

std::string Menu::run_menu(){
  theselect = false;
  while(window.isOpen()){
    loop_events();
    draw_all();
    if (theselect) break;
  }
  return options[pos];
}

// int main() {
//   sf::RenderWindow window(sf::VideoMode(sf::Vector2u(1280, 720)), "Menu Test");
//   Menu menu(window);
//   std::string selected = menu.run_menu();
//   return 0;
// }