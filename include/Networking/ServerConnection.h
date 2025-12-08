#pragma once

#include "Networking/Connection.h"
#include "Types.h"
#include "Inputs.h"
#include <queue>

class ServerConnection : public Connection
{
private:
    OBJECT_ID_TYPE playerId = 0;
    sf::UdpSocket& udpSocket;
    std::queue<indexedPlayerInput> inputQueue;
    TICK_TYPE highestProcessedInput = 0;
public:
    std::optional<indexedPlayerInput> getNextPlayerInput(){
        if(inputQueue.size() == 0){
            return std::nullopt;
        }

        struct indexedPlayerInput nextInput(inputQueue.front());
        highestProcessedInput = nextInput.idx;
        inputQueue.pop();
        return nextInput;
    }

    int enqueueInput(indexedPlayerInput const & input){
        if(inputQueue.size()>= 20){
            return -1;//too much inputs queued
        }
        if(!inputQueue.empty() && inputQueue.back().idx >= input.idx){
            return -2;//input already enqueued
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
