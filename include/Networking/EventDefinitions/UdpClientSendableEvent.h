#pragma once

#include "Types.h"
#include "Networking/Event.h"

//this class basically contains the playerId as an identifier
//so that the server can determine the sender even in connectionless udp
class UdpClientSendableEvent : public Event{
    public: OBJECT_ID_TYPE playerId;
    UdpClientSendableEvent(OBJECT_ID_TYPE playerId) : playerId(playerId){}
    UdpClientSendableEvent() : playerId(0){}
};