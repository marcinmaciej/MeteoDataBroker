/**
 * @author  Created by Marcin Guziołek on 06.12.21.
 */


#include "HttpTask.h"


HttpTask::HttpTask(const ConfigManager &configManager, int pipeDesc) : Task(configManager),
                                                                       http(configManager),
                                                                       PIPEDESC(pipeDesc) {

    this->meteoLog = MyLog("CLASS::HttpTask");

}


std::string HttpTask::readPipe() {

    std::string msgTitle = "readPipe()";
    ssize_t bytesRead;
    size_t serialDataLength;

    /* Przechwytuje błąd 'stoi' */
    try {

        serialDataLength = std::stoi(
                this->getConfigManager().getConfig("SerialDataLength", this->getConfigManager().SERIAL));

    } catch (std::exception &e) {

        this->meteoLog.err((msgTitle + "{ std::stoi() } = " + e.what()).c_str());
    }

    auto *dataRead = new char[serialDataLength]();

    /* Odczyt z potoku */
    bytesRead = read(this->PIPEDESC, dataRead, serialDataLength);

    /* Zapisuje błąd, żeby go nie utracić */
    int saved_err = errno;

    std::string result(dataRead);

    delete[] dataRead;

    /* Funkcja 'read' zwróciła błąd */
    if (bytesRead == -1) {

        this->meteoLog.err((msgTitle + "{ read(pipeDesc,buf,len) = -1 }: ").c_str(), saved_err);

        std::exit(EXIT_FAILURE);

        /* Funkcja 'read' podczas czytania z pustego bufora potoku blokuje zasób i czeka na dane,
           jednak gdy zwraca 0 (EOF), oznacza to, że wszystkie uchwyty zapisu do potoku zostały zamknięte,
           więc restartuje demony w celu utworzenia nowego potoku */
    } else if (bytesRead == 0) {

        this->meteoLog.err((msgTitle + "{ read(pipeDesc,buf,len) = 0 }: ").c_str(), saved_err);

        /* Restartuje demony */
        restartOnBrokenPipe(this->getConfigManager(), this->meteoLog, 1, msgTitle);

    }

    return result;
}


void HttpTask::task() {

    if (!http.isConnected()) {

        http.connectServer();

    } else {

        std::string data = this->readPipe();

        if (!data.empty()) {
            this->http.sendData(data);
        }

        /* Przechwytuje błąd 'std::stoi' */
        try {

            /* Usypia proces na zadaną liczbę sekund */
            sleep(std::stoi(this->getConfigManager().getConfig("SocketSleepTime", this->getConfigManager().SOCKET)));

        } catch (std::exception &e) {

            this->meteoLog.err("task(){ sleep(stoi()) } = ");
        }
    }
}

const ConfigManager &HttpTask::getConfigManager() const {
    return this->configManager;
}

HttpTask::~HttpTask() {

    close(this->PIPEDESC);

}

