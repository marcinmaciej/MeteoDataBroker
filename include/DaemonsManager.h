/**
 * @author  Created by Marcin Guziołek on 06.04.18.
 */

#ifndef DAEMONSMANAGER_H
#define DAEMONSMANAGER_H

#include <csignal> /* int kill(__pid_t __pid, int __sig), SIGTERM, */
#include <sys/prctl.h> /* prctl(int option,...) kontrola wywołania procesu, PR_SET_NAME - opcja ustawia nazwę procesu  */
#include <string> /* std::string */
#include <unistd.h>  /*  read(), fork(), close(), pipe()  */
#include <fcntl.h> /* open(), O_RDONLY */
#include <cerrno>   /* errno */

#include "ConfigManager.h"
#include "MyDaemon.h"
#include "HttpTask.h"
#include "SerialTask.h"

class DaemonsManager {

public:

    explicit DaemonsManager(const ConfigManager &configManager);

    // Copying constructor
    DaemonsManager(DaemonsManager &daemonsManager);

    ~DaemonsManager();

    void init();

    void restartDaemons();

    void killAll();

    const ConfigManager &getConfigManager() const;


private:

    MyLog *meteoLog;

    const ConfigManager &configManager;

    /*
     * Wartości stałych odpowiadają wartościom indeksów w tablicy uchwytów potoku
     * łączącego oba demony. Demon "rs232d" zapisuje do potoku, 1 jest zapisem,
     * a demon "httpd" odczytuje z potoku, 0 jest odczytem
     * */
    enum {
        HTTPD, RS232D
    };

    /* Odpowiednio zapisane nazwy demonów */
    const std::string daemonsNames[2] = {"httpd", "rs232d"};

    const std::string *pidFileDirPath;

    pid_t *daemonsPids;

    const std::string *getPidFileDirPath() const;

    pid_t *getPid(unsigned char daemonPidIndex) const;

    void setPid(unsigned char daemonPidIndex, pid_t pid);

    bool isRunning(const pid_t *daemonPid);

    void killDaemon(const pid_t *daemonPid);

    void getSavedPid(unsigned char daemonNameIndex);

};

#endif // DAEMONSMENAGER_H
