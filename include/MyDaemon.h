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

    MyDaemon(unsigned int sleepTime, Task *task, const string &daemonName, const string &path);

    ~MyDaemon();

    pid_t getSid() const;


private:

    unsigned int sleepTime;

    Task *task;

    pid_t sid;

    string pidFilePath,
            daemonName;

    MeteoLog *meteoLog;

    int pidFileDescriptor;

    int getPidFileDesc() const;

    unsigned int getSleepTime() const;

    void setPidFileDesc(int pidFileDescriptor);

    void setSid(pid_t sid);

    void saveNewPid();

    void exitOnAnotherInstance();

    void start();
};

#endif // MYDAEMON_H
