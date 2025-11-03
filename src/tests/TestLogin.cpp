#include "Networking/ClientConnection.h"
#include "Networking/EventDefinitions/EventLoginRequest.h"
#include "Networking/EventDefinitions/EventLoginConfirmation.h"
#include "Networking/EventDefinitions/EventLoginDenied.h"

void* eventHandler(std::unique_ptr<Event> ev, Connection& conn, void* args) {
    (void) args;
    std::cout << "got a new event!!\n";
    EventLoginConfirmation *evSuccess = dynamic_cast<EventLoginConfirmation*>(ev.get());
    if(evSuccess != NULL){
        std::cout << "successfully logged in and got id: " << evSuccess->getPlayerId() << '\n';
    }
    EventLoginDenied *evFail = dynamic_cast<EventLoginDenied*>(ev.get());
    if(evFail != NULL){
        std::cout << "login got denied\n";
    }
    return NULL;
}
int main(int argc, char const *argv[])
{
    ClientConnection conn({127, 0, 0, 1});
    conn.sendEvent(EventLoginRequest());
    conn.setEventHandler(eventHandler);
    sleep(1);
    return 0;
}
