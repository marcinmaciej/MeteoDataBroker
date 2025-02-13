/**
 * @author  Created by Marcin Guziołek on 06.04.18.
 */

#include "HttpConnection.h"
#include <iostream>

#define TESTING true
#define TESTING_HTTP true

HttpConnection::HttpConnection(const ConfigManager &configManager) {

    this->hostIp = "";
    this->hostName = "";
    this->request = "";
    this->requestContent = "";

    this->hints = new addrinfo;

    this->meteoLog = new MyLog("MS-HttpConnection");

    this->setHostIp(configManager.getHttpConfig("HttpHostIp"));

    this->setHostName(configManager.getHttpConfig("HttpHostName"));

    this->setPort(configManager.getHttpConfig("HttpPort"));

    this->setSocketType(configManager.getHttpConfig("HttpSocketType"));

    this->setAddressFamily(configManager.getHttpConfig("HttpAddressFamily"));

    this->setProtocol(configManager.getHttpConfig("HttpProtocol"));

    this->setRequest(configManager.getHttpConfig("HttpRequest"));

    this->setDataKeyName(configManager.getHttpConfig("DataKeyName"));

    this->setHints();

}

int HttpConnection::httpConnect() {

    struct addrinfo *rp,
            *result;

    result = new addrinfo;

    try {

        auto *node = new string(
                (this->getHints()->ai_flags == AI_NUMERICHOST) ? this->getHostIp() : this->getHostName());

        if (getaddrinfo(node->c_str(), this->getPort().c_str(), this->getHints(), &result) > 0) {
            throw;
        }


        for (rp = result; rp != nullptr; rp = rp->ai_next) {
            this->createSocket(rp->ai_family);

            if (this->getSocketDescriptor() == -1) {
                continue; /* Socket not created yet */
            }

            if (connect(this->getSocketDescriptor(), rp->ai_addr, rp->ai_addrlen) != -1) {
                break; /* Success */
            }
        }

        freeaddrinfo(result);

        //delete node;

        if (rp == nullptr) {
            return 0;
        }

#if TESTING
        this->meteoLog->notice("Http connected!");
#endif // TESTING

        return 1;

    }
    catch (...) {

        this->meteoLog->err(strerror(errno));

    }
    return 0;
}

bool HttpConnection::sendData(std::string &data) {

    /* Ilość wysałanych bajtów */
    ssize_t bytesSentToServer = 0;

    /* Ilość bajtów odczytanych z serwera */
    ssize_t serverResponseLength = 4196;

    /* Bufor do zapisu odpowiedzi serwera */
    auto *serverResponse = new char[serverResponseLength+1]();

    if (this->getSocketDescriptor() >= 0) {

        try {
            const std::string dataToSend = this->mergeRequestAndData(data);

            /* send data to the server */
            if (!dataToSend.empty()) {
                bytesSentToServer = write(this->getSocketDescriptor(), dataToSend.c_str(), this->getBytesToSend());
            }

            if (bytesSentToServer > 0) {

                /* Odczytuję odpowiedź serwera */
                serverResponseLength = read(this->getSocketDescriptor(), serverResponse, serverResponseLength);


#if TESTING_HTTP

                // Wyświetlam w dzienniku długość wysłanego do serwera zapytania http
                std::string dataSentLength("Request length sent by http ----> ");

                dataSentLength.append(to_string(bytesSentToServer));

                this->meteoLog->info(dataSentLength.c_str());

                // Zapisuje do pliku wysłane zapytanie http
                std::string temppath("/home/marcin/myrequest.txt");

                int fd = open(temppath.c_str(), O_CREAT | O_WRONLY, 0777);

                write(fd, dataToSend.c_str(), dataToSend.length());
                close(fd);

                // Zapisuję do pliku odpowiedź servera
                temppath.assign("/home/marcin/serverresponse.txt");

                fd = open(temppath.c_str(), O_CREAT | O_WRONLY, 0777);

                write(fd, serverResponse, serverResponseLength);
                close(fd);

#endif

                return true;
            }
        }
        catch (std::exception &e) {

            //this->httpClose(); Połączenie jest tu zamykane (NAWET W DESTRUKTORZE) co powoduje nie wykonanie zapytania POST i brak odpowiedzi serwera
            this->meteoLog->err(e.what());
        }
    }
    return false;
}

/* PRIVATE METHODS */

