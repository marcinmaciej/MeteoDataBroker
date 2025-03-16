/**
 * @author  Created by Marcin Guziołek on 06.04.18.
 */

#ifndef HTTPCONNECTION_H
#define HTTPCONNECTION_H

#include <netdb.h> /* getaddrinfo(), freeaddrinfo(), connect(), socket(), getprotobyname(),
                    * struct protoent, struct addrinfo,
                    * AI_NUMERICHOST, AI_NUMERICSERV,
                    * AF_UNSPEC, AF_INET, AF_INET6,
                    * SOCK_DGRAM, SOCK_RAW, SOCK_STREAM
                    * */

#include <csignal> /* struct sigaction, sigaction(), sigemptyset(), SIG_IGN, SIGPIPE */
#include <cstring>  /* memset */
#include <unistd.h>  /* write(), close(), sleep() */
#include <string> /* std::string */

#include "MyLog.h"
#include "ConfigManager.h"
#include "HttpRequestCreator.h"

extern std::string toLowerCase(const std::string &pstr, MyLog &meteoLog);

class Connection {

public:

    explicit Connection(const ConfigManager &configManager);

    Connection(const Connection &connection);

    ~Connection();

    void connectServer();

    bool sendData(std::string &data);

    void closeConnection();

    bool isConnected() const;

private:

    const ConfigManager &configManager;

    MyLog *meteoLog;

    HttpRequestCreator *httpRequestCreator;

    bool connected = false;

    struct addrinfo *hints;


    std::string hostIp, /* Numeryczny adres hosta jako ciąg czterech liczb odseparowanych kropkami */
    port, /* Numer portu, na którym host udostępnia serwis */
    httpRequest; /* Zapytanie http */


    int socketDescriptor = -1, /* Uchwyt do gniazda */

    socketType = SOCK_STREAM,  /* Używaj gniazda typu protokół strumieniowy */

    protocol = 0, /* Używaj jakiegokolwiek protokołu dla typu gniazda 'SOCK_STREAM' */

    /* Wymagaj numerycznego adresu hosta, nie wyszukuj nazwy hosta, aby
     * uniknąć wszelkich potencjalnie długich wyszukiwań adresów hostów |
     * nie konwertuj numeru portu do nazwy
     * */
    flags = AI_NUMERICHOST | AI_NUMERICSERV,


            family = AF_UNSPEC,  /* Używa obu rodzin protokołów ipv4 i ipv6 */

    waitNetworkTime; /* Czas uśpienia w oczekiwaniu na połączenie internetowe */

    size_t bytesToSend = 0;

    bool createSocket(int ai_family, int ai_socktype, int ai_protocol);


    /* Ustaw lub pobierz parametry socket za pomocą tych metod */

    /* Getters */

    int getFlags() const;

    addrinfo *getHints() const;

    std::string getHostIp() const;

    std::string getPort() const;

    int getSocketType() const;

    int getProtocol() const;

    int getAddressFamily() const;

    int getSocketDescriptor() const;

    std::string getRequest() const;


    /* Setters */

    void setRequest(const std::string &prequest);

    void setHints();

    void setAddressFamily(const std::string &pfamily);

    void setHostIp(const std::string &phostIp);

    void setPort(const std::string &pport);

    void setSocketDescriptor(int psocketDescriptor);

    void setSocketType(const std::string &psocketType);

    void setProtocol(const std::string &pprotocol);

    /*------------------------------------------*/

    int getWaitNetwork() const;

    void waitNetwork();

};

#endif // HTTPCONNECTION_H
