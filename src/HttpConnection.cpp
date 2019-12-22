/**
 * @author  Created by Marcin Guziołek on 06.04.18.
 */

#include "HttpConnection.h"

#define RASPBERRYPI true

HttpConnection::HttpConnection(const ConfigManager *configManager)
{

    /* Set default flag AI_NUMERICHOST */
    this->setFlags();

    this->meteoLog = new MyLog("MS-HttpdConnectionClass");

    this->setHostIp(configManager->httpConfig->getHostIp().c_str());

    this->setHostName(configManager->httpConfig->getHostName().c_str());

    this->setPort(configManager->httpConfig->getPort().c_str());

    this->setSocketType(configManager->httpConfig->getSocketType());

    this->setAddressFamily(configManager->httpConfig->getAddressFamily());

    this->setProtocol(configManager->httpConfig->getProtocol());

    this->setRequest(configManager->httpConfig->getRequest());

    this->setHints();


}

int HttpConnection::httpConnect()
{

    struct addrinfo *rp,
            *result;

    result = new addrinfo;

    try
    {

        string *node = new string((this->getHints()->ai_flags == AI_NUMERICHOST) ? this->getHostIp() : this->getHostName());

        if (getaddrinfo(node->c_str(), this->getPort(), this->getHints(), &result) > 0)
        {
            throw "no network";
        }


        for (rp = result; rp != nullptr; rp = rp->ai_next)
        {
            this->createSocket(rp->ai_family);

            if (this->getSocketDescriptor() == -1)
            {
                continue; /* Socket not created yet */
            }

            if (connect(this->getSocketDescriptor(), rp->ai_addr, rp->ai_addrlen) != -1)
            {
                break; /* Success */
            }
        }

        freeaddrinfo(result);

        if (rp == nullptr)
        {
            return 0;
        }

#if TESTING
        this->meteoLog->notice("Http connected!");
#endif // TESTING

        return 1;

    }
    catch (...)
    {

        this->meteoLog->err(strerror(errno));

        throw errno;
    }
}

bool HttpConnection::sendData(const char *data)
{

    string *request = new string(this->mergeRequestAndData(data));
    ssize_t bytesSent = 0;
    size_t bytesToSend = this->getBytesToSend();
    char *buf = new char[1025]();

    if (this->getSocketDescriptor() >= 0)
    {
        try
        {
            /* send data to the server */
            bytesSent = write(this->getSocketDescriptor(), request->c_str(), bytesToSend);

            if (bytesSent == bytesToSend)
            {

#if RASPBERRYPI
                string temppath = "/home/pi/serverresponse.txt";
#else
                string temppath = "/home/marcin/serverresponse.txt";
#endif
                int fd = open(temppath.c_str(), O_CREAT | O_WRONLY, 0777);
                write(fd, buf,read(this->getSocketDescriptor(), buf, 1024));
                close(fd);
#if RASPBERRYPI
                temppath = "/home/pi/serverresponse.txt";
#else
                temppath = "/home/marcin/mylog.txt";
#endif
                fd = open(temppath.c_str(), O_CREAT | O_WRONLY, 0777);
                write(fd, request->c_str(), strlen(request->c_str()));
                close(fd);

                return true;
            }
        }
        catch (...)
        {
            this->meteoLog->err(strerror(errno));
        }

    }

    return false;
}

/* PRIVATE METHODS */


void HttpConnection::createSocket(int ai_family)
{
    try
    {

        this->setSocketDescriptor(socket(ai_family, this->getSocketType(), this->getProtocol()));

        if (this->getSocketDescriptor() == -1)
        {
            throw errno;
        }

    }
    catch (...)
    {
        this->meteoLog->err(strerror(errno));
    }
}

const char *HttpConnection::mergeRequestAndData(string data)
{
    /* This delimiter is in place of the data length number in content-length header of the http request */
    const char *DELIMITER = "#####";
    size_t len = data.length();

    /* Get request string loaded from the config file */
    string *request = new string(this->getRequest());

    /* Count and add length of the variable with equal sign which is at the end of the request */
    len += (request->length() - request->rfind("\n") - 1); /* Minus 1 because of two characters in rfind() */

    /* Replace delimiter with data length in content-length header we don't know the data length before */
    request->replace(request->find(DELIMITER), strlen(DELIMITER), to_string(len));

    *request += data;

    this->setBytesToSend(request->length());

    return request->c_str();
}


