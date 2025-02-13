/**
 * @author  Created by Marcin Guziołek on 06.12.21.
 */

#ifndef METEOLOG_H
#define METEOLOG_H


#include <cstring>  /* strerror() */
#include <string>/* std::string */
#include <cerrno>   /* errno */
#include <syslog.h> /* openlog, syslog, closelog */

using namespace std;

class MyLog {

public:
    explicit MyLog(std::string pident = "MeteoStationLog");

    ~MyLog() = default;

    void info(const char *msgTitle, const ssize_t *number = nullptr);

    void notice(const char *msgTitle,const ssize_t *number = nullptr);

    void warn(const char *msgTitle, int error_number = 0);

    void err(const char *msgTitle, int error_number = 0);

private:

    std::string ident, /* Identyfikator procesu */
    msg;

    int facility,
            levels[8] = {},
            options;


};

#endif // METEOLOG_H