const std::string &HttpConnection::mergeRequestAndData(std::string &data) {

    std::string dataLengthAsString,
            trequest;

    this->requestContent = "";

    /*
     * Ustalam długość danych
     * Długość pary 'klucz=wartość'
     * */
    size_t len = this->getDataKeyName().length();
    len += data.length();

    /* Pobieram zapytanie załadowane z pliku konfiguracyjnego */
    trequest = this->getRequest();

    /* Znajduję pozycję ogranicznika w zapytaniu */
    std::string::size_type start = trequest.find(placeHolder),
            delimLen = placeHolder.length();

    /* Konwertuje liczbę danych do stringa */
    dataLengthAsString = to_string(len);

    /* Podmieniam ogranicznik w nagłówku "content-length" zapytania http wartością liczbową */
    if (!trequest.replace(start, delimLen, dataLengthAsString).empty()) {

        /* Dodaję do zapytania http dane do wysłania
         * w formie klucz=wartość */
        trequest.append(this->getDataKeyName());
        trequest.append("=");
        trequest.append(data);

        /* Ustawiam pole requestContent aby móc zwrócić stałą referencję */
        this->requestContent = trequest;

        this->setBytesToSend(this->requestContent.length());

        return this->requestContent;
    }

    return this->requestContent;

}

void HttpConnection::createSocket(int ai_family) {
    try {

        this->setSocketDescriptor(socket(ai_family, this->getSocketType(), this->getProtocol()));

    }
    catch (...) {
        this->meteoLog->err(strerror(errno));
    }
}

void HttpConnection::setProtocol(const std::string &pprotocol) {

    if (this->toLowerCase(pprotocol) != "none") {

        this->protocol = std::stoi(pprotocol);
    }
}

void HttpConnection::setAddressFamily(const std::string &pfamily) {

    if (this->toLowerCase(pfamily) != "both") {
        if (pfamily == "ip4") {
            this->family = AF_INET;
        } else if (pfamily == "ip6") {
            this->family = AF_INET6;
        }
    }
}

void HttpConnection::setSocketType(const std::string &psocketType) {

    if (this->toLowerCase(psocketType) != "tcp") {
        if (psocketType == "udp") {
            this->socketType = SOCK_DGRAM;
        } else if (psocketType == "raw") {
            this->socketType = SOCK_RAW; /* Używaj swojego protokołu, powodzenia :) */
        }
    }
}

void HttpConnection::setHints() {

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


addrinfo *HttpConnection::getHints() const {
    return this->hints;
}

std::string HttpConnection::getHostName() const {
    return this->hostName;
}

void HttpConnection::setHostName(const std::string &phostName) {
    this->hostName = phostName;
}

std::string HttpConnection::getHostIp() const {
    return this->hostIp;
}

void HttpConnection::setHostIp(const std::string &phostIp) {
    this->hostIp = phostIp;
}

std::string HttpConnection::getPort() const {
    return this->port;
}

void HttpConnection::setPort(const std::string &pport) {
    this->port = pport;
}

int HttpConnection::getSocketDescriptor() const {
    return this->socketDescriptor;
}

void HttpConnection::setSocketDescriptor(int psocketDescriptor) {
    this->socketDescriptor = psocketDescriptor;
}

int HttpConnection::getSocketType() const {
    return this->socketType;
}

int HttpConnection::getProtocol() const {
    return this->protocol;
}

int HttpConnection::getFlags() const {
    return this->flags;
}

void HttpConnection::setFlags(int pflags) {
    this->flags = pflags;
}

int HttpConnection::getAddressFamily() const {
    return this->family;
}

std::string HttpConnection::getRequest() const {
    return this->request;
}

void HttpConnection::setRequest(const std::string &prequest) {
    this->request = prequest;
}

std::string HttpConnection::getDataKeyName() const {
    return this->dataKeyName;
}

void HttpConnection::setDataKeyName(const std::string &pkeyname) {
    this->dataKeyName = pkeyname;
}

size_t HttpConnection::getBytesToSend() {
    return this->bytesToSend;
}

void HttpConnection::setBytesToSend(size_t pbytesToSend) {
    this->bytesToSend = pbytesToSend;
}

void HttpConnection::httpClose() {
    close(this->getSocketDescriptor());
}

std::string HttpConnection::toLowerCase(const std::string &pstring) {

    std::string temp = pstring;
    transform(temp.begin(), temp.end(), temp.begin(), ::tolower); /* Zmień litery na małe */
    return temp;

}
/* Destruktor */
HttpConnection::~HttpConnection() {

    delete this->meteoLog;
    delete this->hints;

}
/* Konstruktor kopiujący */
HttpConnection::HttpConnection(HttpConnection &httpConnection) {

    this->meteoLog = new MyLog("MS-HttpdConnection");
    this->meteoLog = httpConnection.meteoLog;

    this->hints = new struct addrinfo;
    this->hints = httpConnection.hints;

}
