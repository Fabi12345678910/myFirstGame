#include "NetworkManager.h"
#include <threads.h>

int main(int argc, char const *argv[])
{
    NetworkManager netMan;
    
    try
    {
        ClientConnection clientSocket = netMan.createClient();


    }
    catch(const std::exception& e)
    {
        std::cerr << "exception occured: " << e.what() << '\n';
    }
    
    
    return 0;
}
