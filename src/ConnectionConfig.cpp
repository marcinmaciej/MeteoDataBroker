//
// Created by Marcin Guziołek on 05.04.18.
//

#include "ConnectionConfig.h"

ConnectionConfig::ConnectionConfig()
{
    this->setHostIp();
    this->setHostName();
    this->setPort();
    this->setAddressFamily();
    this->setProtocol();
    this->setSocketType();
    this->setRequest();
}

const string &ConnectionConfig::getHostName() const
{
    return this->hostName;
}

void ConnectionConfig::setHostName(const string &hostName)
{
    this->hostName = hostName;
}

const string &ConnectionConfig::getAddressFamily() const
{
    return this->addressFamily;
}

const string &ConnectionConfig::getProtocol() const
{
    return this->protocol;
}

const string &ConnectionConfig::getRequest() const
{
    return this->request;
}

const string &ConnectionConfig::getHostIp() const
{
    return this->hostIp;
}

const string &ConnectionConfig::getSocketType() const
{
    return this->socketType;
}

void ConnectionConfig::setSocketType(const string &socketType)
{
    this->socketType = socketType;
}

void ConnectionConfig::setHostIp(const string &hostIp)
{
    this->hostIp = hostIp;
}

void ConnectionConfig::setRequest(const string &request)
{
    this->request = request;
}

void ConnectionConfig::setProtocol(const string &protocol)
{
    this->protocol = protocol;
}

void ConnectionConfig::setAddressFamily(const string &addressFamily)
{
    this->addressFamily = addressFamily;
}

int ConnectionConfig::getPort() const
{
    return this->port;
}

void ConnectionConfig::setPort(int port)
{
    this->port = port;
}