void HttpConnection::setProtocol(string protocol)
{
    transform(protocol.begin(), protocol.end(), protocol.begin(), ::tolower); /* Change all letters to lower */
    this->meteoLog->err(protocol.c_str());
    if (protocol == "none")
    {
        this->protocol = 0;
    }
    else
    {
        this->protocol = atoi(protocol.c_str());
    }
}

void HttpConnection::setAddressFamily(string family)
{

    transform(family.begin(), family.end(), family.begin(), ::tolower); /* Change all letters to lower */
    this->meteoLog->err(family.c_str());
    if (family == "ip4")
    {
        this->family = AF_INET;
    }
    else if (family == "ip6")
    {
        this->family = AF_INET6;
    }
    else if (family == "both")
    {
        this->family = AF_UNSPEC;
    }
}

void HttpConnection::setSocketType(string type)
{
    transform(type.begin(), type.end(), type.begin(), ::tolower); /* Change all letters to lower */
    this->meteoLog->err(type.c_str());
    if (type == "tcp")
    {
        this->socketType = SOCK_STREAM;
    }
    else if (type == "udp")
    {
        this->socketType = SOCK_DGRAM;
    }
    else if (type == "raw")
    {
        this->socketType = SOCK_RAW; /* Use your own protocol, good luck :) */
    }
}

void HttpConnection::setHints()
{

    this->hints = new addrinfo;

    memset(this->getHints(), 0, sizeof(struct addrinfo));

    this->getHints()->ai_family = this->getAddressFamily();
    this->getHints()->ai_socktype = this->getSocketType();
    this->getHints()->ai_protocol = this->getProtocol();
    this->getHints()->ai_flags = this->getFlags();
    this->getHints()->ai_addr = 0;
    this->getHints()->ai_addrlen = 0;
    this->getHints()->ai_canonname = 0;
    this->getHints()->ai_next = 0;

}


addrinfo *HttpConnection::getHints() const
{
    return this->hints;
}

const char *HttpConnection::getHostName() const
{
    return this->hostName->c_str();
}

void HttpConnection::setHostName(const char *hostName)
{
    this->meteoLog->err(hostName);
    this->hostName = new string(hostName);
}

const char *HttpConnection::getHostIp() const
{
    return this->hostIp->c_str();
}

void HttpConnection::setHostIp(const char *hostIp)
{
    this->meteoLog->err(hostIp);
    this->hostIp = new string(hostIp);
}

const char *HttpConnection::getPort() const
{
    return this->port->c_str();
}

void HttpConnection::setPort(const char *port)
{
    this->meteoLog->err(port);
    this->port = new string(port);
}

int HttpConnection::getSocketDescriptor() const
{
    return this->socketDescriptor;
}

void HttpConnection::setSocketDescriptor(int socketDescriptor)
{
    this->socketDescriptor = socketDescriptor;
}

int HttpConnection::getSocketType() const
{
    return this->socketType;
}

int HttpConnection::getProtocol() const
{
    return this->protocol;
}

int HttpConnection::getFlags() const
{
    return this->flags;
}

void HttpConnection::setFlags(int flags)
{
    this->flags = flags;
}

int HttpConnection::getAddressFamily() const
{
    return this->family;
}

const string &HttpConnection::getRequest() const
{
    return this->request;
}

void HttpConnection::setRequest(const string &request)
{
    this->request = request;
}

/*
void HttpConnection::collectGarbage()
{
    delete [] this->port;
    delete [] this->hostIp;
    delete [] this->hostName;
    delete [] this->configPath;
    delete [] this->requestPath;
    delete this->hints;
    delete this->meteoLog;

}
*/

HttpConnection::~HttpConnection()
{
    this->meteoLog->err("BYE BYE");
    /* Not recommended if app has to work without the Internet, throws segmentation fault on luck of the Internet */
    // this->collectGarbage();
}

size_t HttpConnection::getBytesToSend()
{
    return this->bytesToSend;
}

void HttpConnection::setBytesToSend(size_t bytesToSend)
{
    this->bytesToSend = bytesToSend;
}

void HttpConnection::httpClose()
{
    close(this->getSocketDescriptor());
}
