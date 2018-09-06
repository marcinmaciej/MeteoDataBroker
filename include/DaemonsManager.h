/**
 * @author  Created by Marcin Guziołek on 06.04.18.
 */

#ifndef DAEMONSMANAGER_H
#define DAEMONSMANAGER_H

#include <csignal>

#include "ConfigManager.h"
#include "MyDaemon.h"
#include "HttpTask.h"
#include "SerialTask.h"

class DaemonsManager
{
public:

    explicit DaemonsManager(ConfigManager * configManager);

    ~DaemonsManager();

    void startDaemons();

    void restartDaemons();

    void killAll();

    void setPidFileDirPath(const string *pidFilePath);

    ConfigManager *getConfigManager() const;

    void setConfigManager(ConfigManager *configManager);

private:

    const unsigned char RS232D = 0,
            HTTPD = 1;

    const string *pidFilePath,
            *daemonsNames;

    pid_t *daemonsPids;

    ConfigManager * configManager;

    const MyDaemon *httpd,
            *rs232d;

    MyLog *meteoLog;

    const string getPidFileDirPath() const;

    const string getDaemonName(unsigned char daemon) const;

    pid_t *getPid(unsigned char daemon) const;

    void setDaemonsNames(const string *daemonsNames);

    void setPid(unsigned char daemon, pid_t pid);

    bool isRunning(const pid_t *daemonPid);

    void killDaemon(const pid_t *daemonPid);

    void getSavedPid(unsigned char daemon);

    void collectGarbage();
};

#endif // DAEMONSMENAGER_H
