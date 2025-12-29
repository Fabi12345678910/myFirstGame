#include <SFML/Graphics.hpp>
#include "menu/Menu.h"
#include "menu/EnterIpMenu.h"
#include "menu/EnterPortMenu.h"
#include "Client.h"
#include "Server.h"
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
    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(1280, 720)), "AdvancedCPP Game");
    window.setFramerateLimit(60);

    Scene currentScene = Scene::MENU;

    std::unique_ptr<Client> client;
    std::unique_ptr<Server> server;
    std::thread serverThread;

    Menu menu(window);

    while (window.isOpen() && currentScene != Scene::EXIT) {
        switch (currentScene) {

        case Scene::MENU: {
            std::string menuResult = menu.run_menu();

            if (menuResult == "Host") {

                EnterPortMenu enterPortMenu(window);
                unsigned short port = enterPortMenu.run_menu();

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

                client = std::make_unique<Client>(window, result.first, result.second);
                currentScene = Scene::CLIENT_LOBBY;
            }
            else {
                currentScene = Scene::EXIT;
            }
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

        default:
            currentScene = Scene::EXIT;
            break;
        }
    }

    return 0;
}
