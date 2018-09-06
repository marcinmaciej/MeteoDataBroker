/**
 * @author  Created by Marcin Guziołek on 06.04.18.
 */

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

    explicit MyDaemon(Task *task, const string &daemonName, const string &path);

    ~MyDaemon();

    const pid_t getSid() const;


private:

    Task *task;

    pid_t sid;

    string pidFilePath,
            daemonName;

    MyLog *meteoLog;

    int pidFileDescriptor;

    const int getPidFileDesc() const;

    void setPidFileDesc(int pidFileDescriptor);

    void setSid(pid_t sid);

    void saveNewPid();

    void exitOnAnotherInstance();

    void start();
};

#endif // MYDAEMON_H
