/**
 * @author  Created by Marcin Guziołek on 06.04.18.
 */

#include "DaemonsManager.h"


DaemonsManager::DaemonsManager(ConfigManager *configManager)
{

    this->setConfigManager(configManager);

    this->setPidFileDirPath(&this->getConfigManager()->programConfig->getPidFileDirPath());

    this->setDaemonsNames(this->getConfigManager()->programConfig->getDaemonsNames());

    this->daemonsPids = new pid_t[2];

    this->setPid(this->RS232D, 0);

    this->setPid(this->HTTPD, 0);

    this->meteoLog = new MyLog("MS-DaemonsManagerClass");

}

void DaemonsManager::killAll()
{

    this->getSavedPid(this->HTTPD);

    this->getSavedPid(this->RS232D);

    if (this->isRunning(this->getPid(this->HTTPD)))
    {
        this->killDaemon(this->getPid(this->HTTPD));
    }

    if (this->isRunning(this->getPid(this->RS232D)))
    {
        this->killDaemon(this->getPid(this->RS232D));
    }

}

void DaemonsManager::restartDaemons()
{

    this->killAll();

    this->startDaemons();

}

void DaemonsManager::startDaemons()
{
    try
    {
        if (fork())
        {
            if (fork())
            {

            }
            else
            {

                this->rs232d = new MyDaemon(new SerialTask(this->getConfigManager()), this->getDaemonName(this->RS232D),
                                            this->getPidFileDirPath());

                this->setPid(this->RS232D, this->rs232d->getSid());

            }
        }
        else
        {

            this->httpd = new MyDaemon(new HttpTask(this->getConfigManager()),
                                       this->getDaemonName(this->HTTPD), this->getPidFileDirPath());

            this->setPid(this->HTTPD, this->httpd->getSid());

        }

    }
    catch (...)
    {
        this->meteoLog->err(strerror(errno));
    }
}

void DaemonsManager::getSavedPid(unsigned char daemon)
{

    string pidFilePath = this->getPidFileDirPath();
    pidFilePath += "."; /* Make file hidden */
    pidFilePath += this->getDaemonName(daemon);
    pidFilePath += ".pid";

    int fd;

    try
    {
        /* Open pid file to read */
        if ((fd = open(pidFilePath.c_str(), O_RDONLY)) == -1)
        {
            throw errno;
        }

        const size_t LEN = 6;

        char buf[LEN];

        if (read(fd, buf, LEN))
        {
            *(this->daemonsPids + daemon) = atoi(buf);

            close(fd);

        }
        else
        {
            throw errno;
        }

    }
    catch (...)
    {
        this->meteoLog->err(strerror(errno));
    }
}

void DaemonsManager::killDaemon(const pid_t *daemonPid)
{
    try
    {

        if (kill(*daemonPid, SIGKILL) < 0)
        {

            if (errno == EPERM)
            {
                this->meteoLog->warn("NO PERMISSION TO KILL!");
            }
            else if (errno == ESRCH)
            {
                this->meteoLog->warn("NO ACTIVE PID!");
            }
            else if (errno == EINVAL)
            {
                this->meteoLog->warn("INVALID SIGNAL!");
            }

            throw errno;

        }
        else
        {
            string message = "Daemon killed! PID:";
            message += to_string(*daemonPid);

            this->meteoLog->notice(message.c_str());
        }

    }
    catch (...)
    {
        this->meteoLog->err(strerror(errno));
    }
}

bool DaemonsManager::isRunning(const pid_t *daemonPid)
{
    try
    {
        if (kill(*daemonPid, 0) == 0) /* Check if daemon with this pid is running */
        {
            return true;
        }
    }
    catch (...)
    {
        this->meteoLog->err(strerror(errno));
    }

    return false;
}

ConfigManager *DaemonsManager::getConfigManager() const
{
    return this->configManager;
}

void DaemonsManager::setConfigManager(ConfigManager *configManager)
{
    this->configManager = configManager;
}

const string DaemonsManager::getPidFileDirPath() const
{
    return *this->pidFilePath;
}

void DaemonsManager::setPidFileDirPath(const string *pidFilePath)
{
    this->pidFilePath = pidFilePath;
}


const string DaemonsManager::getDaemonName(unsigned char daemon) const
{
    return *(this->daemonsNames + daemon);
}

void DaemonsManager::setDaemonsNames(const string *daemonsNames)
{
    this->daemonsNames = daemonsNames;
}

pid_t *DaemonsManager::getPid(unsigned char daemon) const
{
    return (this->daemonsPids + daemon);
}

void DaemonsManager::setPid(unsigned char daemon, pid_t pid)
{
    *(this->daemonsPids + daemon) = pid;
}

void DaemonsManager::collectGarbage()
{

    delete[] this->daemonsPids;
    delete this->meteoLog;
    delete this->httpd;
    delete this->rs232d;
}

DaemonsManager::~DaemonsManager()
{
    this->meteoLog->err("BYE BYE");

    this->collectGarbage();
}
