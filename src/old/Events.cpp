#include "Events.h"
#include <stdio.h>

void* EventPressLeft::getData(){
    return NULL;
}
size_t EventPressLeft::getDataSize(){
    return 0;
}
int EventPressLeft::getType(){
    return EVENT_TYPE_PRESS_LEFT;
}
EventDebugMessage::EventDebugMessage(const char* content){
    msgLength = strlen(content)+1;
    msg = (char*) malloc(sizeof(*msg) * msgLength);
    memcpy(msg, content, msgLength);
}
EventDebugMessage::EventDebugMessage(const char* content, size_t contentLength)
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

char* EventDebugMessage::getMessage(){
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
    printf("createEventDebug: %d\n", event.type);
    switch (event.type)
    {
    case EVENT_TYPE_DEBUG_MESSAGE:{
        printf("aha, debug message\n");
        ev = new EventDebugMessage(event.dataBuffer, event.dataSize);
        printf("ev type: %d\n", ev->getType());
        break;
    }
    case EVENT_TYPE_PRESS_LEFT:{
        ev = new EventPressLeft();
        break;
    }
    default:
        break;
    }
    return *ev;
}