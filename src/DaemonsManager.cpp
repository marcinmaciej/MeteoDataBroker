#include "DaemonsManager.h"

#include "HttpTask.h"
#include "Rs232Task.h"


DaemonsManager::DaemonsManager()
{

    this->FILE_EXT = new const string(".pid");

    this->pidFilePath = new const string("/etc/meteo-station/");

    this->daemonsNames = new const string[2]{"rs232d", "httpd"};

    this->sleepTime = new unsigned int[2]{2, 5};

    this->daemonsPids = new pid_t[2];

    this->setPid(this->RS232D, 0);

    this->setPid(this->HTTPD, 0);

    this->meteoLog = new MeteoLog("MS-DaemonsManagerClass");

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

            } else
            {

                this->rs232d = new MyDaemon(this->getSleepTime(this->RS232D), new Rs232Task(),
                                            this->getDaemonName(this->RS232D), this->getPidFilePath());

                this->setPid(this->RS232D, this->rs232d->getSid());

            }
        } else
        {

            this->httpd = new MyDaemon(*(this->sleepTime + this->HTTPD), new HttpTask(),
                                       this->getDaemonName(this->HTTPD), this->getPidFilePath());

            this->setPid(this->HTTPD, this->httpd->getSid());

        }

    }
    catch (...)
    {
        this->meteoLog->err(strerror(errno));
    }

}

void DaemonsManager::getSavedPid(int daemon)
{

    string pidFilePath = *this->pidFilePath;
    pidFilePath += "."; /* Make file hidden */
    pidFilePath += *(this->daemonsNames + daemon);
    pidFilePath += *this->FILE_EXT;

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

        } else
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
            } else if (errno == ESRCH)
            {
                this->meteoLog->warn("NO ACTIVE PID!");
            } else if (errno == EINVAL)
            {
                this->meteoLog->warn("INVALID SIGNAL!");
            }

            throw errno;

        } else
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

const string DaemonsManager::getDaemonName(unsigned char daemon) const
{
    return *(this->daemonsNames + daemon);
}

const string DaemonsManager::getPidFilePath() const
{
    return *this->pidFilePath;
}

unsigned int DaemonsManager::getSleepTime(unsigned char daemon) const
{
    return *(this->sleepTime + daemon);
}

pid_t *DaemonsManager::getPid(unsigned char daemon) const
{
    return (this->daemonsPids + daemon);
}

void DaemonsManager::setPid(unsigned char daemon, int pid)
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

void DaemonsManager::setSleepTime(unsigned int *sleepTime)
{
    this->sleepTime = sleepTime;
}

void DaemonsManager::setDaemonsNames(const string *daemonsNames)
{
    this->daemonsNames = daemonsNames;
}
