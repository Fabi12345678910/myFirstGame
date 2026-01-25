#include "Client.h"
#include "Logger.h"
#include "Config.h"
#include <SFML/Network/IpAddress.hpp>
#include <climits>

int main(int argc, char const *argv[])
{
    unsigned short port = DEFAULT_PORT;
    sf::IpAddress ip = sf::IpAddress::LocalHost;
    if(argc > 1){
        auto maybeIp = sf::IpAddress::resolve(argv[1]);
        if(!maybeIp.has_value()){
            fprintf(stderr, "Invalid IP adress\n");
            return -2;
        }else{
            ip = maybeIp.value();
        }
    }
    if(argc > 2){
        int iPort = atoi(argv[2]);
        if (iPort > USHRT_MAX || iPort <= 0){
            fprintf(stderr, "Invalid Port\n");
            return -1;
        }else{
            port = iPort;
        }
    }

    initLogger();
    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(1280, 720)), "Client");
    Client client(window, ip, port, false);
    client.run();
    return 0;
    
}