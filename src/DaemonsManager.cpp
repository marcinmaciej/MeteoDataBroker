#include "DaemonsManager.h"

#include "HttpTask.h"
#include "Rs232Task.h"


DaemonsManager::DaemonsManager()
{

    this->daemonsPids = new pid_t[2];
    this->daemonsPids[0] = 0;
    this->daemonsPids[1] = 0;

    this->meteoLog = new MeteoLog("MS-DaemonsManagerClass");

}

void DaemonsManager::killAll()
{

    this->getPid(this->HTTPD);

    this->getPid(this->RS232D);

    if(isRunning((this->daemonsPids+this->HTTPD)))
    {
        this->killDaemon((this->daemonsPids + this->HTTPD));
    }

    if(isRunning((this->daemonsPids+this->RS232D)))
    {
        this->killDaemon((this->daemonsPids + this->RS232D));
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
        if(fork())
        {
            if(fork())
            {
            }
            else
            {

                this->rs232d = new MyDaemon(*(this->SLEEP_TIME+this->RS232D), new Rs232Task(), *(this->DAEMONS_NAMES+this->RS232D), this->PATH);
                this->rs232d->start();

            }
        }
        else
        {

            this->httpd = new MyDaemon(*(this->SLEEP_TIME+this->HTTPD), new HttpTask(), *(this->DAEMONS_NAMES+this->HTTPD), this->PATH);
            this->httpd->start();

        }

    }
    catch(...)
    {
        this->meteoLog->err(strerror(errno));
    }

}

void DaemonsManager::getPid(const int daemon)
{

    string pidFilePath = this->PATH;
    pidFilePath += "."; /* Make file hidden */
    pidFilePath += *(this->DAEMONS_NAMES+daemon);
    pidFilePath += this->FILE_EXT;

    int fd;
    const size_t LEN = 6;

    try
    {
        /* Open pid file to read */
        if((fd = open(pidFilePath.c_str(), O_RDONLY)) == -1)
        {
            throw errno;
        }

        char buf[LEN];

        if(read(fd, buf, LEN))
        {
            *(this->daemonsPids+daemon) = atoi(buf);

            close(fd);
        }
        else
        {
            throw errno;
        }

    }
    catch(...)
    {
        this->meteoLog->err(strerror(errno));
    }
}

void DaemonsManager::killDaemon(const pid_t *daemonPid)
{
    try
    {

        if(kill(*daemonPid, SIGKILL) < 0)
        {

            if(errno == EPERM)
            {
                this->meteoLog->warn("NO PERMISSION TO KILL!");
            }
            else if(errno == ESRCH)
            {
                this->meteoLog->warn("NO ACTIVE PID!");
            }
            else if(errno == EINVAL)
            {
                this->meteoLog->warn("INVALID SIGNAL!");
            }

            throw errno;

        }
        else
        {
            string message ="Daemon killed! PID:";
            message+= to_string(*daemonPid);

            this->meteoLog->notice(message.c_str());
        }

    }
    catch(...)
    {
        this->meteoLog->err(strerror(errno));
    }
}

bool DaemonsManager::isRunning(const pid_t * daemonPid)
{
    try
    {
        if(kill(*daemonPid, 0) == 0) /* Check if daemon with this pid is running */
        {
            return true;
        }
    }
    catch(...)
    {
        this->meteoLog->err(strerror(errno));
    }

    return false;
}

void DaemonsManager::collectGarbage()
{

    delete [] this->daemonsPids;
    delete this->meteoLog;
    delete this->httpd;
    delete this->rs232d;

}

DaemonsManager::~DaemonsManager()
{
    this->meteoLog->err("BYE BYE");

    this->collectGarbage();
}
