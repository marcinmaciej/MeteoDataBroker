/**
 * @author  Created by Marcin Guziołek on 14.12.21.
 */

#include "DataManager.h"


DataManager::DataManager(std::string dataFilePath) : dataFilePath(std::move(dataFilePath)) {

    this->meteoLog = MyLog("CLASS::DataManager::");
}

void DataManager::saveData(const std::string &data) {

    ssize_t numWritten;

    size_t numToWrite;

    numToWrite = data.length();

// TESTING BLOCK
#if SHOW_BUFFER
    string ms = "saveData(bytes to write) = ";
    ms += to_string(numToWrite);
    this->meteoLog.err(ms.c_str());
    ms = "Content to write = ";
    ms += data;
    ms.clear();
#endif

    /* Otwiera plik do archiwizacji danych pobranych przez serial port */
    this->openDataFile();


    /* Zapisuje dane do pliku o nazwie odpowiadającej bieżącej dacie */
    numWritten = write(this->getDataFileDesc(), data.c_str(), numToWrite);


    /* Zapisuje błąd zwracany przez 'write', żeby go nie utracić */
    int saved_err = errno;

    if (numWritten == -1) {

        /* Zapisuje w dzienniku systemowym komunikat o błędzie */
        this->meteoLog.err("saveData(data){ write(data-file,buf,length) }: ", saved_err);

    }


    close(this->getDataFileDesc());


// TESTING BLOCK
#if SHOW_BUFFER
    ms = ("saveData(bytes written) = ");
    ms += to_string(numWritten);
    this->meteoLog.err(ms.c_str());
    ms = "Content written = ";
    auto mms = new string(data);
    this->meteoLog.warn(mms->c_str());
    ms.clear();
#endif

}

void DataManager::openDataFile() {

    /* Bieżąca data i czas wyrażona w liczbie sekund mierzonych od daty 1970-01-01 00:00:00 +0000 (UTC) */
    const time_t time_T = time(nullptr);

    /* Tworzy ciąg znaków reprezentujący lokalnie bieżącą datę i czas w formacie 'Mon Wee  d hh:mm:ss yyyy'
     * gdzie:
     * Mon = trzy pierwsze litery miesiąca,
     * Wee = trzy pierwsze litery dnia tygodnia
     * d = numer dnia miesiąca
     * hh = godzina
     * mm = minuty
     * ss = sekundy
     * yyyy = czterocyfrowy rok
     * ciąg jest zakończony znakiem nowej lini.
     */
    std::string currentDate(ctime(&time_T));

    /* Pobiera ścieżkę do katalogu na pliki z danymi */
    std::string path = this->getDataFilePath();

    /* Przechwytuje błąd 'replace()' i 'erase()' */
    try {

        /* Usuwa w bieżącej dacie godzinę poprzez zmianę 8 znaków godziny (format hh:mm:ss) i dwóch spacji
         * na jedną spację, zaczynając od długości ciągu znaków minus 15, zostawia 4 znaki roku */
        currentDate.replace((currentDate.length() - 15), 10, " ");

        /* Usuwa znak nowej lini */
        currentDate = currentDate.erase(currentDate.length()-1);

    } catch (std::exception &e) {

        this->meteoLog.err(("openDataFile(){ replace(); erase(); } = "+ std::string(e.what())).c_str());
    }

    /* Dodaje do ścieżki nazwę pliku */
    path += currentDate;

    int saved_err,
            fd,

            /* Sprawdza, czy plik istnieje */
            ar = access(path.c_str(), F_OK);

    /* Plik o tej nazwie istnieje */
    if (ar == 0) {

        /* Otwiera istniejący plik do zapisu */
        fd = open(path.c_str(), O_WRONLY | O_APPEND, 0744);

        /* Zapisuje błąd zwracany przez 'open', żeby go nie utracić */
        saved_err = errno;

        if (fd == -1) {

            /* Zapisuje w dzienniku systemowym komunikat o błędzie zwróconym przez 'open' */
            this->meteoLog.err("openDataFile(){ open(data-file,O_WRONLY,0744) = -1 }: ", saved_err);

            std::exit(EXIT_FAILURE);
        }

    } else { /* Nie ma pliku o tej nazwie, zaczęła się nowa doba */

        /* Tworzy plik do zapisu o nowej nazwie odpowiadającej bieżącej dacie */
        fd = open(path.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0744);

        /* zapisuje błąd, żeby go nie utracić */
        saved_err = errno;

        /* funkcja 'open' zwróciła błąd */
        if (fd == -1) {

            /* Zapisuje w dzienniku systemowym 'syslog' komunikat o błędzie zwróconym przez 'open' */
            this->meteoLog.err("openDataFile(){ open(data-file,O_WRONLY,0744) = -1 }: ", saved_err);

            std::exit(EXIT_FAILURE);
        }

    }

    /* Ustawia uchwyt do pliku na dane */
    this->setDataFileDesc(fd);

}

int DataManager::getDataFileDesc() const {

    return this->dataFileDesc;
}

std::string DataManager::getDataFilePath() const {

    return this->dataFilePath;
}

void DataManager::setDataFileDesc(int pfd) {

    this->dataFileDesc = pfd;
}
