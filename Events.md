# Events
Event.h provides an "Interface", e.g. abstract base class.\
All objects implementing this can be sent and received via various networking classes

## Implementation of new Events
a new Event `<new_event>` must in general use the following steps to be used in the application:

1. a corresponding `EVENT_TYPE_<new_event>` macro must be created in `EventTypeList.hpp`, see "TypeGuidelines"
2. create a new class using superclass `Event` (similar to `EventDebugMessage.hpp`)
3. implement the functions `sf::Packet <new_event>::toPacket()`, `<new_event>(sf::Packet packet)`(Constructor from packet). Ensure the packet created by `toPacket` starts with EVENT_TYPE_<new_event>;
4. to receive the packet, the function `Event& getEventFromPacket(sf::Packet&)` as defined in events.cpp must be expanded to create the new Event from a received Packet\
this usually happens by adding a new case statement to `getEventFromPacket`
5. create any (optional) arbitrary constructor used in the application and perhaps some getter;

## simple Events
a template for new events will be available, allowing 

## TypeGuidelines
to preserve the application from total chaos, use the following number ranges for new events:
- 0: reserved for yo mama
- 1-999: basic connection information(e.g. login, playerId)
- 1000-1999: client side player updates
- 2000-2999: server side player updates
- 3000-3999: world updates
- 9000-9999: game coordination(e.g. map transmission, votes for new map)