#include <cstdlib>

struct event{
    int type;
    size_t dataSize;
    char dataBuffer[128];
};

class Event
{
public:
    //returns a pointer to the data of the ClientEvent, which must not be freed
    virtual void* getData() = 0;
    virtual size_t getDataSize() = 0;
    virtual int getType() = 0;
};
