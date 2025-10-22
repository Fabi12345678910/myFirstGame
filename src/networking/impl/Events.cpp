#include "../Events.hpp"
#include <memory>

#include "../EventDefinitions/EventDebugMessage.hpp"


std::unique_ptr<Event> getEventFromPacket(sf::Packet& packet){
    DATATYPE_EVENT_TYPE event_type;
    if(!(packet >> event_type)){
        throw std::runtime_error("invalid Packet received");
    };

    switch (event_type)
    {
        case EVENT_TYPE_DEBUG_MESSAGE:
            return std::make_unique<EventDebugMessage>(packet);    
        default:
            throw std::runtime_error("invalid event type");
    }
}