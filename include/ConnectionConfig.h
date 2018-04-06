//
// Created by Marcin Guziołek on 05.04.18.
//

#ifndef METEOSTATIONDRIVERS_CONNECTIONCONFIG_H
#define METEOSTATIONDRIVERS_CONNECTIONCONFIG_H

#include <iostream>

using namespace std;

class ConnectionConfig
{

public:
    ConnectionConfig();

    virtual ~ConnectionConfig() = default;

    int getPort() const;

    const string &getHostName() const;

    const string &getSocketType() const;

    const string &getHostIp() const;

    const string &getRequest() const;

    const string &getProtocol() const;

    const string &getAddressFamily() const;


    void setHostName(const string &hostName = "localhost");

    void setAddressFamily(const string &addressFamily = "both");

    void setProtocol(const string &protocol = "none");

    void setRequest(const string &request = "GET / HTTP/1.1\r\nHOST: localhost\r\n\r\n");

    void setHostIp(const string &hostIp = "127.0.0.1");

    void setSocketType(const string &socketType = "TCP");

    void setPort(int port = 80);

private:
    string hostName,
            hostIp,
            addressFamily,
            socketType,
            protocol,
            request;

    int port;
};


#endif //METEOSTATIONDRIVERS_CONNECTIONCONFIG_H
