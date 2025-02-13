//
// Created by marcin on 26.12.2021.
//

#include "HttpRequestCreator.h"

#define SHOW_REQUEST true

HttpRequestCreator::HttpRequestCreator(const ConfigManager &configManager) {

    /* Inicjalizuje obsługę logów */
    this->meteoLog = new MyLog("MS::HttpRequestCreator");

    /* Ustawiam metodę i ścieżkę URL zasobu w zapytaniu http */
    this->httpMethod = configManager.getHttpConfig("HttpMethod"); /* Metoda zapytania http*/
    this->httpPath = configManager.getHttpConfig("HttpPath");  /* Ścieżka URL do zasobu w zapytaniu http */

    /* Nazwa klucza dla wartości danych */
    this->httpDataKeyName = configManager.getHttpConfig("HttpDataKeyName");

    /* Kopiuje mapę z nagłówkami zapytania http */
    this->headers = configManager.getHttpHeaders();

    char name[128];

    /* Pobiera nazwę urządzenia */
    gethostname(name, 128);

    /* Pobiera nazwę użytkownika urządzenia i ustawia wraz z nazwą urządzenia
       w nagłówku 'User-agent' jako nazwę przeglądarki */
    this->httpUserAgent += getenv("USER");

    this->httpUserAgent += " from ";

    this->httpUserAgent += name;
}

std::string HttpRequestCreator::createRequest(const string &data) {

    /* Obsługuje metody 'POST', 'GET', 'PUT', 'PATCH' */
    if (this->httpMethod == "POST" || this->httpMethod == "GET" || this->httpMethod == "PUT" ||
        this->httpMethod == "PATCH") {

        /* Formuje pierwszy wiersz zapytania 'HTTP' zgodnie ze specyfikacją */

        /* Metoda zapytania */
        this->request = this->httpMethod;
        this->request += SPACE;


        /* Jeżeli używa metody http 'GET' dane dodawane do ścieżki URL */
        if (this->httpMethod == "GET") {

            /* Ścieżka URL do żądanego zasobu */
            this->request += this->httpPath;

            /* Znak zapytania, oddzielający ścieżkę od danych */
            this->request += QUESTION_MARK;

            /* Dane w formie klucz=wartość */
            this->request += this->httpDataKeyName;
            this->request += EQUAL_SIGN;
            this->request += data;

        } else { /* Jeżeli inna metoda niż 'GET' */

            /* Sama ścieżka URL */
            this->request += this->httpPath;

        }

        /* Wersja protokołu http: 'HTTP/1.1' */
        this->request += SPACE;
        this->request += this->httpVersion;
        this->request += NEWLINE;

        /* Opcjonalnie dodaje nagłówek dla przeglądarki, w tym przypadku nazwa komputera i użytkownika*/
        this->request += this->httpUserAgent;
        this->request += NEWLINE;

        /* Dołączam nagłówki z pliku konfiguracyjnego 'http.yaml' */
        for (auto & header : headers) {

            this->request += header.first;
            this->request += ": ";
            this->request += header.second;
            this->request += NEWLINE;

        }

        /* Druga nowa linia kończąca zapytanie 'GET' */
        if (this->httpMethod == "GET") {

            this->request += NEWLINE;

        } else {

            /* Jeżeli metoda zapytania inna niż 'GET', dodaje dodatkowe nagłówki
               i na końcu dane */

            /* Ustawia długość danych w nagłówku */
            this->request += this->httpContentLength;
            this->request += std::to_string((data.length() + this->httpDataKeyName.length() + 1));
            this->request += NEWLINE;
            this->request += NEWLINE;

            /* Jeżeli nazwa klucza nie jest pustym ciągiem,
               dodaje nazwę klucza i znak równości przed danymi,
               dane są w formie 'klucz=wartość' */
            if (!this->httpDataKeyName.empty()) {

                this->request += this->httpDataKeyName;

                this->request += EQUAL_SIGN;
            }

            this->meteoLog->warn(("CLASS::HttpRequestCreator::createRequest(data){ data="+data).c_str());

            /* Dodaje dane na końcu zapytania */
            this->request += data;
        }

#ifdef SHOW_REQUEST

        /* Wyświetla całe zapytanie w trybie testowania */
        this->meteoLog->warn(this->request.c_str());

#endif

    } else {

        this->meteoLog->err("CLASS::HttpRequestCreator::creatRequest()->Błąd! Obsługiwane metody: POST, GET, PUT, PATCH.");

        exit(EXIT_FAILURE);

    }

    return this->request;
}
