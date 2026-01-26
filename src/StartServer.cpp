#include "Config.h"
#include "Server.h"
#include <limits.h>
#include "Logger.h"

int main(int argc, char const *argv[])
{
    initLogger();
    unsigned short port = DEFAULT_PORT;
    if(argc > 1){
        //parsing 
        int iPort = atoi(argv[1]);
        if (iPort > USHRT_MAX || iPort <= 0){
            fprintf(stderr, "Invalid Port");
            return -1;
        }else{
            port = iPort;
        }
    }
    Server server = Server(port);
    server.run();
    return 0;
}