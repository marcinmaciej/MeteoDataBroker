/**
 * @author  Created by Marcin Guziołek on 06.04.18.
 */

#ifndef MYDAEMON_H
#define MYDAEMON_H

#include <sys/stat.h> /* umask */
#include <fcntl.h>      /* fcntl, open O_WRONLY, O_RDONLY, O_CREAT  */
#include <unistd.h>  /* setsid(), chdir(), nice(), write(), close(), ftruncate(), STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO */
#include <cerrno>   /* errno */

#include "Task.h"


using namespace std;

class MyDaemon
{
public:

    explicit MyDaemon(Task *task, const string &daemonName, const string &ppidDirPath);

    MyDaemon(const MyDaemon &myDaemon);

    ~MyDaemon();

    pid_t getSid() const;

    void init();


private:

    Task *task;

    pid_t sid = -1;

    string pidFilePath,
            daemonName;

    MyLog *meteoLog;

    int pidFileDescriptor = -1;

    void setEnvironment();

    int getPidFileDesc() const;

    void setPidFileDesc(int ppidFileDescriptor);

    void setSid(pid_t psid);

    void saveNewPid();

    void exitOnAnotherInstance();

    void start();

    void mainLoop();

    void sayHello();
};

#endif // MYDAEMON_H
