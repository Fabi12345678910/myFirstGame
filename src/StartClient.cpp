#include "Client.h"
int main(int argc, char const *argv[])
{
    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(1280, 720)), "Client");
    Client client(window);
    client.run();
    return 0;
}