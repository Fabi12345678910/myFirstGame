#ifndef LOG_LEVEL
    #define LOG_LEVEL plog::verbose
#endif

#include "Client.h"
#include "Logger.h"

int main(int argc, char const *argv[])
{
    initLogger();
    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(1280, 720)), "Client");
    Client client(window);
    client.run();
    return 0;
}