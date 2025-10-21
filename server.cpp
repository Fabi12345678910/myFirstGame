#include "NetworkManager.h"
#include "ServerConnection.h"

void *handleEvents(Event& ev){
    printf("got a new event!!");
    return NULL;
}

void *handleConnection(int socket){
    printf("Got new connection at socket\"%d\"\n", socket);
    ServerConnection *conn = new ServerConnection(socket);
    conn->setEventHandler(handleEvents);
    return NULL;
}

int main(int argc, char const *argv[])
{
    NetworkManager netMan;
    ServerSocket server = netMan.createServer();
    server.setConnectionHandler(handleConnection);
    //server.
//        
//    int connection = accept(server, NULL, NULL);
//    
//    char buffer[1024] = {0};
//    
//    while (true)
//    {
//        recv(connection, buffer, sizeof(buffer), 0);
//        printf("Message from client: %s\n", buffer);
//    }
//    
//    close(server);
    sleep(100);
    return 0;
}
