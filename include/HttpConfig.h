/**
 * @author  Created by Marcin Guziołek on 06.04.18.
 */

#ifndef METEOSTATIONDRIVERS_CONNECTIONCONFIG_H
#define METEOSTATIONDRIVERS_CONNECTIONCONFIG_H

#include <iostream>

using namespace std;

class HttpConfig
{

public:
    HttpConfig();

    virtual ~HttpConfig() = default;

    const string &getPort() const;

    const string &getHostName() const;

    const string &getSocketType() const;

    const string &getHostIp() const;

    const string &getRequest() const;

    const string &getProtocol() const;

    const string &getAddressFamily() const;


    size_t getDataLength() const;

    const unsigned int getSleepTime() const;

    const unsigned int getWaitNetwork() const;

    void setHostIp(const string &hostIp = "127.0.0.1");

    void setHostName(const string &hostName = "localhost");

    void setPort(const string &port = "80");

    void setSocketType(const string &socketType = "tcp");

    void setAddressFamily(const string &addressFamily = "both");

    void setProtocol(const string &protocol = "none");

    void setRequest(
            const string &request = "POST /index.html HTTP/1.1\r\nHost:localhost\r\nContent-Type:application/x-www-form-urlencoded;charset=utf-8\r\nContent-Length:#####\r\nConnection:close\r\n\r\ndata=");

    void setDataLength(const string & dataLength = "1024");

    void setSleepTime(const string & sleepTime = "5");

    void setWaitNetwork(const string & waitNetwork = "30");

private:
    string hostName,
            hostIp,
            addressFamily,
            socketType,
            protocol,
            request,
            port;

    size_t dataLength;

    unsigned int sleepTime,
            waitNetwork;

};


#endif //METEOSTATIONDRIVERS_CONNECTIONCONFIG_H
