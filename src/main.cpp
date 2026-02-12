#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network/IpAddress.hpp>
#include <SFML/System/Time.hpp>
#include <exception>
#include <nlohmann/json.hpp>
#include "Config.h"
#include "WindowMessages.h"
#include "menu/Menu.h"
#include "menu/EnterIpMenu.h"
#include "menu/EnterPortMenu.h"
#include "menu/OptionsMenu.h"
#include "Client.h"
#include "Server.h"
#include "Options.h"
#include <thread>
#include <memory>
#include "Logger.h"
#include "plog/Log.h"

enum class Scene {
    MENU,
    CLIENT_LOBBY,
    CLIENT_GAME,
    EXIT
};

int main() {
    initLogger();

    WindowMessages msgs;

    Options opts = load_options("config.json");
    save_options(opts, "config.json");
    sf::State style = opts.fullscreen ? sf::State::Fullscreen : sf::State::Windowed;
    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(opts.width, opts.height)), "AdvancedCPP Game", style);
    window.setFramerateLimit(60);

    Scene currentScene = Scene::MENU;

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
            while (window.isOpen()) { 
                Menu menu(window, msgs);
                std::string menuResult = menu.run_menu();
                if (!window.isOpen()) {
                    menuMusic.stop();
                    currentScene = Scene::EXIT;
                    break;
                }
                if (menuResult == "Join public"){
                    auto ip = sf::IpAddress::resolve(PUBLIC_SERVER);
                    if(ip.has_value()){
                        try {
                            client = std::make_unique<Client>(window, *ip, PUBLIC_PORT);
                            currentScene = Scene::CLIENT_LOBBY;
                        } catch (std::exception& e) {
                            msgs.storeMessage(e.what(), Message::WARNING, sf::seconds(2));
                        }
                        break;
                    }else{
                        msgs.storeMessage("unable to find public server", Message::WARNING, sf::seconds(1.5));
                    }
                }
                if (menuResult == "Host") {
                    EnterPortMenu enterPortMenu(window, msgs);
                    auto portResult = enterPortMenu.run_menu();
                    if (!portResult) {
                        continue;
                    }
                    unsigned short port = *portResult;
                    try {
                        server = std::make_unique<Server>(port);
                        serverThread = std::thread([&] {
                            server->run();
                        });
                    } catch (std::exception& e) {
                        msgs.storeMessage(e.what(), Message::WARNING, sf::seconds(1.5));
                        continue;
                    }
                    
                    while (!server || !server->isReady()) {
                        sf::sleep(sf::milliseconds(10));
                    }
                    try{
                        client = std::make_unique<Client>(window, sf::IpAddress::LocalHost, port);
                    } catch(std::exception& e){
                        msgs.storeMessage(e.what(), Message::WARNING, sf::seconds(1.5));
                        continue;
                    }
                    client->setIsHost(true);
                    currentScene = Scene::CLIENT_LOBBY;
                    break;
                }
                else if (menuResult == "Join") {
                    EnterIpMenu enterIpMenu(window, msgs);
                    auto result = enterIpMenu.run_menu();
                    if (!result) {
                        PLOG_ERROR << "something went wrong when entering ip and port";
                        continue;
                    }
                    try{
                        PLOG_ERROR << "creating tha clienta";
                        client = std::make_unique<Client>(window, result->first, result->second);
                    } catch (std::exception& e) {
                        PLOG_ERROR << "client creation failed";
                        msgs.storeMessage(e.what(), Message::WARNING, sf::seconds(2));
                        continue;
                    }
                    currentScene = Scene::CLIENT_LOBBY;
                    break;
                }
                else if (menuResult == "Options") {
                    OptionsMenu optionsMenu(window, opts, msgs);
                    optionsMenu.run_menu(&menuMusic);
                    save_options(opts, "config.json");
                }
                else if (menuResult == "Quit") {
                    currentScene = Scene::EXIT;
                    menuMusic.stop();
                    break;
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
