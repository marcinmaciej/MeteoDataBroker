/**
 * @author  Created by Marcin Guziołek on 06.12.21.
 */

#include "MyLog.h"

MyLog::MyLog(string pident) : ident(move(pident)) {


    /* Poziomy ważności */
    this->levels[0] = LOG_EMERG;     /* system is unusable */

    this->levels[1] = LOG_ALERT;       /* action must be taken immediately */

    this->levels[2] = LOG_CRIT;       /* critical conditions */

    this->levels[3] = LOG_ERR;      /*  error conditions */

    this->levels[4] = LOG_WARNING;    /* warning conditions */

    this->levels[5] = LOG_NOTICE;     /* normal, but significant, condition */

    this->levels[6] = LOG_INFO;       /* informational message */

    this->levels[7] = LOG_DEBUG;      /* debug-levels message */

    /* Opcje wyświetlania komunikatów */
    this->options = LOG_PID | LOG_NDELAY; /* Dodaje id procesu, bez opóźnienia */


    this->facility = LOG_USER | LOG_LOCAL0; /* Dodaje id użytkownika, */

}

void MyLog::info(const char *msgTitle, const ssize_t *number) {

    openlog(this->ident.c_str(), this->options, this->facility);

    this->msg = msgTitle;
    this->msg += (number != nullptr) ? to_string(*number) : "";

    syslog(this->levels[6], "%s", this->msg.c_str());

    closelog();

}

void MyLog::notice(const char *msgTitle, const ssize_t *number) {

    openlog(this->ident.c_str(), this->options, this->facility);

    this->msg = msgTitle;
    this->msg += (number != nullptr) ? std::to_string(*number) : "";

    syslog(this->levels[5], "%s", this->msg.c_str());

    closelog();

}

void MyLog::warn(const char *msgTitle, int error_number) {

    openlog(this->ident.c_str(), this->options, this->facility);

    this->msg = msgTitle;
    this->msg += (error_number != 0) ? strerror(error_number) : "";

    syslog(this->levels[4], "%s", this->msg.c_str());

    closelog();

}

void MyLog::err(const char *msgTitle, int error_number) {

    openlog(this->ident.c_str(), this->options, this->facility);

    this->msg = msgTitle;
    this->msg += (error_number != 0) ? strerror(error_number) : "";

    syslog(this->levels[3], " %s", this->msg.c_str());

    closelog();

}
