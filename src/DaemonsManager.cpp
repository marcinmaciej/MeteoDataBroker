/**
 * @author  Created by Marcin Guziołek on 06.04.18.
 */

#include "DaemonsManager.h"


#define TESTPIPEDESC true

DaemonsManager::DaemonsManager(const ConfigManager &configManager) : configManager(configManager),
                                                                     daemonsPids(new pid_t[2]{0}),
                                                                     pidFileDirPath(new std::string(
                                                                             configManager.getConfig("pidFileDirPath",
                                                                                                     configManager.APP))),
                                                                     meteoLog(new MyLog("CLASS::DaemonsManager")) {}


void DaemonsManager::init() {

    /* Tablica do przechowywania uchwytów potoku */
    int pipeFd[2];


    /* Tworzy potok dla danych z portu szeregowego,
     * przesyłanych do demona obsługującego wysyłanie danych na serwer
     * przez protokół http.
     */
    int pr = pipe(pipeFd);

    /* Zapisuje błąd, żeby go nie utracić */
    int saved_err = errno;

    if (pr == -1) {

        this->meteoLog->err("init(){ pipe(pipefd) = -1 }: ", saved_err);

        std::exit(EXIT_FAILURE);
    }

#if TESTPIPEDESC

    std::string temp = "Write PIPE descriptor: " + std::to_string(pipeFd[this->RS232D]) +
                        "\r\nRead PIPE descriptor: " + std::to_string(pipeFd[this->HTTPD]);

    this->meteoLog->warn(temp.c_str());

#endif

    if (fork()) {/* Rozwidla proces rodzica */

        /* Zamyka skopiowany uchwyt zapisu do potoku */
        close(pipeFd[this->RS232D]);

        if (fork()) {/* Rozwidla proces rodzica jeszcze raz */

            /* Zamyka uchwyty potoku utworzone w procesie rodzica */
            close(pipeFd[this->HTTPD]);
            close(pipeFd[this->RS232D]);

            /* Proces rodzica nic nie robi więc zostanie zamknięty */

        } else {

            /* Zamyka odziedziczony uchwyt zapisu do potoku */
            close(pipeFd[this->RS232D]);

            if (int httpPid = fork()) {

                /* Zamyka odziedziczone uchwyty potoku */
                close(pipeFd[this->HTTPD]);
                close(pipeFd[this->RS232D]);

                /* Proces rodzica nic nie robi więc zostanie zamknięty */

            } else {/* Uruchamia proces demona o nazwie 'httpd', obsługującego wysyłanie danych na serwer
                     *  przez protokół http */

                /* Zmienia odziedziczoną nazwę procesu rodzica na unikalną nazwę demona */
                prctl(PR_SET_NAME, this->daemonsNames[this->HTTPD].c_str());

                /* Tworzy i uruchamia demona */
                (new MyDaemon(new HttpTask(this->getConfigManager(), pipeFd[this->HTTPD]),
                              this->daemonsNames[this->HTTPD],
                              *this->getPidFileDirPath()))->init();


                /* Zamyka uchwyt zapisu do potoku */
                close(pipeFd[!this->RS232D]);

                /* Ustawia PID demona */
                this->setPid(this->HTTPD, httpPid);

            }
        }

    } else {

        /* Zamyka uchwyt odczytu z potoku */
        close(pipeFd[this->HTTPD]);


        if (int serialPid = fork()) {/* Rozwidla ponownie proces */

            /* Zamyka oba uchwyty do potoku */
            close(pipeFd[this->HTTPD]);
            close(pipeFd[this->RS232D]);

            /* Proces rodzica nic nie robi więc zostanie zamknięty */

        } else {/* Uruchamia proces demona o nazwie 'rs232d', obsługującego pobieranie danych z portu szeregowego
                 *  przez protokół rs232 */

            /* Zmienia odziedziczoną nazwę procesu rodzica na unikalną nazwę demona */
            prctl(PR_SET_NAME, this->daemonsNames[this->RS232D].c_str());

            /* Tworzy i uruchamia demona */
            (new MyDaemon(new SerialTask(this->getConfigManager(), pipeFd[this->RS232D]),
                          this->daemonsNames[this->RS232D],
                          *this->getPidFileDirPath()))->init();


            /* Zamyka uchwyt do odczytu z potoku */
            close(pipeFd[!this->HTTPD]);


            /* Ustawia PID demona */
            this->setPid(this->RS232D, serialPid);

        }
    }
}

void DaemonsManager::killAll() {

    /* Odczytuje z pliku pid demona obsługującego połączenie http */
    this->getSavedPid(this->HTTPD);


    /* Odczytuje z pliku pid demona obsługującego połączenie szeregowe ze stacją meteo */
    this->getSavedPid(this->RS232D);

    /* Sprawdza, czy dany pid należy do aktywnego procesu, jeśli tak zakańcza proces */
    if (this->isRunning(this->getPid(this->HTTPD))) {

        this->killDaemon(this->getPid(this->HTTPD));
    }

    /* Sprawdza, czy dany pid należy do aktywnego procesu, jeśli tak zakańcza proces */
    if (this->isRunning(this->getPid(this->RS232D))) {

        this->killDaemon(this->getPid(this->RS232D));
    }
}

