#include "networking/EventDefinitions/EventDebugMessage.hpp"
#include "networking/Events.hpp"
#include <cstdio>



int main(int argc, char const *argv[])
{
    EventDebugMessage test("sendThatShiiiiit");

    sf::Packet packet = test.toPacket();
    auto ev = getEventFromPacket(packet);
    
    EventDebugMessage *evDebug = dynamic_cast<EventDebugMessage*>(ev.get());
    if(evDebug != NULL){
        std::cout << "debug_message: " << evDebug->getMessage() << "\n";
    }else{
        printf("wasnt a debug message\n");
    }

    printf("haha, works\n");
    return 0;
}
