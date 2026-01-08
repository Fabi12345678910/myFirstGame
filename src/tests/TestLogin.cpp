#include "Networking/ClientConnection.h"
#include "Networking/EventDefinitions/EventLoginRequest.h"
#include "Networking/EventDefinitions/EventLoginConfirmation.h"
#include "Networking/EventDefinitions/EventLoginDenied.h"

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

void* eventHandler(std::unique_ptr<Event> ev, Connection& conn, void* args) {
    (void) args;
    std::cout << "got a new event!!\n";
    EventLoginConfirmation *evSuccess = dynamic_cast<EventLoginConfirmation*>(ev.get());
    if(evSuccess != NULL){
        std::cout << "successfully logged in and got id: " << evSuccess->playerId << '\n';
    }
    EventLoginDenied *evFail = dynamic_cast<EventLoginDenied*>(ev.get());
    if(evFail != NULL){
        std::cout << "login got denied\n";
    }
    return NULL;
}
int main(int argc, char const *argv[])
{
    ClientConnection conn = ClientConnection::createClientConnection({100, 75, 135, 22}, 30567);
    conn.sendTcpEvent(EventLoginRequest(conn.getUdpPort()));
    conn.setEventHandler(eventHandler);
    sleep(1);
    return 0;
}
