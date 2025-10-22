#include "NetworkManager.h"
#include <iostream>

NetworkManager::NetworkManager(){
    fprintf(stderr, "constructing NetworkManager\n");
}

NetworkManager::~NetworkManager(){
    fprintf(stderr, "destructing NetworkManager\n");
}
//
ServerSocket NetworkManager::createServer(){
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(serverSocket == -1){
        fprintf(stderr, "servercreation: error creating server socket\n");
        exit(EXIT_FAILURE);
    };

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(6969);
    address.sin_addr.s_addr = INADDR_ANY;

    if(bind(serverSocket, (struct sockaddr*) &address, sizeof(address))){
        close(serverSocket);
        throw std::runtime_error("servercreation: error binding server socket to network port");
    };
    if(listen(serverSocket, 10)){
        close(serverSocket);
        throw std::runtime_error("servercreation: error listening on server socket\n");
    };
    return ServerSocket(serverSocket);
}

ClientConnection NetworkManager::createClient(){
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    if(clientSocket == -1){
        fprintf(stderr, "servercreation: error creating client socket\n");
        exit(EXIT_FAILURE);
    };

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(6969);
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");

    if(connect(clientSocket, (struct sockaddr*) &serverAddress, sizeof(serverAddress))){
        fprintf(stderr, "servercreation: error connecting to server\n");
        close(clientSocket);
        throw std::runtime_error("connection failed");
    };

    return ClientConnection(clientSocket);
}