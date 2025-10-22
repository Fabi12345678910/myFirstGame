#ifndef _EVENTS_H
#define _EVENTS_H
#include "Event.h"
#include <cstring>


#define EVENT_TYPE_DEBUG_MESSAGE 1
#define EVENT_TYPE_PRESS_LEFT 101
#define EVENT_TYPE_PRESS_RIGHT 102

class EventPressLeft: public Event{
    void* getData() override;
    size_t getDataSize() override;
    int getType() override;
};

class EventDebugMessage: public Event
{
private:
    char* msg;
    size_t msgLength;
public:
    EventDebugMessage(const char* content, size_t msgLength);
    EventDebugMessage(const char* content);
    ~EventDebugMessage();
    char* getMessage();
    void* getData() override;
    size_t getDataSize() override;
    int getType() override;
};

Event& createEventFromEventData(struct event event);

#endif