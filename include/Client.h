#pragma once

#include "Renderer.h"
#include "Networking/ClientConnection.h"
#include "Networking/Event.h"
#include "CircularArray.h"
#include "Networking/EventDefinitions/EventGamestatePlayerInputHistory.h"
#include "Networking/EventDefinitions/EventServerHealth.h"
#include "ClientGameStateStore.h"
#include "StageManager.h"
#include "Types.h"

#include <SFML/System/Time.hpp>
#include <optional>
#include <queue>
#include <mutex>
#include <SFML/Graphics.hpp>
#include <unordered_map>

struct clientEventHandlerData{
    std::queue<std::unique_ptr<Event>> connectionEventsQueue;
    std::mutex connectionEventsMutex;
};
struct MapSelectionState {
        TICK_TYPE selectUntil;
        int16_t selectedIndex = 0;
        bool confirmed = false;
        std::vector<std::pair<int16_t, std::string>> maps = StageManager::loadStageList();
};

struct MapSelectionInput{
    bool goLeft = false;
    bool goRight = false;
    bool confirm = false;
};

enum clientState{C_CONNECTING, C_LOBBY, C_MAP_SELECTION, C_COUNTDOWN, C_WAITING_FOR_COUNTDOWN, C_GAME_RUNNING, C_END_OF_ROUND, C_END_OF_GAME};

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
    enum clientState clientState = C_CONNECTING;
    int playerId;
    Renderer renderer;
    std::uint64_t latestGeneratedTick = 0;
    std::uint64_t latestPreRenderedTick = 0;
    std::uint16_t tickrateMs = 100;
    std::unordered_map<OBJECT_ID_TYPE, unsigned short> endOfRoundScoreboard;
    std::optional<OBJECT_ID_TYPE> endOfGameWinnerId = std::nullopt;
    TICK_TYPE tickToDisplay = 0;
    //used by various input readers to avoid spamming selection
    TICK_TYPE lastTickWithSelection = 0;
    HEALTH_INPUT_QUEUE_TYPE serverQueueHealth = 0;
    CircularArray<HEALTH_FRAME_TIME_TYPE, 256> serverFrameTimes = CircularArray<HEALTH_FRAME_TIME_TYPE, 256>();
    CircularArray<HEALTH_FRAME_TIME_TYPE, 256> clientFrameTimes = CircularArray<HEALTH_FRAME_TIME_TYPE, 256>();
    //the targeted Tick to display(higher means)
    
    TICK_TYPE displayTickDifference = 4;
    void performLogin();
    void processEventsPlaying();
    void processEventsAwaitingSpawn();
    void processMapSelectionInputs();
    void renderStateSpecificInfo(bool readyToPlay, const GameState& gameState);
    void mainLoop();
    void updateGameStates(EventGamestatePlayerInputHistory &ev);
    playerInput processInputs();
    MapSelectionInput processInputsMapSelection();
//    CircularArray<ClientGameState, clientGameStateBufferSize> gameStates;
//    GameState gameState = GameState();
    struct clientEventHandlerData eventData;
    ClientConnection conn;

    bool isHost = false;
    MapSelectionState mapSelectionState;
    TICK_TYPE gameStartTick;
    TICK_TYPE showEndOfRoundUntilTick;
    bool isReadyForSelection(){return ((std::int64_t)(tickToDisplay - lastTickWithSelection) * sf::milliseconds(tickrateMs) >= sf::milliseconds(selectionTimeoutMs));};
    void selectedSomething(){lastTickWithSelection = tickToDisplay;};
    void storeInputs(const TICK_TYPE& startingTick, const TICK_TYPE& currentTick, playerInput input);
    void sendInputs(const TICK_TYPE& startingTick, const TICK_TYPE& currentTick);
public:
    void run();
    Client(sf::RenderWindow& win, bool isHost = false);
    Client(sf::RenderWindow& win, sf::IpAddress ip, unsigned short port, bool isHost = false);
    ~Client(){};
    bool const& getIsHost() const{
        return isHost;
    }

    void setIsHost(bool b) {
        isHost = b;
    }
};
