#pragma once

#include "Networking/Connection.h"
#include "Types.h"
#include "Inputs.h"
#include "plog/Log.h"
#include <queue>

class ServerConnection : public Connection
{
private:
    OBJECT_ID_TYPE playerId = 0;
    sf::UdpSocket& udpSocket;
    std::queue<indexedPlayerInput> inputQueue = std::queue<indexedPlayerInput>();
    TICK_TYPE highestProcessedInput = 0;
public:

    std::optional<indexedPlayerInput> getNextPlayerInput(){
        PLOG_VERBOSE << this <<", queue size: " << inputQueue.size();
        if(inputQueue.size() == 0){
            return std::nullopt;
        }

        indexedPlayerInput nextInput(inputQueue.front());
        highestProcessedInput = nextInput.idx;
        inputQueue.pop();
        return nextInput;
    }

    std::size_t getInputQueueSize(){
        return inputQueue.size();
    }

    int enqueueInput(indexedPlayerInput const & input){
        PLOG_VERBOSE << this <<", queue size: " << inputQueue.size();
        if(inputQueue.size()>= 6){
            PLOG_VERBOSE << "too many inputs enqueued";
            return -1;
        }
        if(!inputQueue.empty() && inputQueue.back().idx >= input.idx){
            PLOG_VERBOSE << "input already enqueued";
            return -2;
        }
        inputQueue.emplace(input);
        return 0;
    }
    TICK_TYPE getHighestProcessedInput(){
        return highestProcessedInput;
    }

    sf::IpAddress udpRecipientIpAdress;
    unsigned short udpRecipientPort;
    ServerConnection(std::unique_ptr<sf::TcpSocket> tcpSocket, sf::UdpSocket& udpSocket, sf::IpAddress udpRecipientIpAdress, unsigned short udpRecipientPort, void* eventHandlerArgs = NULL)
        : Connection(std::move(tcpSocket), eventHandlerArgs), udpSocket(udpSocket), udpRecipientIpAdress(udpRecipientIpAdress), udpRecipientPort(udpRecipientPort) {
    }
    OBJECT_ID_TYPE getPlayerId(){
        return playerId;
    }
    void setPlayerId(OBJECT_ID_TYPE id){
        playerId = id;
    }
    void sendUdpEvent(const Event& ev){
        sf::Packet pack = ev.toPacket();
        if(udpSocket.send(pack, udpRecipientIpAdress, udpRecipientPort) != sf::Socket::Status::Done){
            throw std::runtime_error("error sending event");
        }
    }
};
