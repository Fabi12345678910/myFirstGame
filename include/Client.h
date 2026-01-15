#pragma once

#include "GameState.h"
#include "Renderer.h"
#include "Networking/ClientConnection.h"
#include "Networking/Event.h"
#include "CircularArray.h"
#include "Networking/EventDefinitions/EventGamestatePlayerInputHistory.h"
#include "ClientGameStateStore.h"
#include "StageManager.h"

#include <queue>
#include <mutex>
#include <SFML/Graphics.hpp>

struct clientEventHandlerData{
    std::queue<std::unique_ptr<Event>> connectionEventsQueue;
    std::mutex connectionEventsMutex;
};
struct MapSelectionState {
        TICK_TYPE selectUntil;
        int16_t selectedStageId = -1;
        int16_t selectedIndex = 0;
        bool confirmed = false;
        std::vector<std::pair<int16_t, std::string>> maps = StageManager::loadStageList();
};
enum clientState{CONNECTING, PLAYING, AWAITING_SPAWN};

/*struct ClientGameState{
    enum State{
        UNINITIALIZED, READY_FOR_GENERATION, GENERATED
    };
    State state;
    Player localPlayer;
    playerInput input;
    GameState gameState;
    std::vector<struct playerInputWithId> playerInputs;
    ClientGameState() : state(UNINITIALIZED){}
    //uint32_t tick == position in Array
};*/
constexpr unsigned int clientGameStateBufferSize = 64;

class Client
{
private:
    ClientGameStateStore<clientGameStateBufferSize> gameStore = ClientGameStateStore<clientGameStateBufferSize>(32);
    enum clientState clientState = CONNECTING;
    int playerId;
    Renderer renderer;
    std::uint64_t latestGeneratedTick = 0;
    std::uint64_t latestPreRenderedTick = 0;
    std::uint16_t tickrateMs = 100;
    TICK_TYPE tickToDisplay = 0;
    //the targeted Tick to display(higher means)
    TICK_TYPE displayTickDifference = 10;
    gameState lastRenderedGameState = gameState::WAITING;
    void performLogin();
    void processEventsPlaying();
    void processEventsAwaitingSpawn();
    void mainLoop();
    void updateGameStates(EventGamestatePlayerInputHistory &ev);
    playerInput processInputs();
//    CircularArray<ClientGameState, clientGameStateBufferSize> gameStates;
//    GameState gameState = GameState();
    struct clientEventHandlerData eventData;
    ClientConnection conn;

    bool isHost = false;
    MapSelectionState mapSelectionState;

public:
    void run();
    Client(sf::RenderWindow& win);
    Client(sf::RenderWindow& win, sf::IpAddress ip, unsigned short port);
    ~Client(){};
    bool const& getIsHost() const{
        return isHost;
    }

    void setIsHost(bool b) {
        isHost = b;
    }
};
