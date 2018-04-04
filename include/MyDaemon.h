#ifndef MYDAEMON_H
#define MYDAEMON_H

#include <sys/types.h>  /* umask */
#include <sys/stat.h>   /* umask */
#include <fcntl.h>      /* fcntl, O_WRONLY, O_RDONLY, O_CREAT */

#include "Task.h"


using namespace std;

class MyDaemon
{
public:

    MyDaemon(unsigned short sleepTime, Task *task,const char * daemonName, string path);
    ~MyDaemon();

    void start();

private:

    unsigned short getSleepTime();
    unsigned short sleepTime;

    const char * DAEMON_NAME;

    Task * task;

    pid_t sid;

    string pidFileName,
           pidFilePath;

    MeteoLog * meteoLog;

    int pidFileDescriptor;
};

#endif // MYDAEMON_H
