/**
 * @author  Created by Marcin Guziołek on 06.04.18.
 */


#include "MyDaemon.h"


MyDaemon::MyDaemon(Task *task, const string &daemonName, const string &ppidDirPath) : task(task),
                                                                                      daemonName(daemonName),
                                                                                      meteoLog(new MyLog(
                                                                                              "CLASS::MyDaemon::" +
                                                                                              daemonName)) {


    /* Tworzę ścieżkę do pliku z PID'em demona */
    this->pidFilePath = ppidDirPath;
    this->pidFilePath += "."; /* Ukrywam plik */
    this->pidFilePath += this->daemonName;
    this->pidFilePath += ".pid";


}

void MyDaemon::init() {

    /* Zacznij główną pętlę z zadaniem */
    this->start();
}


void MyDaemon::start() {

    /* Podstawowe ustawienia */
    this->setEnvironment();

    this->exitOnAnotherInstance();

    this->saveNewPid();

    this->sayHello();

    this->mainLoop();
}


void MyDaemon::setEnvironment() {

    /* Resetuje uprawnienia do plików */
    umask(0);

    /* Tworzę nowy SID (Session ID taki sam jak PID Prosses ID) dla procesu potomnego */
    int rs = setsid();

    /* Zapisuje błąd zwrócony przez 'setsid', żeby go nie utracić */
    int saved_err = errno;

    /* Funkcja 'setsid' zwróciła błąd */
    if (rs == -1) {

        this->meteoLog->err("setEnvironment(){ setsid() = -1 }: ", saved_err);

        exit(EXIT_FAILURE);

    }

    this->setSid(rs);

    /* Zmieniam bieżący katalog na główny katalog */
    rs = chdir("/");

    /* Zapisuje błąd zwrócony przez 'chdir', żeby go nie utracić */
    saved_err = errno;

    if (rs == -1) {

        this->meteoLog->err("setEnvironment(){ chdir(/) = -1 }: ", saved_err);

        exit(EXIT_FAILURE);

    }

    /* Zeruje errno, żeby wykryć czy funkcja 'nice' zwróciła błąd, ponieważ udane wywołanie funkcji,
     * może zwrócić -1 */
    errno = 0;

    /* Ustawiam priorytet na bardzo wysoki -10, najwyższy jest -20, a najniższy +19, domyślnie 0 */
    rs = nice(-10);

    /* Zapisuje błąd zwrócony przez 'nice', żeby go nie utracić */
    saved_err = errno;

    if (rs == -1 && errno != 0) {

        this->meteoLog->err("setEnvironment(){ nice(-10) = -1 }: ", saved_err);

        exit(EXIT_FAILURE);

    }

    /* Zamykam standardowe strumienie */
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

}


void MyDaemon::exitOnAnotherInstance() {

    int saved_err, fd;

    /* Otwieram plik z PID'em do zapisu albo tworzę nowy, gdy nie istnieje */
    fd = open(this->pidFilePath.c_str(), O_CREAT | O_WRONLY, 0600);

    /* Zapisuje błąd zwrócony przez 'open', żeby go nie utracić */
    saved_err = errno;

    /* Funkcja 'open' zwróciła błąd */
    if (fd == -1) {

        this->meteoLog->err("exitOnAnotherInstance(){ open(pidDesc,flags,mode) = -1 }: ", saved_err);

        exit(EXIT_FAILURE);

    }

    this->setPidFileDesc(fd);


    /* Próbuje zablokować plik z PID'em, jeżeli niepowodzenie, demon już istnieje więc kończy działanie */
    fd = lockf(this->getPidFileDesc(), F_TLOCK, 0);

    /* Zapisuje błąd zwrócony przez 'lockf', żeby go nie utracić */
    saved_err = errno;

    /* Funkcja 'lockf' zwróciła błąd */
    if (fd == -1) {

        this->meteoLog->err("exitOnAnotherInstance(){ lockf(pidfDesc,cmd,len) = -1 }: ", saved_err);

        exit(EXIT_FAILURE);

    }
}


void MyDaemon::saveNewPid() {

    /* Usuwam stary PID */
    int fr = ftruncate(this->getPidFileDesc(), 0);

    /* Zapisuje błąd zwrócony przez 'ftruncate', żeby go nie utracić */
    int saved_err = errno;

    /* Funkcja 'ftruncate' zwróciła błąd */
    if (fr == -1) {

        this->meteoLog->err("ExitOnAnotherInstance(){ ftruncate(pipefDesc,len) = -1 }: ", saved_err);

        exit(EXIT_FAILURE);

    }

    auto len = to_string(this->getSid()).length();


    /* Zapisuje PID demona do pliku */
    ssize_t wr = write(this->getPidFileDesc(), to_string(this->getSid()).c_str(), len);

    /* Zapisuje błąd zwrócony przez 'write', żeby go nie utracić */
    saved_err = errno;

    /* Funkcja 'write' zwróciła błąd */
    if (wr == -1) {

        this->meteoLog->err("exitOnAnotherInstance(){ write(pipefDesc,buf,len) = -1 }: ", saved_err);

        exit(EXIT_FAILURE);

    }
}


void MyDaemon::mainLoop() {

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

    /* Główna pętla */
    while (true) {

        /* Wykonuje zadanie  */
        this->task->task();

    }

#pragma clang diagnostic pop

}

void MyDaemon::sayHello() {

    /* Zapisuje w dzienniku systemowym komunikat o rozpoczęciu działania */
    this->meteoLog->notice(("Daemon: " + this->daemonName + " has just started!").c_str());

}


pid_t MyDaemon::getSid() const {
    return this->sid;
}


void MyDaemon::setSid(pid_t psid) {
    this->sid = psid;
}


int MyDaemon::getPidFileDesc() const {
    return this->pidFileDescriptor;
}


void MyDaemon::setPidFileDesc(int ppidFileDescriptor) {
    this->pidFileDescriptor = ppidFileDescriptor;
}


MyDaemon::~MyDaemon() {

    delete this->task;
    delete this->meteoLog;

}

MyDaemon::MyDaemon(MyDaemon &myDaemon) : task(myDaemon.task),
                                         sid(myDaemon.getSid()),
                                         pidFileDescriptor(myDaemon.getPidFileDesc()) {

    this->meteoLog = new MyLog(this->daemonName);
    this->meteoLog = myDaemon.meteoLog;

}


