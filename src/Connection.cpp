/**
 * @author  Created by Marcin Guziołek on 06.04.18.
 */

#include "Connection.h"


#define TESTING_HTTP false
#define SIGPIPE_SOLUTION_SIGPROCMASK false
#define SIGPIPE_SOLUTION_SIGACTION true



Connection::Connection(const ConfigManager &configManager) : configManager(configManager),
                                                             hints(new addrinfo),
                                                             hostIp(),
                                                             httpRequest(),
                                                             waitNetworkTime(std::stoi(configManager.getConfig(
                                                                     "SocketWaitNetwork", configManager.SOCKET))),
                                                             httpRequestCreator(new HttpRequestCreator(configManager)),
                                                             meteoLog(new MyLog("CLASS::Connection::")) {


    this->setHostIp(configManager.getConfig("SocketHostIp", configManager.SOCKET));

    this->setPort(configManager.getConfig("SocketPort", configManager.SOCKET));

    this->setSocketType(configManager.getConfig("SocketType", configManager.SOCKET));

    this->setAddressFamily(configManager.getConfig("SocketAddressFamily", configManager.SOCKET));

    this->setProtocol(configManager.getConfig("SocketProtocol", configManager.SOCKET));

    this->setHints();

}

void Connection::connectServer() {

    struct addrinfo *unique_result,
            *results;

    int saved_err;

    /* Inicjalizuje obszar pamięci dla struktury 'addrinfo' */
    results = new addrinfo;


    /* Ustawia numeryczny adres hosta, zgodnie z ustawioną flagą AI_NUMERICHOST */
    std::string node = this->getHostIp();


    /* Pobiera informacje o podanym serwerze i serwisie udostępnianym przez niego, jeżeli jakiś istnieje */
    int gai = getaddrinfo(node.c_str(), this->getPort().c_str(), this->getHints(), &results);

    /* Funkcja 'getaddrinfo' zwróciła błąd */
    if (gai == -1) {

        this->meteoLog->err("connectServer(){ getaddrinfo(node,port,hints,results) = -1 }: ", errno);

        std::exit(EXIT_FAILURE);
    }

    /* Iteruje przez wszystkie wyniki z 'getaddrinfo', aby utworzyć gniazdo i połączyć je z serwerem */
    for (unique_result = results; unique_result != nullptr; unique_result = unique_result->ai_next) {

        /* Jeżeli nie powiodło się utworzenie gniazda z tym wynikiem kontynuuje z następnym */
        if (!this->createSocket(unique_result->ai_family, unique_result->ai_socktype, unique_result->ai_protocol)) {

            continue;
        }

        /* Połącz gniazdo z serwerem */
        int con = connect(this->getSocketDescriptor(), unique_result->ai_addr, unique_result->ai_addrlen);

        /* Jeżeli funkcja 'connect' zwróciła błąd, zapisuje go, żeby nie utracić */
        saved_err = errno;

        /* Funkcja 'connect' zwróciła błąd, połączenie się nie powiodło */
        if (con == -1) {

            if (saved_err == ENETUNREACH) {

                /*
                   Nie ma połączenia internetowego, zapisuje w dzienniku systemowym komunikat z czasem,
                   po którym nastąpi następna próba połączenia i przechodzi w stan uśpienia,
                   następnie po wybudzeniu wychodzi z pętli
                */
                this->waitNetwork();

                break;

            }

        } else {/* Połączenie powiodło się, wychodzi z pętli, można wysyłać dane */

            this->meteoLog->notice("Connection established! ");

            this->connected = true;

            break;
        }
    }

    /* Zwalnia pamięć struktury wyników wyszukań 'getaddrinfo' */
    freeaddrinfo(results);

    /* Jeżeli iteracja zakończyła się null-em,
       nie można było utworzyć gniazda z żadnym hostem, zakończ program */
    if (unique_result == nullptr) {

        /* Zapisuje w dzienniku systemowym komunikat o nieznalezieniu hosta do połączenia. */
        this->meteoLog->err("connectServer(){ getaddrinfo(unique_result==null) }: Niepowodzenie dla każdego hosta ");

        std::exit(EXIT_FAILURE);

    }

}


