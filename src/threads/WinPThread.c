#ifdef _WIN32
#include "Threads/_WinPThread.h"

struct threadData
{
    LPVOID args;
    LPVOID retval;
    void * (*start_routine)(void *);
};


DWORD WINAPI start_routine_wrapper(LPVOID lpParam){
    struct threadData* data = (struct threadData*) lpParam;
    data->retval = data->start_routine(data->args);
    return 0;
}

int pthread_create(pthread_t *thread,
                    const pthread_attr_t * attr,
                    void * start_routine(void *),
                    void * arg){
    if(thread == NULL){
        return 1;
    }
    thread->_tData.args = arg;
    thread->_tData.start_routine = start_routine;
    thread->_tData.retval = NULL;

    thread->_tHandle = CreateThread(NULL, 0, start_routine_wrapper, &thread->_tData, 0, &thread->id);
    return 0;
}

int pthread_join(pthread_t thread, void **retval){
    DWORD waitReturnValue = WaitForSingleObject(thread._tHandle, INFINITE);
    switch (waitReturnValue){
        case WAIT_OBJECT_0:
            break;
        case WAIT_ABANDONED:
            return 0x80;
        case WAIT_TIMEOUT:
            return 0x0102;
        case WAIT_FAILED:
            return 0xFFFF;
    }
    if(retval != NULL){
        *retval = thread._tData.retval;
    }
    return 0;
}

//a wrapper around any os-specific thread model
/*class Thread
{
private:
    #ifdef _WIN32
    DWORD threadId;
    #endif
public:
    Thread(void* threadFunction(void* args), void* args);
    ~Thread();
};

#ifdef _WIN32



Thread::Thread(void* threadFunction(void* args), void* args)
{
    CreateThread(NULL, 0, threadFunction, args, 0, &threadId);
}

Thread::~Thread()
{

}*/
#endif