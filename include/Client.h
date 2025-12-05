#pragma once

#include "GameState.h"
#include "Renderer.h"
#include "Networking/ClientConnection.h"
#include "Networking/Event.h"
#include "CircularArray.h"
#include "Networking/EventDefinitions/EventGamestatePlayerInputHistory.h"
#include "ClientGameStateStore.h"

#include <queue>
#include <mutex>
struct clientEventHandlerData{
    std::queue<std::unique_ptr<Event>> connectionEventsQueue;
    std::mutex connectionEventsMutex;
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
public:
    void run();
    Client();
    ~Client(){};
};
