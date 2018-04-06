#ifndef DAEMONSMANAGER_H
#define DAEMONSMANAGER_H

#include <csignal>

#include "MyDaemon.h"

class DaemonsManager
{
public:

    DaemonsManager();

    ~DaemonsManager();

    void startDaemons();

    void restartDaemons();

    void killAll();

private:

    const unsigned char RS232D = 0,
            HTTPD = 1;

    unsigned int *sleepTime;

    const string *pidFilePath,
            *daemonsNames,
            *FILE_EXT;

    pid_t *daemonsPids;

    MyDaemon *httpd,
            *rs232d;

    MeteoLog *meteoLog;

    const string getPidFilePath() const;

    unsigned int getSleepTime(unsigned char daemon) const;

    const string getDaemonName(unsigned char daemon) const;

    pid_t *getPid(unsigned char daemon) const;

    void setDaemonsNames(const string *daemonsNames);

    void setSleepTime(unsigned int *sleepTime);

    void setPid(unsigned char daemon, pid_t pid);

    bool isRunning(const pid_t *daemonPid);

    void killDaemon(const pid_t *daemonPid);

    void getSavedPid(int daemon);

    void collectGarbage();
};

#endif // DAEMONSMENAGER_H
