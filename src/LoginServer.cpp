/*what the login server provides:*/
//stores stuff in sqlite
//Login(privateId): {playerId, playerName, (maybe Character aswell)}

//UpdatePlayerName (playerName)

//Queue Find Server -> sends ServerFoundEvent

//Optional UpdateCharacter

//For GameServers: sendGameStat()

//GetStatOverView()
#include "Login/LoginServer.h"
#include "Login/PersistentServerDataProviderMock.h"
#include "Networking/Connection.h"
#include "Networking/Event.h"
#include "Networking/EventDefinitions/Login/EventRequestCreateUser.h"
#include "Networking/EventDefinitions/Login/EventNewUserCreated.h"
#include "Login/PersistentServerDataProviderMock.h"
#include "Login/PersistentServerDataProviderSqlite.h"
#include "Config.h"
#include <memory>

void* loginServerTcpEventHandler(std::unique_ptr<Event> ev, Connection& conn, void* args) {
    struct loginServerEventHandlerData *handle = (loginServerEventHandlerData*) args;
    std::lock_guard<std::mutex> queueLockGuard(handle->connectionEventsMutex);
    ServerConnection* serverConn = dynamic_cast<ServerConnection*>(&conn);
    if(serverConn == NULL){
        PLOG_ERROR << "did not get a server connection";
        return NULL;
    }
    std::tuple<ServerConnection&, std::unique_ptr<Event>> queueEntry(*serverConn, std::move(ev));
    handle->connectionEventsQueue.push(std::move(queueEntry));
    PLOG_VERBOSE_IF(debugServerNetworking) << "handlung tcp event";
    return NULL;
};

void LoginServer::run(){
    serverSocket.setArgs(&eventData);
    serverSocket.setEventHandler(loginServerTcpEventHandler);
    mainLoop();
}

void LoginServer::processEvents(){
    std::lock_guard<std::mutex> queueLockGuard(eventData.connectionEventsMutex);
    while(!eventData.connectionEventsQueue.empty()){
        auto& connEv = eventData.connectionEventsQueue.front();
        ServerConnection& conn = std::get<0>(connEv);
        Event* ev = std::get<1>(connEv).get();
        //somehow handle tha event
        EventRequestCreateUser *evRequestCreateUser = dynamic_cast<EventRequestCreateUser*>(ev);
        if(evRequestCreateUser != NULL){
            auto key = serverDataProvider.createUser();
//            conn.sendTcpEvent(EventNewUserCreated(key));
        }
    }
}

void LoginServer::mainLoop(){
    while (true) {
        processEvents();
    }
}

int main(int argc, char const *argv[]){
    PersistentServerDataProviderMock mockProvider;
    PersistentServerDataProviderSqlite userDb("data.db");
/*    {    // Insert a user and login key
        
        int64_t loginKey = -1; 
        loginKey = userDb.createUser("Alice");
        printf("Login Key: %ld\n", loginKey);

        // Retrieve userId from loginKey
        auto userId = userDb.getUserId(loginKey);
        printf("User ID: %lld\n", (long long)userId);

        // Retrieve userName
        auto name = userDb.getUserName(userId);
        printf("User name: %s\n", name.c_str());

        // Rename user
        userDb.renameUser(userId, "AliceNew");
        printf("Renamed user: %s\n", userDb.getUserName(userId).c_str());
    }*/
    LoginServer(mockProvider, DEFAUL_LOGIN_PORT).run();
}