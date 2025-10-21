#ifndef _NETWORK_MANAGER_H
#define _NETWORK_MANAGER_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <stdexcept>

#include "ClientConnection.h"
#include "ServerSocket.h"


class NetworkManager
{
private:
public:
    NetworkManager();
    ~NetworkManager();
    ServerSocket createServer();
    ClientConnection createClient();
};
#endif