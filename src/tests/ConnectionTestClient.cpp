#include "Networking/ClientConnection.h"
#include "Networking/EventDefinitions/EventDebugMessage.h"

int main(int argc, char const *argv[])
{
    ClientConnection conn({127, 0, 0, 1});
    conn.sendEvent(EventDebugMessage("hello from connectionTestClient"));
    sleep(1);
    return 0;
}
