#include "Server.h"
#include "Client.h"
#include "Threads/Threads.h"
#include "Logger.h"
#include <SFML/Network/IpAddress.hpp>
pthread_t serverThread;
pthread_t clientThread;

#ifdef _WIN32
int usleep(unsigned long usec){
    Sleep(usec/1000);
    return 0;
}

int sleep(unsigned long sec){
    Sleep(sec * 1000);
    return 0;
}
#endif

void* runServer(void*){
    Server server = Server(4445);
    server.run();
    return NULL;
}

void* runClient(void*){
    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(1920,1080)), "LocalGameClient");
    Client client(window, sf::IpAddress(127,0,0,1), 4445);
    client.run();
    return NULL;
}

int main(int argc, char const *argv[])
{
    initLogger();
    if(pthread_create(&serverThread, NULL, runServer, NULL)){return -1;};
    usleep(100000);
    if(pthread_create(&clientThread, NULL, runClient, NULL)){return -2;};
    if(pthread_join(clientThread, NULL)){return -3;};
    if(pthread_join(serverThread, NULL)){return -4;}

    return 0;
}
