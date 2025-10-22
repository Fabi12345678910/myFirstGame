#ifndef _EVENT_HPP
#define _EVENT_HPP
#include <SFML/Network/Packet.hpp>
#include <iostream>

#define DATATYPE_EVENT_TYPE int16_t
class Event
{
public:
    //creates a network sendable packet from the event
    virtual sf::Packet toPacket() = 0;
    
    //recreates the event from a packet
    Event(sf::Packet&){
        throw std::runtime_error("not implemented");
    };
    virtual ~Event() = default;
protected:
    Event() = default;
};


#endif