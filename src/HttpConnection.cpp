#include "HttpConnection.h"


HttpConnection::HttpConnection()
{

    meteoLog = new MeteoLog("MS-HttpdConnectionClass");

    this->port = new char[5];
    this->hostIp = new char[15];
    this->hostName = new char[256];
    this->configPath = new char[this->LENGTH];
    this->requestPath = new char[this->LENGTH];

    this->setConfigPath();

    this->setRequestPath();

    this->loadConfig();

    this->loadRequest();

    this->setHints();

}

int HttpConnection::httpConnect()
{

    const char * node = new char[this->LENGTH];
    struct addrinfo * rp,
            *result;

    result = new addrinfo;

    try
{

        node = (this->getHints()->ai_flags == AI_NUMERICHOST)? this->getHostIp() : this->getHostName();

        if(getaddrinfo(node, this->getPort(), this->getHints(), &result) > 0)
        {
            throw errno;
        }


        for (rp = result; rp != nullptr; rp = rp->ai_next)
        {
            this->createSocket(rp->ai_family);

            if (this->getSocketDescriptor() == -1)
            {
                continue; /* Socket not created yet */
            }

            if(connect(this->getSocketDescriptor(), rp->ai_addr, rp->ai_addrlen) != -1)
            {
                break; /* Success */
            }

            close(this->getSocketDescriptor());
        }

        freeaddrinfo(result);

        if(rp == nullptr)
        {
            throw errno;
        }

#if TESTING
        this->meteoLog->notice("Http connected!");
#endif // TESTING

        return 0;

    }
    catch(...)
    {
        this->meteoLog->err(strerror(errno));

        /*
         * delete node;
         * delete result;
         */

      return -1;
    }
}

bool HttpConnection::sendData(const char * data)
{
    unsigned short bytesToSend = 0;
    ssize_t bytesSent = 0;

    if(this->getSocketDescriptor() >= 0)
    {
        /* set http request */
        this->setRequest(data);
        bytesToSend = this->getRequestLength();

        /* send data to the server */
        bytesSent = write(this->getSocketDescriptor(), this->getRequest(), bytesToSend);

        if(bytesSent == bytesToSend)
        {
            return true;
        }
    }

    return false;
}

/* PRIVATE METHODS */


void HttpConnection::createSocket(int ai_family)
{
    try
    {

        this->setSocketDescriptor(socket(ai_family,this->getSocketType() ,this->getProtocol()));

        if(this->getSocketDescriptor() == -1)
        {
            throw errno;
        }

    }
    catch(...)
    {
        this->meteoLog->err(strerror(errno));
    }
}


/* GETTERS */

const char * HttpConnection::getConfigPath()
{
    return this->configPath;
}

const char * HttpConnection::getRequestPath()
{
    return this->requestPath;
}

struct addrinfo * HttpConnection::getHints()
{
    return this->hints;
}

int HttpConnection::getSocketType()
{
    return this->socketType;
}

int HttpConnection::getFlags()
{
    return this->flags;
}

int HttpConnection::getSocketDescriptor()
{
    return this->socketDescriptor;
}

const char *  HttpConnection::getRequest()
{
    return this->request.c_str();
}

const char * HttpConnection::getPort()
{
    return this->port;
}

int HttpConnection::getProtocol()
{
    return this->protocol;
}

const char * HttpConnection::getHostIp()
{
    return this->hostIp;
}

const char * HttpConnection::getHostName()
{
    return this->hostName;
}

int HttpConnection::getAddressFamily()
{
    return this->family;
}

unsigned short HttpConnection::getRequestLength()
{
    return static_cast<unsigned short>(this->request.length());
}


/* SETTERS */

void HttpConnection::setRequest(string data)
{

    const string DELIMITER = "#####", /* Delimiter in place of data length number in content-length header of http request */
                 NEWLINE = "\r\n";

    string dataToSend = "data=" + data; /* Can change or remove it if needed */

    size_t len = dataToSend.length();

    /* Load request string from the config file */
    string request = this->loadRequest();

    /* Put data length in content-length header we don't know the data length before */
    request.replace(request.find_first_of(DELIMITER), 5, to_string(len), 0, len);

    this->request += request;
    this->request += NEWLINE; /* add second new line to the request (already is one) */
    this->request += dataToSend;

}

void HttpConnection::setFlags(int flags)
{
    this->flags = flags;
}

void HttpConnection::setConfigPath(const char * path)
{
    this->configPath = path;
}

