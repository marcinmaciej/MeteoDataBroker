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

    const string &getPort() const;

    const string &getHostName() const;

    const string &getSocketType() const;

    const string &getHostIp() const;

    const string &getRequest() const;

    const string &getProtocol() const;

    const string &getAddressFamily() const;


    void setHostIp(const string &hostIp = "127.0.0.1");

    void setHostName(const string &hostName = "localhost");

    void setPort(const string &port = "80");

    void setSocketType(const string &socketType = "tcp");

    void setAddressFamily(const string &addressFamily = "both");

    void setProtocol(const string &protocol = "none");

    void setRequest(const string &request = "POST /index.html HTTP/1.1\r\nHost:localhost\r\nContent-Type:application/x-www-form-urlencoded;charset=utf-8\r\nContent-Length:#####\r\nConnection:close\r\n\r\ndata=");


private:
    string hostName,
            hostIp,
            addressFamily,
            socketType,
            protocol,
            request,
            port;

};


#endif //METEOSTATIONDRIVERS_CONNECTIONCONFIG_H