bool Connection::sendData(std::string &data) {

    /* Ilość wysyłanych bajtów */
    ssize_t bytesSentToServer,

    /* Maksymalna ilość bajtów odczytanych z serwera */
    serverResponseLength = 4196;

    /* Bufor do zapisu odpowiedzi serwera */
    auto *serverResponse = new char[serverResponseLength + 1]();

    /* Dodaje dane meteo do zapytania http i zapisuje do pola 'httpRequest' */
    this->setRequest(this->httpRequestCreator->createRequest(data));

    /* Jeżeli zapytanie http nie jest pustym ciągiem wysyłam je do serwera */
    if (!this->getRequest().empty()) {

        int saved_err, rs;

        /*
           Domyślne działanie sygnału 'SIGPIPE' jest kopiowane do 'old_sigact', a następnie
           ustawiane jest, działanie ignoruj sygnał SIGPIPE (zamykający process), po udanej próbie
           zapisu do gniazda na serwerze przywracane jest domyślne działanie sygnału 'SIGPIPE'
        */
#if SIGPIPE_SOLUTION_SIGACTION

        /* Struktura dla funkcji 'sigaction' */
        struct sigaction sigact{}, old_sigact{};

        /* Ustawia handler na ignoruj sygnał */
        sigact.sa_handler = SIG_IGN;

        /* Czyści maskę */
        sigemptyset(&sigact.sa_mask);

        /* Zeruje flagi */
        sigact.sa_flags = 0;

        /* Kopiuje domyślną akcję dla sygnału 'SIGPIPE' */
        rs = sigaction(SIGPIPE, nullptr, &old_sigact);

#endif

#if SIGPIPE_SOLUTION_SIGPROCMASK

        /* Ustawia maskę z zestawem blokowanych sygnałów, tylko 'SIGPIPE' */
        sigset_t sigpipe_mask;
        sigemptyset(&sigpipe_mask);
        sigaddset(&sigpipe_mask, SIGPIPE);

        rs = sigprocmask(SIG_SETMASK, &sigpipe_mask, nullptr);

#endif

        /* Zapisuje błąd, żeby go nie utracić */
        saved_err = errno;

        /* Został zwrócony błąd */
        if (rs == -1) {

#ifdef SIGPIPE_SOLUTION_SIGACTION

            this->meteoLog->err("sendData(data){ sigaction(SIGPIPE,NULL,oldact) = -1 }: ",
                                saved_err);

#endif

#if SIGPIPE_SOLUTION_SIGPROCMASK

            this->meteoLog->err("CLASS::Connection::sendData(data){ sigprocmas(SIG_SETMASK,sigpipe_mask,NULL) = -1 }: ",
                                saved_err);
#endif

            std::exit(EXIT_FAILURE);

        }

#if SIGPIPE_SOLUTION_SIGACTION

        /* Ignoruje sygnał 'SIGPIPE' wysyłany przy zapisie do zamkniętego przez host-a gniazda */
        rs = sigaction(SIGPIPE, &sigact, nullptr);

        /* Jeśli został zwrócony błąd, zapisuje go, żeby nie utracić */
        saved_err = errno;

        /* Funkcja 'sigaction' zwróciła błąd */
        if (rs == -1) {

            /* Wpis w dzienniku systemowym */
            this->meteoLog->err("sendData(data){ sigaction(SIGPIPE,sigact,NULL) = -1 }: ",
                                saved_err);
        }
#endif
        /* Wysyła dane meteo na serwer */
        bytesSentToServer = write(this->getSocketDescriptor(), this->getRequest().c_str(),
                                  this->getRequest().length());

        /* Jeśli został zwrócony błąd, zapisuje go, żeby nie utracić */
        saved_err = errno;

        /* Funkcja 'write' zwróciła błąd */
        if (bytesSentToServer == -1) {

            this->connected = false;

            this->meteoLog->err("sendData(data){ write(fd, buf, len) = -1 }: ", saved_err);

        } else if (bytesSentToServer > 0) {

            this->connected = true;

#if SIGPIPE_SOLUTION_SIGACTION

            /* Przywraca domyślną akcję dla sygnału SIGPIPE */
            rs = sigaction(SIGPIPE, &old_sigact, nullptr);

            /* Jeśli został zwrócony błąd, zapisuje go, żeby nie utracić */
            saved_err = errno;

            /* Funkcja 'sigaction' zwróciła błąd */
            if (rs == -1) {

                /* Wpis w dzienniku systemowym */
                this->meteoLog->err("sendData(data){ sigaction(SIGPIPE,oldact,NULL) = -1 }: ",
                                    saved_err);
            }
#endif

#if TESTING_HTTP

            /* Odczytuję odpowiedź serwera */
            serverResponseLength = read(this->getSocketDescriptor(), serverResponse, serverResponseLength);

            /* Zapisuje błąd zwrócony przez 'read', żeby go nie utracić */
            saved_err = errno;

            /* Funkcja 'read' zwróciła błąd */
            if (serverResponseLength == -1) {

                this->meteoLog->err("TESTING_HTTP::CLASS::Connection::sendData(data){ read(fd,buf,len) = -1 }: ",
                                    saved_err);

            }

            // Wyświetlam w dzienniku systemowym długość wysłanego do serwera zapytania http
            this->meteoLog->notice("TESTING_HTTP::Request length sent by http: ", &serverResponseLength);

            /* Zapisuje do pliku wysłane zapytanie http */
            std::string temppath = "/home/marcin/";
            temppath += "myrequest.txt";

            int fd = open(temppath.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0777);

            /* Zapisuje błąd zwrócony przez 'open', żeby go nie utracić */
            saved_err = errno;

            /* Funkcja 'open' zwróciła błąd */
            if (fd == -1) {

                this->meteoLog->err(
                        "TESTING_HTTP::Connection::sendData(data){ open(myrequest.txt,O_CREAT | O_WRONLY | O_TRUNC,0777) = -1 }: ",
                        saved_err);
            }

            ssize_t len = write(fd, this->getRequest().c_str(), this->getRequest().length());

            /* Zapisuje błąd zwrócony przez 'write', żeby go nie utracić */
            saved_err = errno;

            /* Funkcja write zwróciła błąd */
            if (len == -1) {

                this->meteoLog->err("TESTING_HTTP::CLASS::sendData(data){ write(myrequest.txt-Desc,buf,len) }: ",
                                    saved_err);
            }

            close(fd);


            // Zapisuję do pliku odpowiedź servera
            temppath = "/home/marcin/";
            temppath += "serverresponse.txt";

            fd = open(temppath.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0777);

            /* Zapisuje błąd zwrócony przez 'open', żeby go nie utracić */
            saved_err = errno;

            /* Funkcja 'open' zwróciła błąd */
            if (fd == -1) {

                this->meteoLog->err(
                        "TESTING_HTTP::CLASS::Connection::sendData(data){ open(serverresponse.txt,O_CREAT | O_WRONLY | O_TRUNC,0777) = -1 }: ",
                        saved_err);
            }

            len = write(fd, serverResponse, serverResponseLength);

            /* Zapisuje błąd zwrócony przez 'write', żeby go nie utracić */
            saved_err = errno;

            /* Funkcja 'write' zwróciła błąd */
            if (len == -1) {

                this->meteoLog->err(
                        "TESTING_HTTP::CLASS::Connection::sendData(data){ write(serverresponse.txt-Desc,buf,len) = -1 }: ",
                        saved_err);
            }

            close(fd);
#endif

        }
    }

    return this->connected;
}


