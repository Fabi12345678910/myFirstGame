#include "NetworkManager.h"
#include <threads.h>
#include "Events.h"

int main(int argc, char const *argv[])
{
    NetworkManager netMan;
    
    try
    {
        ClientConnection conn = netMan.createClient();
        EventDebugMessage ev("hello");
        conn.sendEvent(ev);
    }
    catch(const std::exception& e)
    {
        std::cerr << "exception occured: " << e.what() << '\n';
    }
    
    
    return 0;
}