void HttpConnection::setRequestPath(const char * path)
{
    this->requestPath = path;
}

void HttpConnection::setSocketDescriptor(int descriptor)
{
    this->socketDescriptor = descriptor;
}

void HttpConnection::setPort(const char * port)
{
    this->port = this->trim(port);
}

void HttpConnection::setHostName(const char * hostName)
{
    this->hostName = this->trim(hostName);
}

void HttpConnection::setHostIp(const char * hostIp)
{
    this->hostIp = this->trim(hostIp);
}

void HttpConnection::setProtocol(string protocol)
{
    protocol = this->trim(protocol.c_str());
    transform(protocol.begin(),protocol.end(),protocol.begin(), ::tolower); /* Change all letters to lower */

    if(protocol == "none")
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
    family = this->trim(family.c_str());
    transform(family.begin(),family.end(),family.begin(), ::tolower); /* Change all letters to lower */

    if(family == "ip4")
    {
        this->family = AF_INET;
    }
    else if(family == "ip6")
    {
        this->family = AF_INET6;
    }
    else if(family == "both")
    {
        this->family = AF_UNSPEC;
    }
}

void HttpConnection::setSocketType(string type)
{
    type = this->trim(type.c_str());
    transform(type.begin(),type.end(),type.begin(), ::tolower); /* Change all letters to lower */

    if(type == "tcp")
    {
        this->socketType = SOCK_STREAM;
    }
    else if(type == "udp")
    {
        this->socketType = SOCK_DGRAM;
    }
    else if(type == "raw")
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

void HttpConnection::loadConfig()
{

    this->setFlags(); /* setting internally flag AI_NUMERICHOST */

    FILE * fd;

    size_t len = 0;
    unsigned short i = 0;

    char * line = nullptr; /* Null pointer points to address 0. Don't use old macro NULL */

    /* Array as container for configuration parameters of socket connection */
    string  config[6];

    const string BEGINDELIMITER = "=",
                 ENDDELIMITER  = "\r\n",
                 NEWLINE = "\n";


    if(!(fd = fopen(this->getConfigPath(), "r")))
    {

        throw errno;

    }

    string  temp;
    /* read line by line file with configuration parameters of socket connection */
    while(getline(&line, &len, fd) != -1)
    {
        temp.clear();
        temp = line;

        /* ignore new line and empty strings and continue */
        if(temp.empty() || NEWLINE == temp || ENDDELIMITER == temp)
        {
            continue;
        }
        else
        {
            /* substring parameters */
            temp = temp.substr((temp.find_first_of(BEGINDELIMITER)+1),temp.length());

            temp = temp.erase(temp.find_first_of(ENDDELIMITER));

            /* Don't overflow the array */
            if(i<6)
            {
                *(config+i) = temp;
                i++;
            }
        }
    }

    delete line;

    fclose(fd);

    /* Secure from empty strings parameters. No parameters validation. */
    if(!((*config).empty()))
    {
        this->setHostIp((*config).c_str());
    }
    else
    {
        this->setHostIp();
    }


    if(!((*(config+1)).empty()))
    {
        this->setHostName((*(config+1)).c_str());
    }
    else
    {
        this->setHostName();
    }


    if(!((*(config+2)).empty()))
    {
        this->setPort((*(config+2)).c_str());
    }
    else
    {
        this->setPort();
    }


    if(!((*(config+3)).empty()))
    {
        this->setAddressFamily(*(config+3));
    }
    else
    {
        this->setAddressFamily();
    }


    if(!((*(config+4)).empty()))
    {
        this->setSocketType(*(config+4));
    }
    else
    {
        this->setSocketType();
    }


    if(!((*(config+5)).empty()))
    {
        this->setProtocol((*(config+5)));
    }
    else
    {
        this->setProtocol();
    }
}

string HttpConnection::loadRequest()
{

    auto * request = new string();
    FILE * fd;
    auto * line = new char[this->LENGTH];


    if(!(fd = fopen(this->getRequestPath(),"r")))
    {
        throw errno;
    }

    /* read all lines from file */
    while(fgets(line, this->LENGTH, fd))
    {
        /* merge request */
        *request +=  *line;
        *line = {0};

    }

    delete line;

    fclose(fd);

    return *request;
}

char * HttpConnection::trim(const char * str)
{

    auto * trimmed = new char[1024];

    sprintf(trimmed, "%s", str);

    return trimmed;

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

   // this->collectGarbage(); /* Not recommended if app has to work without the Internet */
}
