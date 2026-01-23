#include "Networking/Events.h"
#include "Networking/EventTypeList.h"
#include <memory>

#include "Networking/EventDefinitions/EventDebugMessage.h"
#include "Networking/EventDefinitions/EventLoginRequest.h"
#include "Networking/EventDefinitions/EventLoginConfirmation.h"
#include "Networking/EventDefinitions/EventLoginDenied.h"
#include "Networking/EventDefinitions/EventPlayerLocation.h"
#include "Networking/EventDefinitions/EventPlayerVelocity.h"
#include "Networking/EventDefinitions/EventSpawnNewPlayer.h"
#include "Networking/EventDefinitions/EventUserInput.h"
#include "Networking/EventDefinitions/EventGamestatePlayerInputHistory.h"
#include "Networking/EventDefinitions/EventSelectMap.h"
#include "Networking/EventDefinitions/EventSelectedMap.h"
#include "Networking/EventDefinitions/EventEndOfRound.h"
#include "Networking/EventDefinitions/EventEndOfGame.h"
#include "Networking/EventDefinitions/EventGoToLobby.h"
#include "Networking/EventDefinitions/EventStartGame.h"
#include "Networking/EventDefinitions/EventServerHealth.h"


std::unique_ptr<Event> getEventFromPacket(sf::Packet& packet){
    DATATYPE_EVENT_TYPE event_type;
    if(!(packet >> event_type)){
        throw std::runtime_error("invalid Packet received");
    };

    switch (event_type)
    {
        case EVENT_TYPE_DEBUG_MESSAGE:
            return std::make_unique<EventDebugMessage>(packet);    
        case EVENT_TYPE_LOGIN_REQUEST:
            return std::make_unique<EventLoginRequest>(packet);
        case EVENT_TYPE_LOGIN_CONFIRMATION:
            return std::make_unique<EventLoginConfirmation>(packet);
        case EVENT_TYPE_LOGIN_DENIED:
            return std::make_unique<EventLoginDenied>(packet);
        case EVENT_TYPE_PLAYER_LOCATION:
            return std::make_unique<EventPlayerLocation>(packet);
        case EVENT_TYPE_PLAYER_VELOCITY:
            return std::make_unique<EventPlayerVelocity>(packet);
        case EVENT_TYPE_SPAWN_NEW_PLAYER:
            return std::make_unique<EventSpawnNewPlayer>(packet);
        case EVENT_TYPE_USER_INPUT:
            return std::make_unique<EventUserInput>(packet);
        case EVENT_TYPE_GAMESTATE_PLAYERINPUT_HISTORY:
            return std::make_unique<EventGamestatePlayerInputHistory>(packet);
        case EVENT_TYPE_SELECT_MAP:
            return std::make_unique<EventSelectMap>(packet);
        case EVENT_TYPE_SELECTED_MAP:
            return std::make_unique<EventSelectedMap>(packet);
        case EVENT_TYPE_END_OF_ROUND:
            return std::make_unique<EventEndOfRound>(packet);
        case EVENT_TYPE_END_OF_GAME:
            return std::make_unique<EventEndOfGame>(packet);
        case EVENT_TYPE_START_GAME:
            return std::make_unique<EventStartGame>(packet);
        case EVENT_TYPE_GO_TO_LOBBY:
            return std::make_unique<EventGoToLobby>(packet);
        case EVENT_TYPE_SERVER_HEALTH:
            return std::make_unique<EventServerHealth>(packet);
        default:
            throw std::runtime_error("invalid event type");
    }
}