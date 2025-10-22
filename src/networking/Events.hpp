#ifndef _EVENTS_HPP
#define _EVENTS_HPP
#include "Event.hpp"
#include <SFML/Network/Packet.hpp>

#define EVENT_TYPE_DEBUG_MESSAGE 1

Event& getEventFromPacket(sf::Packet&);

#endif