#include "Networking/ClientConnection.h"
#include "Networking/EventDefinitions/EventDebugMessage.h"
#include "Networking/EventDefinitions/EventLoginRequest.h"

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

void* udpEventHandler(std::unique_ptr<Event> ev, std::optional<sf::IpAddress>& remoteAddress, unsigned short& remotePort, void* args){
    std::cout << "got a new udp event!!\n";
    if(remoteAddress.has_value()){
        std::cout << "remoteAdress: " << remoteAddress.value() << '\n';
    }
    std::cout << "remotePort: " << remotePort << '\n';
    EventDebugMessage *evDebug = dynamic_cast<EventDebugMessage*>(ev.get());
    if(evDebug != NULL){
        std::cout << "debug message: " << evDebug->message << '\n';
    }else{
        std::cout << "wasnt a debug message\n";
    }
    return NULL;
}

int main(int argc, char const *argv[])
{
    ClientConnection conn = ClientConnection::createClientConnection({127, 0, 0, 1}, 42069);
    conn.setUdpEventHandler(udpEventHandler);
    conn.sendTcpEvent(EventDebugMessage("hello from connectionTestClient"));
    conn.sendTcpEvent(EventLoginRequest(conn.getUdpPort()));
    sleep(1);
    conn.sendUdpEvent(EventDebugMessage("udp hello from connectionTestClient"));
    sleep(1);
    return 0;
}
