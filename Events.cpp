#include "Event.cpp"
#include <cstring>


#define EVENT_TYPE_DEBUG_MESSAGE 1
#define EVENT_TYPE_PRESS_LEFT 101
#define EVENT_TYPE_PRESS_RIGHT 102

class EventPressLeft: public Event{
    void* getData() override;
    size_t getDataSize() override;
    int getType() override;
};

void* EventPressLeft::getData(){
    return NULL;
}
size_t EventPressLeft::getDataSize(){
    return 0;
}
int EventPressLeft::getType(){
    return EVENT_TYPE_PRESS_LEFT;
}

class EventDebugMessage: public Event
{
private:
    char* msg;
    size_t msgLength;
public:
    EventDebugMessage(size_t msgLength, const char* content);
    ~EventDebugMessage();
    void* getData() override;
    size_t getDataSize() override;
    int getType() override;
};

EventDebugMessage::EventDebugMessage(size_t contentLength, const char* content)
{
    msg = (char*) malloc(sizeof(*msg) * contentLength);
    memcpy(msg, content, contentLength);
    msgLength = contentLength;
}

EventDebugMessage::~EventDebugMessage()
{
    free(msg);
}

void* EventDebugMessage::getData(){
    return msg;
}

size_t EventDebugMessage::getDataSize(){
    return msgLength;
}

int EventDebugMessage::getType(){
    return EVENT_TYPE_DEBUG_MESSAGE;
}

Event& createEventFromEventData(struct event event){
    switch (event.type)
    {
    case EVENT_TYPE_DEBUG_MESSAGE:{
        EventDebugMessage ev(event.dataSize, event.dataBuffer);
        return ev;    
    }
    case EVENT_TYPE_PRESS_LEFT:{
        EventPressLeft ev;
        return ev;
    }
    default:
        break;
    }
}