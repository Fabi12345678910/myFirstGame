#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network/IpAddress.hpp>
#include <SFML/Network/TcpSocket.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>
#include <mutex>
#include <nlohmann/json.hpp>
#include "Character.h"
#include "Config.h"
#include "Login/LoginTypes.h"
#include "Networking/ClientConnection.h"
#include "Networking/EventDefinitions/Login/EventNewUserCreated.h"
#include "Networking/EventDefinitions/Login/EventRequestCreateUser.h"
#include "Networking/EventDefinitions/Login/EventUserCreationDenied.h"
#include "Types.h"
#include "WindowMessage.h"
#include "menu/AccountMenu.h"
#include "menu/Menu.h"
#include "menu/EnterIpMenu.h"
#include "menu/EnterPortMenu.h"
#include "menu/OptionsMenu.h"
#include "Client.h"
#include "Server.h"
#include "Options.h"
#include <optional>
#include <stdexcept>
#include <string>
#include <thread>
#include <memory>
#include "Logger.h"
#include "plog/Log.h"
#include "Networking/EventDefinitions/Login/EventUserLogin.h"

// Enum for scene management
enum class Scene {
    MENU,
    CLIENT_LOBBY,
    CLIENT_GAME,
    EXIT
};

static void *handleEvent(std::unique_ptr<Event> evPtr, Connection &, void *args){
    struct clientEventHandlerData *handle = (clientEventHandlerData*) args;
    std::lock_guard<std::mutex> queueLockGuard(handle->connectionEventsMutex);
    handle->connectionEventsQueue.push(std::move(evPtr));
    return NULL;
}

struct userData{
    USER_ID_TYPE id;
    USER_CHARACTER_TYPE character;
};

static USER_LOGIN_KEY_TYPE waitForKey(sf::Time&& timeout, clientEventHandlerData& eventData, WindowMessages& msgs){
    sf::Clock timeoutTimer;
    timeoutTimer.restart();
    while (true)
    {
        std::lock_guard<std::mutex> queueLockGuard(eventData.connectionEventsMutex);
        while(!eventData.connectionEventsQueue.empty()){
            auto& evPtr = eventData.connectionEventsQueue.front();
            Event* ev = evPtr.get();
            auto creationDenied = dynamic_cast<EventUserCreationDenied*>(ev);
            if(creationDenied != NULL){
                throw std::runtime_error("user creation denied");
            }

            auto userCreated = dynamic_cast<EventNewUserCreated*>(ev);
            if(userCreated != NULL){
                return userCreated->item;
            }

            // Ignore unrelated events during login to avoid blocking on a non-login packet.
            eventData.connectionEventsQueue.pop();
        }
    }
}

static userData waitForLogin(sf::Time timeout, clientEventHandlerData& eventData, WindowMessages& msgs, Options& opts){

}

int main(int argc, char const *argv[]) {


    initLogger();

    USER_ID_TYPE playerId;
    WindowMessages messageManager = WindowMessages();

    Options opts = load_options("config.json");
    save_options(opts, "config.json");

    bool loggedIn = false;
    auto ip = sf::IpAddress::resolve(LOGIN_SERVER);
    std::unique_ptr<ClientConnection> conn;
    clientEventHandlerData handlerData;

    if(ip.has_value()){

        try {
            conn = std::make_unique<ClientConnection>(*ip, DEFAUL_LOGIN_PORT, sf::IpAddress::LocalHost, 0);
            conn->setArgs(&handlerData);

            conn->setEventHandler(handleEvent);
            if(opts.loginKey != 0){
                PLOG_ERROR << "loggin in using key" << opts.loginKey;
                conn->sendTcpEvent(EventUserLogin(opts.loginKey));
            }
            else{
                PLOG_ERROR << "creating new user";
                conn->sendTcpEvent(EventRequestCreateUser());
                waitForKey(sf::seconds(4), handlerData, messageManager);
            }

            //TODO while timeout not reached, wait for either create new user or user details
        } catch (...) {
            messageManager.storeMessage("login failed", Message::WARNING, sf::seconds(4));
        }        
    }else{
            messageManager.storeMessage("login failed", Message::WARNING, sf::seconds(4));
    }

    sf::State style = opts.fullscreen ? sf::State::Fullscreen : sf::State::Windowed;
    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(opts.width, opts.height)), "AdvancedCPP Game", style);
    window.setFramerateLimit(60);

    Scene currentScene = Scene::MENU;
    std::string username = "anonymous";
    Characters::type character = Characters::ALI; 

    std::unique_ptr<Client> client;
    std::unique_ptr<Server> server;
    std::thread serverThread;


    while (window.isOpen() && currentScene != Scene::EXIT) {
        switch (currentScene) {

        case Scene::MENU: {
            sf::Music menuMusic;
            if (menuMusic.openFromFile("../assets/music/menu.mp3")) {
                menuMusic.setLooping(true);
                menuMusic.play();
                menuMusic.setVolume(opts.music_volume);
                if (opts.music_muted) {
                    menuMusic.setVolume(0);
                }
            }
            Menu menu(window, messageManager); //needs to be inside so graphical changes are applied
            while (window.isOpen()) {
                std::string menuResult = menu.run_menu();
                if (!window.isOpen()) {
                    menuMusic.stop();
                    currentScene = Scene::EXIT;
                    break;
                }
                if(menuResult == "Account"){
                    if(!loggedIn){
                        messageManager.storeMessage("no connection to login server", Message::WARNING, sf::seconds(1.5));
                        PLOG_WARNING << "unable to open account settings, no connection to LoginServer";
                        continue;
                    }else{
                        AccountMenu accMenu(window, username, character);
                        accMenu.run_menu();
                    }
                }
                if (menuResult == "Host") {
                    EnterPortMenu enterPortMenu(window);
                    auto portResult = enterPortMenu.run_menu();
                    if (!portResult) {
                        // User cancelled, show menu again
                        continue;
                    }
                    unsigned short port = *portResult;
                    server = std::make_unique<Server>(port);
                    serverThread = std::thread([&] {
                        server->run();
                    });
                    // Wait for server to be ready
                    while (!server || !server->isReady()) {
                        std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    }
                    client = std::make_unique<Client>(window, sf::IpAddress::LocalHost, port);
                    client->setIsHost(false);
                    currentScene = Scene::CLIENT_LOBBY;
                    break;
                }
                else if (menuResult == "Join") {
                    EnterIpMenu enterIpMenu(window);
                    auto result = enterIpMenu.run_menu();
                    if (!result) {
                        PLOG_ERROR << "something went wrong when entering ip and port";
                        continue;
                    }
                    client = std::make_unique<Client>(window, result->first, result->second);
                    currentScene = Scene::CLIENT_LOBBY;
                    break;
                }
                else if (menuResult == "Options") {
                    OptionsMenu optionsMenu(window, opts);
                    optionsMenu.run_menu(&menuMusic);
                    save_options(opts, "config.json");
                }
                else if (menuResult == "Quit") {
                    currentScene = Scene::EXIT;
                    menuMusic.stop();
                    break;
                }
                else {
                    //menuResult.clear();
                }
            }
            menuMusic.stop();
            break;
        }

        case Scene::CLIENT_LOBBY: {
            if (client) {
                window.setFramerateLimit(0);
                client->run();
                window.setFramerateLimit(60);
            }
            if (serverThread.joinable())
                serverThread.join();

            currentScene = Scene::EXIT;
            break;
        }

        case Scene::EXIT: {
            return 0;
        }

        default:
            currentScene = Scene::EXIT;
            break;
        }
    }

    return 0;
}
