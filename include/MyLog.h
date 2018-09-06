/**
 * @author  Created by Marcin Guziołek on 06.04.18.
 */

#ifndef METEOLOG_H
#define METEOLOG_H


#include <cstring>  /* strlen, strerror strcat strcpy */
#include <cerrno>   /* errno */
#include <unistd.h>  /* getpid, setsid, open, read, write, fork, close, sleep, lockf, ftruncate, mkdir */
#include <algorithm>

#include <syslog.h> /* openlog, syslog, closelog */

using namespace std;

class MyLog
{

public:
    explicit MyLog(string ident = "MeteoStationLog");
    ~MyLog() = default;

    void info(const char * msg);
    void notice(const char * msg);
    void echoDigit(int digit);
    void warn(const char * msg);
    void err(const char * msg);

private:

    string ident;

    int facility,
        levels[8],
        options;



};

#endif // METEOLOG_H
