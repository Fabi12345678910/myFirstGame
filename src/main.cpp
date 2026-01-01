#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <nlohmann/json.hpp>
#include "menu/Menu.h"
#include "menu/EnterIpMenu.h"
#include "menu/EnterPortMenu.h"
#include "menu/AudioOptionsMenu.h"
#include "Client.h"
#include "Server.h"
#include "Options.h"
#include <thread>
#include <memory>
#include <atomic>

// Enum for scene management
enum class Scene {
    MENU,
    CLIENT_LOBBY,
    CLIENT_GAME,
    EXIT
};

int main() {
    Options opts = load_options("config.json");
    save_options(opts, "config.json");
    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(opts.width, opts.height)), "AdvancedCPP Game");
    window.setFramerateLimit(60);

    Scene currentScene = Scene::MENU;

    std::unique_ptr<Client> client;
    std::unique_ptr<Server> server;
    std::thread serverThread;

    Menu menu(window);

    float audioVolume = 100.f;
    bool audioMuted = false;
    while (window.isOpen() && currentScene != Scene::EXIT) {
        switch (currentScene) {

        case Scene::MENU: {
            sf::Music menuMusic;
            if (menuMusic.openFromFile("../assets/music/menu.mp3")) {
                menuMusic.setLooping(true);
                menuMusic.play();
                menuMusic.setVolume(opts.volume);
                if (opts.muted) {
                    menuMusic.setVolume(0);
                }
            }
            while (window.isOpen()) { // <-- check window.isOpen() in menu loop
                std::string menuResult = menu.run_menu();
                if (!window.isOpen()) {
                    menuMusic.stop();
                    currentScene = Scene::EXIT;
                    break;
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
                    client->setIsHost(true);
                    currentScene = Scene::CLIENT_LOBBY;
                }
                else if (menuResult == "Join") {
                    EnterIpMenu enterIpMenu(window);
                    auto result = enterIpMenu.run_menu();
                    if (!result) {
                        continue;
                    }
                    client = std::make_unique<Client>(window, result->first, result->second);
                    currentScene = Scene::CLIENT_LOBBY;
                }
                else if (menuResult == "Options") {
                    AudioOptionsMenu audioOptionsMenu(window, opts);
                    audioOptionsMenu.run_menu(&menuMusic);
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
                client->run();
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
