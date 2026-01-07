#include <menu/Menu.h>
#include <SFML/Graphics.hpp>

Menu::Menu(sf::RenderWindow& win) : window(win) {
  set_values();
}

Menu::~Menu(){
  // No manual delete needed for member variables
}

void Menu::set_values(){
  // Do not create window here; it is managed externally
  // window.create(...);
  // window.setPosition(...);

  pos = 0;
  pressed = theselect = false;
  if(!font.openFromFile("../assets/fonts/PressStart2P-Regular.ttf")) {
    std::cout << "failed loading font\n";
  }

  pos_mouse = {0,0};
  mouse_coord = {0, 0};

  options = {"Host", "Join", "Options", "Quit"};
  coords = {{400,200},{400,300},{400,400},{400,500}};
  sizes = {40,40,40,40};

  texts.clear();
  for (std::size_t i{}; i < options.size(); ++i){
    sf::Text text(font, options[i], sizes[i]);
    text.setOutlineColor(sf::Color::Black);
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

    // pos_mouse = sf::Mouse::getPosition(window);
    // mouse_coord = window.mapPixelToCoords(pos_mouse);

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down) && !pressed){
      if( pos < 3){
        ++pos;
        pressed = true;
        texts[pos].setOutlineThickness(4);
        texts[pos - 1].setOutlineThickness(0);
        pressed = false;
        theselect = false;
      }
    }

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) && !pressed){
      if( pos > 0){
        --pos;
        pressed = true;
        texts[pos].setOutlineThickness(4);
        texts[pos + 1].setOutlineThickness(0);
        pressed = false;
        theselect = false;
      }
    }

    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Enter) && !theselect){
      theselect = true;
      // if( pos == 3){
      //   window.close();
      // }
      std::cout << options[pos] << '\n';
    }

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
  window.display();
}

std::string Menu::run_menu(){
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
//   std::cout << "Selected option: " << selected << std::endl;
//   return 0;
// }