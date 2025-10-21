#include "Events.h"

void* EventPressLeft::getData(){
    return NULL;
}
size_t EventPressLeft::getDataSize(){
    return 0;
}
int EventPressLeft::getType(){
    return EVENT_TYPE_PRESS_LEFT;
}

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
    Event *ev = NULL;
    switch (event.type)
    {
    case EVENT_TYPE_DEBUG_MESSAGE:{
        ev = new EventDebugMessage(event.dataSize, event.dataBuffer);
    }
    case EVENT_TYPE_PRESS_LEFT:{
        ev = new EventPressLeft();
    }
    default:
        break;
    }
    return *ev;
}