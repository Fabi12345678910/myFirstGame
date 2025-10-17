#include "NetworkManager.cpp"
#include <threads.h>

int main(int argc, char const *argv[])
{
    NetworkManager netMan;
    int clientSocket = netMan.createClient();
    const char* message = "Hello, server!";
    send(clientSocket, message, strlen(message), 0);
    sleep(5);
    send(clientSocket, message, strlen(message), 0);
    close(clientSocket);
    return 0;
}
