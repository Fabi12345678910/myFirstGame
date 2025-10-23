#include "networking/ClientConnection.hpp"
#include "networking/EventDefinitions/EventDebugMessage.hpp"

int main(int argc, char const *argv[])
{
    ClientConnection conn({127, 0, 0, 1});
    conn.sendEvent(EventDebugMessage("hello from connectionTestClient"));
    return 0;
}
