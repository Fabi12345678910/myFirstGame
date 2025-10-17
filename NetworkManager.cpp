#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>     // for close()
#include <cstring>      // for memset(), etc.
#include <iostream>

#include <Connection.cpp>


class NetworkManager
{
private:
public:
    NetworkManager();
    ~NetworkManager();
    Connection createServer();
    Connection createClient();
};

NetworkManager::NetworkManager(){

}

NetworkManager::~NetworkManager(){

}

Connection NetworkManager::createServer(){
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(serverSocket == -1){
        fprintf(stderr, "servercreation: error creating server socket\n");
        return -1;
    };

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(6969);
    address.sin_addr.s_addr = INADDR_ANY;

    if(bind(serverSocket, (struct sockaddr*) &address, sizeof(address))){
        fprintf(stderr, "servercreation: error binding server socket to network port\n");
        close(serverSocket);
        return -1;
    };
    if(listen(serverSocket, 10)){
        fprintf(stderr, "servercreation: error listening on server socket\n");
        close(serverSocket);
        return -1;
    };
    return Connection(serverSocket);
}

Connection NetworkManager::createClient(){
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    if(clientSocket == -1){
        fprintf(stderr, "servercreation: error creating server socket\n");
        return -1;
    };

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(6969);
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");

    if(connect(clientSocket, (struct sockaddr*) &serverAddress, sizeof(serverAddress))){
        fprintf(stderr, "servercreation: error connecting to server\n");
        close(clientSocket);
        return -1;
    };

    return Connection(clientSocket);
}