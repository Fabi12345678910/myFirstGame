#include "ServerSocket.h"

ServerSocket::~ServerSocket(){
    close(socket);
}

int ServerSocket::receiveNextConnection(){
    return accept(socket, NULL, NULL);
}

void* connectionHandlerFunction(void* arg){
    ServerSocket* conn = (ServerSocket*) arg;
    while(true){
        int connection = conn->receiveNextConnection();
        conn->connectionHandler(connection);
    }
}

int ServerSocket::setConnectionHandler(void* handleConnection(int)){
    if(this->connectionHandler != NULL){
        return 1;
    }
    this->connectionHandler = handleConnection;
    pthread_create(&connectionHandlerThread, NULL, connectionHandlerFunction, this);
    return 0;
}