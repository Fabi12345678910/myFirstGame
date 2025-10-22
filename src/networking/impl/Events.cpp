#include "../Events.hpp"

Event& getEventFromPacket(sf::Packet& packet){
    DATATYPE_EVENT_TYPE event_type;
    if(!(packet >> event_type)){
        throw std::runtime_error("invalid Packet received");
    };

    switch (event_type)
    {
    case EVENT_TYPE_DEBUG_MESSAGE:
        //return 
        break;
    
    default:
        break;
    }
}