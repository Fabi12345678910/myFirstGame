#include "Networking/EventDefinitions/EventDebugMessage.h"
#include "Networking/Events.h"
#include <cstdio>
#include <iostream>



int main(int argc, char const *argv[])
{
    EventDebugMessage test("sendThatShiiiiit");

    sf::Packet packet = test.toPacket();
    auto ev = getEventFromPacket(packet);
    
    EventDebugMessage *evDebug = dynamic_cast<EventDebugMessage*>(ev.get());
    if(evDebug != NULL){
        std::cout << "debug_message: " << evDebug->message << "\n";
    }else{
        printf("wasnt a debug message\n");
    }

    printf("haha, works\n");
    return 0;
}