void DaemonsManager::restartDaemons() {

    this->killAll();
    this->init();
}

void DaemonsManager::getSavedPid(unsigned char daemonNameIndex) {

    /* Tworzy nazwę pliku z PID-em demona */
    std::string pidFileName = "." /* Plik jest ukryty */
                            + this->daemonsNames[daemonNameIndex] /* Wybiera z tablicy nazwę demona */
                            + ".pid"; /* Dodaje rozszerzenie pliku */

    /* Tworzy ścieżkę do pliku z PID-em demona */
    std::string pidFilePath = *this->getPidFileDirPath() + pidFileName;


    /* Open pid file to read */
    int fd = open(pidFilePath.c_str(), O_RDONLY);

    /* Zapisuje błąd, żeby go nie utracić */
    int saved_err = errno;

    /* Funkcja open zwraca błąd */
    if (fd == -1) {

        this->meteoLog->err("getSavedPid(daemonNameIndex){ open(pid-file,read) = -1 }: ", saved_err);

        std::exit(EXIT_FAILURE);

    }

    /* size_t == unsigned long */
    const size_t LEN = 6;

    char buf[LEN];

    ssize_t rr = read(fd, buf, LEN);

    /* Zapisuje błąd, żeby go nie utracić */
    saved_err = errno;

    /* Funkcja 'read' zwraca błąd */
    if (rr == -1) {

        this->meteoLog->err("getSavedPid(daemonNameIndex){ read(pid-file,buf,length) = -1 }:", saved_err);

        std::exit(EXIT_FAILURE);

    }

    /* Przechwytuje błąd 'std::stoi', dodaje opis i zapisuje do dziennika systemowego */
    try {

        /* Zapisuje odczytany z pliku PID demona */
        *(this->daemonsPids + daemonNameIndex) = std::stoi(buf);

    } catch (std::exception &e) {

        this->meteoLog->err((std::string("getSavedPid(daemonNameIndex){ std::stoi(buf) } = ") + e.what()).c_str());
    }

    close(fd);


}

void DaemonsManager::killDaemon(const pid_t *daemonPid) {

    if (kill(*daemonPid, SIGTERM) == -1) {

        /* Zapisuje błąd, żeby go nie utracić */
        int saved_err = errno;

        /* Brak uprawnień */
        if (saved_err == EPERM) {

            this->meteoLog->err("killDaemon(daemonPid){ kill(daemonPid,SIGTERM) = -1 EPERM }: ",
                                saved_err);

            /* Nie ma takiego procesu */
        } else if (saved_err == ESRCH) {

            this->meteoLog->err("killDaemon(daemonPid){ kill(daemonPid,SIGTERM) = -1 ESRCH }: ",
                                saved_err);

            /* Nieprawidłowy sygnał */
        } else if (saved_err == EINVAL) {

            this->meteoLog->err("killDaemon(daemonPid){ kill(daemonPid,SIGTERM) = -1 EINVAL }: ",
                                saved_err);
        }

        std::exit(EXIT_FAILURE);

    } else {

        /* Zapisuje w dzienniku systemowym komunikat o zakończeniu pracy procesu */
        this->meteoLog->notice(("Daemon terminated! PID: " + std::to_string(*daemonPid)).c_str());

    }

}

bool DaemonsManager::isRunning(const pid_t *daemonPid) {

    /* Sprawdza, czy demon z danym PID-em istnieje */
    int kr = kill(*daemonPid, 0);

    /* Zapisuje błąd, żeby go nie utracić */
    int saved_err = errno;

    if (kr == -1) {

        if (saved_err == ESRCH) {

            this->meteoLog->warn("isRunning(daemonPid){ kill(daemonPid,0) = -1 ESRCH }: ", saved_err);

        } else {

            this->meteoLog->err("isRunning(daemonPid){ kill(daemonPid,0) = -1 EINVAL||EPERM }: ", saved_err);
        }

        return false;
    }

    return true;
}

const ConfigManager &DaemonsManager::getConfigManager() const {

    return this->configManager;
}

const std::string *DaemonsManager::getPidFileDirPath() const {

    return this->pidFileDirPath;
}

pid_t *DaemonsManager::getPid(unsigned char daemonPidIndex) const {

    return (this->daemonsPids + daemonPidIndex);
}

void DaemonsManager::setPid(unsigned char daemonPidIndex, pid_t pid) {

    *(this->daemonsPids + daemonPidIndex) = pid;
}


/* Destruktor */

DaemonsManager::~DaemonsManager() {

    delete[] this->daemonsPids;
    delete this->meteoLog;
    delete pidFileDirPath;
}


/* Konstruktor kopiujący */

DaemonsManager::DaemonsManager(DaemonsManager &daemonsManager) : configManager(daemonsManager.getConfigManager()) {

    this->pidFileDirPath = new std::string;
    this->pidFileDirPath = daemonsManager.pidFileDirPath;

    this->daemonsPids = new pid_t[2];
    this->daemonsPids[0] = daemonsManager.daemonsPids[0];
    this->daemonsPids[1] = daemonsManager.daemonsPids[1];

    this->meteoLog = new MyLog("MS::DaemonsManager");
    this->meteoLog = daemonsManager.meteoLog;
}
