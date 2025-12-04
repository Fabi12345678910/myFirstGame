#pragma once
#ifdef _WIN32
#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <processthreadsapi.h>

struct _threadData
{
    LPVOID args;
    LPVOID retval;
    void * (*start_routine)(void *);
};

typedef struct processInfo{
    DWORD id;
    HANDLE _tHandle;
    struct _threadData _tData;
} pthread_t;

typedef struct pthread_attr{
    int dummy;
}pthread_attr_t;

#ifdef __cplusplus
extern "C" {
#endif
int pthread_create(pthread_t *thread,
                    const pthread_attr_t * attr,
                    void * start_routine(void *),
                    void * arg);

int pthread_join(pthread_t thread, void **retval);

#ifdef __cplusplus
}
#endif

#endif