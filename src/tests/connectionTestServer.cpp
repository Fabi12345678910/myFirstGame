#include "../networking/ServerSocket.hpp"
#include "../networking/EventDefinitions/EventDebugMessage.hpp"

void *handleEvents(std::unique_ptr<Event> ev, Connection& conn, void* args){
    (void) args;
    std::cout << "got a new event!!\n";
    EventDebugMessage *evDebug = dynamic_cast<EventDebugMessage*>(ev.get());
    if(evDebug != NULL){
        std::cout << "debug message: " << evDebug->getMessage() << '\n';
    }else{
        std::cout << "wasnt a debug message\n";
    }
    return NULL;
}

int main(int argc, char const *argv[])
{
    ServerSocket sock(42069);
    sock.setEventHandler(handleEvents);

    std::cout << "this tester will automatically quit after 20 seconds, call connnectionTestClient to send a debug message\n";
    sleep(20);
    return 0;
}
