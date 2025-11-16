#include "Networking/ServerSocket.h"
#include "Networking/EventDefinitions/EventDebugMessage.h"
#include "Networking/EventDefinitions/EventLoginRequest.h"

ServerSocket sock(42069);

void *handleEvents(std::unique_ptr<Event> ev, Connection& conn, void* args){
    (void) args;
    ServerConnection* serverConn = dynamic_cast<ServerConnection*>(&conn);
    std::cout << "got a new event!!\n";
    EventDebugMessage *evDebug = dynamic_cast<EventDebugMessage*>(ev.get());
    if(evDebug != NULL){
        std::cout << "debug message: " << evDebug->message << '\n';
    }else{
        std::cout << "wasnt a debug message\n";
    }

    EventLoginRequest *evLoginRequest = dynamic_cast<EventLoginRequest*>(ev.get());
    if(evLoginRequest != NULL){

        std::cout<< "got a login request\n";
        serverConn->udpRecipientPort = evLoginRequest->udpPort;
    }
    return NULL;
}

void* udpEventHandler(std::unique_ptr<Event>, std::optional<sf::IpAddress>& remoteAddress, unsigned short& remotePort, void* args){
    std::cout << "got a new udp event!!\n";
    if(remoteAddress.has_value()){
        std::cout << "remoteAdress: " << remoteAddress.value() << '\n';
    }
    std::cout << "remotePort: " << remotePort << '\n';
    for (auto& conn : sock.connections)
    {
        if(conn->udpRecipientIpAdress == remoteAddress && conn->udpRecipientPort == remotePort){
            std::cout << "sender found in connection list, responding with udp debug message\n";
            conn->sendUdpEvent(EventDebugMessage("responce to udp packet"));
        }else{
            std::cout << "didnt match with connection " << conn->udpRecipientIpAdress << ", port " << conn->udpRecipientPort << '\n';
        }
    }
    
    return NULL;
}


int main(int argc, char const *argv[])
{
    sock.setEventHandler(handleEvents);
    sock.setUdpEventHandler(udpEventHandler);
    std::cout << "this tester will automatically quit after 20 seconds, call connnectionTestClient to send a debug message\n";
    sleep(20);
    return 0;
}
