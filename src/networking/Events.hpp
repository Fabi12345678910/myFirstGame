#ifndef _EVENTS_HPP
#define _EVENTS_HPP
#include "Event.hpp"
#include "EventTypeList.hpp"

#include <memory>
#include <SFML/Network/Packet.hpp>

std::unique_ptr<Event> getEventFromPacket(sf::Packet& packet);

#endif