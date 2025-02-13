/**
 * @author  Created by Marcin Guziołek on 06.04.18.
 */

#ifndef HTTPCONNECTION_H
#define HTTPCONNECTION_H

#include <netdb.h>
#include <sys/socket.h>

#include "ConfigManager.h"

class HttpConnection {

public:
    explicit HttpConnection(const ConfigManager &configManager);

    HttpConnection(HttpConnection &httpConnection);

    ~HttpConnection();

    int httpConnect();

    bool sendData(std::string &data);

    void httpClose();

private:

    MyLog *meteoLog;

    struct addrinfo *hints;

    /* W pliku konfiguracyjnym "/etc/meteo-station/config/http.yaml"
     * w zapytaniu http w nagłówku content-length
     * zamiast liczby określającej długość zawartości zapytania,
     * której jeszcze nie znamy znajduje się ogranicznik "#####"
     *  */
    std::string placeHolder = "#####";

    std::string hostName,
            hostIp,
            port,
            request,
            requestContent,
            dataKeyName;

    int socketDescriptor = -1,

    /* Używaj protokołu TCP  */
    socketType = SOCK_STREAM,

    /* 0 dla opcji "none" */
    protocol = 0,

    /* Wymagaj numercznego adresu hosta aby uniknąć potencjalnych długich wywołań lookup */
    flags = AI_NUMERICHOST,

    /* Używaj obu rodzin adresowych */
    family = AF_UNSPEC;

    size_t bytesToSend = 0;

    void createSocket(int ai_family);


    /* Ustaw lub pobierz parametry socket za pomocą tych metod */
    int getFlags() const;

    addrinfo *getHints() const;

    std::string getHostName() const;

    std::string getHostIp() const;

    std::string getPort() const;

    int getSocketType() const;

    int getProtocol() const;

    int getAddressFamily() const;

    int getSocketDescriptor() const;

    void setHints();

    void setAddressFamily(const std::string &pfamily);

    void setHostName(const std::string &phostName);

    void setHostIp(const std::string &phostIp);

    void setPort(const std::string &pport);

    void setSocketDescriptor(int psocketDescriptor);

    void setSocketType(const std::string &psocketType);

    void setProtocol(const std::string &pprotocol);

    void setFlags(int pflags);
    /*------------------------------------------*/

    /* Metody do pobierania, ustawiania i przetwarzania zapytania http */
    std::string getRequest() const;

    void setRequest(const std::string &prequest);

    const std::string &mergeRequestAndData(std::string &data);

    std::string getDataKeyName() const;

    void setDataKeyName(const std::string &pkeyname);

    size_t getBytesToSend();

    void setBytesToSend(size_t pbytesToSend);

    /* Metody pomocnicze */

    std::string toLowerCase(const std::string &pstring);
};

#endif // HTTPCONNECTION_H
