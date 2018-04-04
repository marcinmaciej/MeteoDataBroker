#include "MyDaemon.h"


MyDaemon::MyDaemon(unsigned short sleepTime, Task * task, const char * daemonName, string path)
{
    this->task = task;
    this->sleepTime = sleepTime;
    this->DAEMON_NAME = daemonName;

    this->pidFileName = "."; /* Make file hidden */
    this->pidFileName += this->DAEMON_NAME;
    this->pidFileName += ".pid";


    this->meteoLog = new MeteoLog(this->DAEMON_NAME);

    this->pidFilePath = path + this->pidFileName;


    try
    {
        /* Change the file mode mask */
        umask(0);

        /* Open any logs here */

        /* Create a new SID (session id same as process id PID) for the child process */
        this->sid = setsid();

        if (this->sid < 0)
        {
            /* Log the failure */
            throw errno;
        }


        /* Change the current working directory */
        if ((chdir("/")) < 0)
        {
            /* Log the failure */
            throw errno;
        }

        /* Close out the standard file descriptors */
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);

    }
    catch(...)
    {
        this->meteoLog->err(strerror(errno));

        delete this->meteoLog;
    }
}

void MyDaemon::start()
{
    /* Convert pid to a string */
    const char * pidAsChar = to_string(this->sid).c_str();


    /* Daemon-specific initialization goes here */
    try
    {

        /* Open pid file to write the pid */
        if((this->pidFileDescriptor = open(this->pidFilePath.c_str(), O_CREAT | O_WRONLY, 0600)) == -1)
        {
            throw errno;
        }


        /* Try to lock pid file or if locked exit */
        if(lockf(this->pidFileDescriptor, F_TLOCK, 0))
        {
                throw errno;    /* Exit another instance is running */
        }

        /* Remove old PID */
        if(ftruncate(this->pidFileDescriptor, 0) < 0)
        {
            throw errno;
        }


        auto len = static_cast<ssize_t>(strlen(pidAsChar));

        /* Write daemon pid to a file */
        if((write(this->pidFileDescriptor, pidAsChar, static_cast<size_t>(len))) < len)
        {
            throw errno;
        }

        string msg ="Daemon: ";
        msg += this->DAEMON_NAME;
        msg += " started!";

        /* Say you are here */
        this->meteoLog->notice(msg.c_str());

    }
    catch(...)
    {
        this->meteoLog->err(strerror(errno));

        delete this->meteoLog;

        exit(EXIT_FAILURE);
    }

    /* The Main Loop */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    while (1)
    {

        try
        {
            /* Do some task here ... */
            this->task->task(this->DAEMON_NAME);

            /* And sleep, wait N seconds */
            sleep(this->getSleepTime());

        }
        catch(const char * s)
        {
            this->meteoLog->err(s);
        }
        catch(...)
        {
            this->meteoLog->err(strerror(errno));
        }
    }
#pragma clang diagnostic pop

    exit(EXIT_SUCCESS);
}

unsigned short MyDaemon::getSleepTime()
{
    return this->sleepTime;
}

MyDaemon::~MyDaemon()
{
    this->meteoLog->err("BYE BYE");

    delete this->meteoLog;
}