/* PRIVATE METHODS */

bool Connection::createSocket(int ai_family, int ai_socktype, int ai_protocol) {

    bool *created = new bool;

    /* Zamyka ostatnio używane gniazdo, zanim otworzy nowe */
    this->closeConnection();


    /* Tworzy nowe gniazdo */
    int sd = socket(ai_family, ai_socktype, ai_protocol);

    /* Zapisuje błąd, żeby go nie utracić */
    int saved_err = errno;

    /* Funkcja 'socket' zwróciła błąd */
    if (sd == -1) {

        *created = false;

        this->meteoLog->err(
                "creatSocket(family,socktype,protocol){ socket(family,socktype,protocol) = -1 }: ",
                saved_err);

    } else { /* Funkcja 'socket' nie zwróciła błędu */

        /* Ustawiam uchwyt do utworzonego gniazda */
        this->setSocketDescriptor(sd);

        *created = true;
    }

    return *created;
}

void Connection::setProtocol(const std::string &pprotocol) {

    if (toLowerCase(pprotocol,*this->meteoLog) != "none") {

        /*
           Pobiera z bazy danych wartość liczbową protokołu odpowiadającej oficjalnej zadanej nazwie,
           jeżeli nazwa niepoprawna lub jej brak zwraca null pointer
        */
        struct protoent *result = getprotobyname(pprotocol.c_str());

        /* Funkcja 'getprotobyname' zwróciła strukturę 'protoent' */
        if (result != nullptr) {

            this->protocol = result->p_proto;

        } else { /* Funkcja 'getprotobyname' zwróciła null pointer */

            /* Zapisuje do dziennika systemowego komunikat o braku w bazie danych nazwy protokołu. */
            this->meteoLog->err(
                    "setProtocol(protocol){ getprotobyname(protocol) }: Nie znaleziono takiego protokołu w /etc/protocols. ");
        }

    }
}

