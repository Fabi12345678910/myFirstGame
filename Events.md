# Events
Event.h provides an "Interface", e.g. abstract base class.\
All objects implementing this can be sent and received via various networking classes

## Implementation of new Events
a new Event `<new_event>` must in general use the following steps to be used in the application:

1. a corresponding `EVENT_TYPE_<new_event>` macro must be created in `events.h`, see "TypeGuidelines"
2. create a new class using superclass `Event`
3. overwrite `sf::Packet toPacket()` as required by class Event. \
Each packet MUST begin with the previously defined `EVENT_TYPE_<new_event>` to differentiate between different Events.
4. to receive the packet, the function `Event& getEventFromPacket(sf::Packet&)` as defined in events.cpp must be expanded to create the new Event from a received Packet\
this usually happens by implementing a Constructor `<new_event>(&sf::Packet)` and adding a new case statement to `getEventFromPacket`

## simple Events
a template for new events will be available, allowing 

## TypeGuidelines
to preserver the application from total chaos, use the following number ranges for new events:
- 0: reserved for yo mama
- 1-999: basic connection information(e.g. login, playerId)
- 1000-1999: client side player updates
- 2000-2999: server side player updates
- 3000-3999: world updates
- 9000-9999: game coordination(e.g. map transmission, votes for new map)