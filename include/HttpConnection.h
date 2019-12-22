/**
 * @author  Created by Marcin Guziołek on 06.04.18.
 */

#ifndef HTTPCONNECTION_H
#define HTTPCONNECTION_H

#define TESTING false

#include <netdb.h>
#include <sys/socket.h>
#include "MyLog.h"
#include "ConfigManager.h"


using namespace std;


class HttpConnection
{

public:
    explicit HttpConnection(const ConfigManager *configManager);

    ~HttpConnection();

    int httpConnect();

    bool sendData(const char *data);

    void httpClose();

private:

    MyLog *meteoLog;

    struct addrinfo *hints;

    const string *hostName,
            *hostIp,
            *port;

    int socketDescriptor = -1,
            socketType,
            protocol,
            flags,
            family;

    string request;

    size_t bytesToSend;

    /* Class private methods */


    /* Parameters of function: getaddrinfo */


    /* Parameter returned by socket function */


    /* Sets hints */
    void setHints();

    /* Internally set parameter */
    /* Require numeric address to avoid potential lengthy lookup calls  */

    void setFlags(int flags = AI_NUMERICHOST);

    int getFlags() const;

    /* Socket parameters */


    addrinfo *getHints() const;

    const char *getHostName() const;

    const char *getHostIp() const;

    const char *getPort() const;

    int getSocketType() const;

    int getProtocol() const;

    int getAddressFamily() const;

    const string &getRequest() const;

    int getSocketDescriptor() const;

    void setAddressFamily(string family);

    void setHostName(const char *hostName);

    void setHostIp(const char *hostIp);

    void setPort(const char *port);

    void setSocketDescriptor(int socketDescriptor);

    void setSocketType(string socketType);

    void setProtocol(string protocol);

    void setRequest(const string &request);

    /* Set http request and data for it */
    const char *mergeRequestAndData(string data);

    void createSocket(int ai_family);

    size_t getBytesToSend();

    void setBytesToSend(size_t bytesToSend);

};

#endif // HTTPCONNECTION_H
