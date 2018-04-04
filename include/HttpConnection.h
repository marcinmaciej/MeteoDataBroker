#ifndef HTTPCONNECTION_H
#define HTTPCONNECTION_H

#include <netdb.h>
#include <sys/socket.h>

#include "MeteoLog.h"

using namespace std;


class HttpConnection
{

public:
    HttpConnection();
    ~HttpConnection();

    int httpConnect();
    bool sendData(const char * data);

private:

    MeteoLog * meteoLog;

    struct addrinfo * hints;

    const char  * hostName,
          * hostIp,
          * port,
          * configPath,
          * requestPath;

    const unsigned short LENGTH = 1025;

    int socketDescriptor = -1,
        socketType,
        protocol,
        flags,
        family;

    string request;

    /* Class private methods */

    int getSocketDescriptor();

    int getSocketType();
    int getAddressFamily();
    int getProtocol();
    int getFlags();

    unsigned short getRequestLength();

    struct addrinfo * getHints();

    const char * getRequest();
    const char * getHostName();
    const char * getHostIp();
    const char * getPort();
    const char * getConfigPath();
    const char * getRequestPath();


    /* Parameters of function: getaddrinfo */

    /* Set parameters from config file*/
    void setHostIp(const char * hostIp = "95.211.144.65");
    void setHostName(const char * hostName = "www.meteo-station.cba.pl");
    void setPort(const char * port = "80");

    /* Parameter returned by socket function */
    void setSocketDescriptor(int descriptor);

    /* Sets hints */
    void setHints();

    /* Internally set parameter */
    /* Require numeric address to avoid potential lengthy lookup calls */
    void setFlags(int flags = AI_NUMERICHOST);

    /* Socket parameters */

    /* Set parameters from config file*/
    void setAddressFamily(string family = "both");
    void setSocketType(string type = "tcp");
    void setProtocol(string protocol = "none");

    /* Set http request and data for it */
    void setRequest(string data);

    /* Set file path for loading configuration of socket connection. */
    void setConfigPath(const char * path = "/etc/meteo-station/config/config.http");

    /* Set file path for loading http request. */
    void setRequestPath(const char * path = "/etc/meteo-station/config/request.http");


    /* load from file configuration of socket connection */
    void loadConfig();

    /* load from file http request */
    string loadRequest();

    char * trim(const char * str);

    void createSocket(int ai_family);

    void collectGarbage();
};

#endif // HTTPCONNECTION_H
