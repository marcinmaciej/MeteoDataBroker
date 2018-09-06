#ifndef METEOLOG_H
#define METEOLOG_H


#include <cstring>  /* strlen, strerror strcat strcpy */
#include <cerrno>   /* errno */
#include <unistd.h>  /* getpid, setsid, open, read, write, fork, close, sleep, lockf, ftruncate, mkdir */
#include <algorithm>

#include <syslog.h> /* openlog, syslog, closelog */

using namespace std;

class MeteoLog
{

public:
    explicit MeteoLog(string ident = "MeteoStationLog");
    ~MeteoLog() = default;

    void info(const char * msg);
    void notice(const char * msg);
    void echoDigit(int digit);
    void warn(const char * msg);
    void err(const char * msg);

private:

    const char * ident;

    int facility,
        levels[8],
        options;



};

#endif // METEOLOG_H