/*
   Usypia proces na zadany czas i zapisuje do dziennika systemowego informację
   o oczekiwaniu na połączenie internetowe
*/
void Connection::waitNetwork() {

    std::string msg = "Waiting for INTERNET CONNECTION! Retrying in seconds: " + std::to_string(this->getWaitNetwork());
    this->meteoLog->info(msg.c_str());

    this->connected = false;

    sleep(this->getWaitNetwork());
}


void Connection::setAddressFamily(const std::string &pfamily) {

    if (toLowerCase(pfamily, *this->meteoLog) != "both") {
        if (pfamily == "ip4") {
            this->family = AF_INET;
        } else if (pfamily == "ip6") {
            this->family = AF_INET6;
        }
    }
}

void Connection::setSocketType(const std::string &psocketType) {

    if (toLowerCase(psocketType, *this->meteoLog) != "tcp") {
        if (psocketType == "udp") {
            this->socketType = SOCK_DGRAM;
        } else if (psocketType == "raw") {
            this->socketType = SOCK_RAW; /* Używaj swojego protokołu, powodzenia :) */
        }
    }
}

void Connection::setHints() {

    /* Inicjuje pamięć dla struktury 'hints' odpowiednio do rozmiaru struktury 'addrinfo' */
    memset(this->getHints(), 0, sizeof(struct addrinfo));

    /* Ustawia odpowiednio pierwsze cztery elementy struktury 'hints',
     * pozostałe null lub 0
     * */

    this->getHints()->ai_family = this->getAddressFamily();
    this->getHints()->ai_socktype = this->getSocketType();
    this->getHints()->ai_protocol = this->getProtocol();
    this->getHints()->ai_flags = this->getFlags();
    this->getHints()->ai_addr = nullptr;
    this->getHints()->ai_addrlen = 0;
    this->getHints()->ai_canonname = nullptr;
    this->getHints()->ai_next = nullptr;

}


addrinfo *Connection::getHints() const {
    return this->hints;
}


std::string Connection::getHostIp() const {
    return this->hostIp;
}


void Connection::setHostIp(const std::string &phostIp) {
    this->hostIp = phostIp;
}


std::string Connection::getPort() const {
    return this->port;
}


void Connection::setPort(const std::string &pport) {
    this->port = pport;
}


int Connection::getSocketDescriptor() const {
    return this->socketDescriptor;
}


void Connection::setSocketDescriptor(int psocketDescriptor) {
    this->socketDescriptor = psocketDescriptor;
}


int Connection::getSocketType() const {
    return this->socketType;
}


int Connection::getProtocol() const {
    return this->protocol;
}


int Connection::getFlags() const {
    return this->flags;
}


int Connection::getAddressFamily() const {
    return this->family;
}

std::string Connection::getRequest() const {
    return this->httpRequest;
}


void Connection::setRequest(const std::string &prequest) {
    this->httpRequest = prequest;
}


int Connection::getWaitNetwork() const {
    return this->waitNetworkTime;
}


bool Connection::isConnected() const {

    return this->connected;
}


void Connection::closeConnection() {

    close(this->getSocketDescriptor());
    this->connected = false;

}


/* Konstruktor kopiujący */
Connection::Connection(Connection &connection) : configManager(connection.configManager) {

    this->waitNetworkTime = connection.getWaitNetwork();

    this->httpRequestCreator = new HttpRequestCreator(connection.configManager);
    this->httpRequestCreator = connection.httpRequestCreator;

    this->meteoLog = new MyLog("MS::SocketConnection");
    this->meteoLog = connection.meteoLog;


    this->hints = new struct addrinfo;
    this->hints = connection.getHints();

}


/* Destruktor */
Connection::~Connection() {

    delete this->meteoLog;
    delete this->hints;

}
