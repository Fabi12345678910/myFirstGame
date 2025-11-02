#include "../Events.hpp"
#include <memory>

#include "../EventDefinitions/EventDebugMessage.hpp"
#include "../EventDefinitions/EventLoginRequest.hpp"
#include "../EventDefinitions/EventLoginConfirmation.hpp"
#include "../EventDefinitions/EventLoginDenied.hpp"


std::unique_ptr<Event> getEventFromPacket(sf::Packet& packet){
    DATATYPE_EVENT_TYPE event_type;
    if(!(packet >> event_type)){
        throw std::runtime_error("invalid Packet received");
    };

    switch (event_type)
    {
        case EVENT_TYPE_DEBUG_MESSAGE:
            return std::make_unique<EventDebugMessage>(packet);    
        case EVENT_TYPE_LOGIN_REQUEST:
            return std::make_unique<EventLoginRequest>(packet);
        case EVENT_TYPE_LOGIN_CONFIRMATION:
            return std::make_unique<EventLoginConfirmation>(packet);
        case EVENT_TYPE_LOGIN_DENIED:
            return std::make_unique<EventLoginDenied>(packet);
        default:
            throw std::runtime_error("invalid event type");
    }
}