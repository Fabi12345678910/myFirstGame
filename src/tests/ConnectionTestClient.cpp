#include "Networking/ClientConnection.h"
#include "Networking/EventDefinitions/EventDebugMessage.h"

int main(int argc, char const *argv[])
{
    ClientConnection conn = ClientConnection::createClientConnection({127, 0, 0, 1}, 42069);
    conn.sendTcpEvent(EventDebugMessage("hello from connectionTestClient"));
    sleep(1);
    return 0;
}
