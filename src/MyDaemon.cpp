/**
 * @author  Created by Marcin Guziołek on 06.04.18.
 */

#include "MyDaemon.h"


MyDaemon::MyDaemon(Task *task, const string &daemonName, const string &path)
{
    this->task = task;
    this->daemonName = daemonName;

    this->pidFilePath = path;
    this->pidFilePath += "."; /* Make file hidden */
    this->pidFilePath += this->daemonName;
    this->pidFilePath += ".pid";

    this->meteoLog = new MyLog(this->daemonName);

    try
    {
        /* Change the file mode mask */
        umask(0);

        /* Open any logs here */

        /* Create a new SID (session id same as process id PID) for the child process */
        this->setSid(setsid());

        if (this->getSid() < 0)
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

        this->start();

    }
    catch (...)
    {
        this->meteoLog->err(strerror(errno));

        delete this->meteoLog;
    }
}

const int MyDaemon::getPidFileDesc() const
{
    return this->pidFileDescriptor;
}

void MyDaemon::setPidFileDesc(int pidFileDescriptor)
{
    this->pidFileDescriptor = pidFileDescriptor;
}

void MyDaemon::start()
{

    /* Daemon-specific initialization goes here */
    try
    {
        this->exitOnAnotherInstance();

        this->saveNewPid();

        string msg = "Daemon: ";
        msg += this->daemonName;
        msg += " started!";

        /* Say you are here */
        this->meteoLog->notice(msg.c_str());

    }
    catch (...)
    {
        this->meteoLog->err(strerror(errno));

        delete this->meteoLog;

        exit(EXIT_FAILURE);
    }

    /* The Main Loop */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    while (true)
    {

        try
        {

                    /* Do some task here ... */
                    this->task->task(this->daemonName.c_str());

        }
        catch (const char *s)
        {
            this->meteoLog->err(s);
        }
        catch (...)
        {
            this->meteoLog->err(strerror(errno));
        }
    }
#pragma clang diagnostic pop

    exit(EXIT_SUCCESS);

}

const pid_t MyDaemon::getSid() const
{
    return this->sid;
}

void MyDaemon::setSid(pid_t sid)
{
    this->sid = sid;
}

void MyDaemon::exitOnAnotherInstance()
{
    /* Open pid file to write the pid */
    this->setPidFileDesc(open(this->pidFilePath.c_str(), O_CREAT | O_WRONLY, 0600));

    if (this->getPidFileDesc() == -1)
    {
        throw errno;
    }

    /* Try to lock pid file or if locked exit */
    if (lockf(this->getPidFileDesc(), F_TLOCK, 0))
    {
        throw errno;    /* Exit another instance is running */
    }
}

void MyDaemon::saveNewPid()
{
    /* Convert pid to a string */
    const char *pidAsChar = to_string(this->getSid()).c_str();

    /* Remove old PID */
    if (ftruncate(this->getPidFileDesc(), 0) < 0)
    {
        throw errno;
    }

    auto len = static_cast<ssize_t>(strlen(pidAsChar));

    /* Write daemon pid to a file */
    if ((write(this->getPidFileDesc(), pidAsChar, static_cast<size_t>(len))) < len)
    {
        throw errno;
    }
}

MyDaemon::~MyDaemon()
{
    this->meteoLog->err("BYE BYE");

    delete this->meteoLog;
}
