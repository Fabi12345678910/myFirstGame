#include "Event.cpp"
#include <sys/socket.h>
#include <unistd.h>

class Connection
{
private:
    int connectionSocket;
    Event& receiveNextEvent();
public:
    Connection(int socket):connectionSocket(socket){

    };
    ~Connection();
    int sendEvent(Event& event);
    int setEventHandler(void* handleEvent(Event&));
};


Connection::~Connection()
{
    close(connectionSocket);
}

int Connection::sendEvent(Event& event){
    std::size_t remainingBytes = event.getDataSize();
    void* sendingPtr = event.getData();
    while (remainingBytes > 0)
    {
        ssize_t sentBytes = send(connectionSocket, sendingPtr, remainingBytes, 0);
        if(sentBytes <= 0){
            return -1;
        }
        remainingBytes -= sentBytes;
    }
}

Event& Connection::receiveNextEvent(){

    struct event nextEvent;
    recv(connectionSocket, &nextEvent, sizeof(nextEvent), 0);
}

int Connection::setEventHandler(void* handleEvent(Event&)){
    //disable setting a new EventHandler on handling this call
    //create a new thread which continuously listens on this socket
}