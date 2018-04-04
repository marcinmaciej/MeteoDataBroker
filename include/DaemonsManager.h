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

    const unsigned char HTTPD = 0,
                        RS232D = 1,
                        SLEEP_TIME[2] = { 10, 2 };

    const char * PATH = "/etc/meteo-station/",
                 * DAEMONS_NAMES[2] = { "httpd", "rs232d" },
                                      * FILE_EXT = ".pid";


    pid_t * daemonsPids;

    MyDaemon * httpd;
    MyDaemon * rs232d;

    MeteoLog * meteoLog;

    bool isRunning(const pid_t * daemonPid);
    void killDaemon(const pid_t *daemonPid);
    void getPid(int daemon);
    void collectGarbage();
};

#endif // DAEMONSMENAGER_H
