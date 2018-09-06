/**
 * @author  Created by Marcin Guziołek on 06.04.18.
 */

#include "HttpConfig.h"

HttpConfig::HttpConfig()
{
    this->setHostIp();
    this->setHostName();
    this->setPort();
    this->setAddressFamily();
    this->setProtocol();
    this->setSocketType();
    this->setRequest();
    this->setDataLength();
    this->setSleepTime();
    this->setWaitNetwork();
}

const string &HttpConfig::getHostName() const
{
    return this->hostName;
}

const string &HttpConfig::getAddressFamily() const
{
    return this->addressFamily;
}

const string &HttpConfig::getProtocol() const
{
    return this->protocol;
}

const string &HttpConfig::getRequest() const
{
    return this->request;
}

const string &HttpConfig::getHostIp() const
{
    return this->hostIp;
}

const string &HttpConfig::getSocketType() const
{
    return this->socketType;
}

const string &HttpConfig::getPort() const
{
    return this->port;
}

void HttpConfig::setHostName(const string &hostName)
{
    this->hostName = hostName;
}

void HttpConfig::setSocketType(const string &socketType)
{
    this->socketType = socketType;
}

void HttpConfig::setHostIp(const string &hostIp)
{
    this->hostIp = hostIp;
}

void HttpConfig::setRequest(const string &request)
{
    this->request = request;
}

void HttpConfig::setProtocol(const string &protocol)
{
    this->protocol = protocol;
}

void HttpConfig::setAddressFamily(const string &addressFamily)
{
    this->addressFamily = addressFamily;
}

void HttpConfig::setPort(const string &port)
{
    this->port = port;
}

size_t HttpConfig::getDataLength() const
{
    return this->dataLength;
}

void HttpConfig::setDataLength(const string & dataLength)
{
    this->dataLength = static_cast<size_t >(stoi(dataLength));
}

const unsigned int HttpConfig::getSleepTime() const
{
    return this->sleepTime;
}

void HttpConfig::setSleepTime(const string & sleepTime)
{
    this->sleepTime = static_cast<unsigned int>(stoi(sleepTime));
}

const unsigned int HttpConfig::getWaitNetwork() const
{
    return this->waitNetwork;
}

void HttpConfig::setWaitNetwork(const string & waitNetwork)
{
    this->waitNetwork = static_cast<unsigned int>(stoi(waitNetwork));
}
