#include "../Event.hpp"

class EventDebugMessage:Event
{
private:
    std::string message;
public:
    EventDebugMessage(sf::Packet packet){
        if(!(packet >> message)){
            throw std::runtime_error("failed to read debug message");
        };
    };
    EventDebugMessage(const std::string& msg){
        this->message = msg;
    }
    sf::Packet& toPacket() override{
        sf::Packet packet;
        packet << message;
        return packet;
    }
};