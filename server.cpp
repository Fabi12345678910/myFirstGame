
#include "NetworkManager.cpp"


int main(int argc, char const *argv[])
{
    NetworkManager netMan;
    int server = netMan.createServer();
        
    int connection = accept(server, NULL, NULL);
    
    char buffer[1024] = {0};
    
    while (true)
    {
        recv(connection, buffer, sizeof(buffer), 0);
        printf("Message from client: %s\n", buffer);
    }
    
    close(server);

    return 0;
}
