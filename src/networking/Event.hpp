#include <cstdlib>
#include <SFML/Network/Packet.hpp>
#ifndef _EVENT_H
#define _EVENT_H
class Event
{
public:
    //returns a pointer to the data of the ClientEvent, which must not be freed
    virtual void* getData() = 0;
    virtual std::size_t getDataSize() = 0;
    virtual int getType() = 0;
};


#endif