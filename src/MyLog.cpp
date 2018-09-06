/**
 * @author  Created by Marcin Guziołek on 06.04.18.
 */

#include "MyLog.h"

MyLog::MyLog(string ident)
{

    this->ident = ident;

    this->levels[0] =  LOG_EMERG;     /* system is unusable */

    this->levels[1] =  LOG_ALERT;       /* action must be taken immediately */

    this->levels[2] =  LOG_CRIT;       /* critical conditions */

    this->levels[3] =  LOG_ERR;      /*  error conditions */

    this->levels[4] =  LOG_WARNING;    /* warning conditions */

    this->levels[5] =  LOG_NOTICE;     /* normal, but significant, condition */

    this->levels[6] =  LOG_INFO;       /* informational message */

    this->levels[7] =  LOG_DEBUG;      /* debug-levels message */

    this->options = LOG_PID | LOG_NDELAY;

    this->facility = LOG_USER | LOG_LOCAL0;

}

void MyLog::info(const char * msg)
{

    openlog(this->ident.c_str(),this->options,this->facility);
    syslog(this->levels[6],"User id: %d, %s",getpid(),msg);
    closelog();

}

void MyLog::notice(const char * msg)
{

    openlog(this->ident.c_str(),this->options,this->facility);
    syslog(this->levels[5],"User id: %d, %s",getpid(),msg);
    closelog();

}

void MyLog::echoDigit(int digit)
{

    openlog(this->ident.c_str(),this->options,this->facility);
    syslog(this->levels[5],"User id: %d, %d",getpid(),digit);
    closelog();

}

void MyLog::warn(const char * msg)
{

    openlog(this->ident.c_str(),this->options,this->facility);
    syslog(this->levels[4],"User id: %d, %s",getpid(),msg);
    closelog();

}

void MyLog::err(const char * msg)
{

    openlog(this->ident.c_str(),this->options,this->facility);
    syslog(this->levels[3],"User id: %d, %s",getpid(),msg);
    closelog();

}
