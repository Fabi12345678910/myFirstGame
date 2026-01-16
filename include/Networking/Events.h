#pragma once
#include "Networking/Event.h"

#include <memory>
#include <SFML/Network/Packet.hpp>

std::unique_ptr<Event> getEventFromPacket(sf::Packet& packet);