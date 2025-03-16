/**
 * @author  Created by Marcin Guziołek on 16.12.21.
 */

#include <iostream> /* std::cout, std::endl */
#include <cerrno>   /* errno */
#include <fcntl.h> /* open(), O_RDONLY */
#include <unistd.h> /* execv(), lockf(), F_ULOCK */
#include <cstring> /* strlen(), strcmp(), strcpy() */
#include <algorithm> /* transform() */


#include "ConfigManager.h"
#include "DaemonsManager.h"

/* Domyślna ścieżka do pliku konfiguracyjnego ze ścieżkami do pozostałych plików konfiguracyjnych */
#define CONFIGPATHS "/etc/meteo-station/config/paths.yaml"

std::string selfPath;


int main(int argc, char *argv[]) {

    selfPath = argv[0];

    auto *configManager = new ConfigManager(CONFIGPATHS);

    configManager->loadAllConfigFiles();

    auto *daemonsManager = new DaemonsManager(*configManager);

    /* Jest podany argument */
    if (argc == 2) {

        const char *arg = argv[1];

        if (strcmp(arg, "-k") == 0 || strcmp(arg, "--kill") == 0) {

            std::cout << "Killing daemons." << std::endl;
            daemonsManager->killAll();

        } else if (strcmp(arg, "-r") == 0 || strcmp(arg, "--restart") == 0) {

            std::cout << "Restarting daemons." << std::endl;
            daemonsManager->restartDaemons();

        } else if (strcmp(arg, "-s") == 0 || strcmp(arg, "--start") == 0) {

            std::cout << "Starting daemons!" << std::endl;
            daemonsManager->init();
        }

    } else {

        std::cout << "Starting daemons!" << std::endl;

        daemonsManager->init();
    }

    delete configManager;
    delete daemonsManager;

    return EXIT_SUCCESS;

}

/* Funkcja pomocnicza. Zmienia wszystkie znaki w ciągu na małe */

std::string toLowerCase(const std::string &pstr, MyLog &meteoLog) {

    std::string temp = pstr;

    try {

        std::transform(temp.begin(), temp.end(), temp.begin(), ::tolower);

    } catch (std::exception &e) {

        meteoLog.err(("toLowerCase::transform(tolower) -> " + std::string(e.what())).c_str());

    }

    return temp;

}

/* Funkcja restartuje demony, gdy którykolwiek z nich przestanie działać */
void
restartOnBrokenPipe(const ConfigManager &configManager, MyLog &meteoLog, const int whichDaemon, std::string &msgTitle) {

    int saved_err;

    /* Parametr dla nowego wywołania programu */
    const char *restartArg = "--start";

    /* Ustawia rozmiar tablic znaków */
    char self[strlen(selfPath.c_str())],
            arg[strlen(restartArg) + 1];

    /* Kopiuje do tablicy znaków 'self' ścieżkę do programu */
    strcpy(self, selfPath.c_str());

    /* Kopiuje do tablicy znaków 'arg' argument programu */
    strcpy(arg, restartArg);

    /* Ustawia tablicę argumentów dla ponownego wywołania programu,
       z tą samą nazwą programu i parametr restartującym program,
       lista argumentów musi się kończyć NULL-em */
    char *newArgv[] = {self, arg, nullptr};


    std::string lockPath = configManager.getConfig("pidFileDirPath", configManager.APP);

    /*Tworzy ścieżkę do pliku z PID-em bieżącego procesu */
    if (whichDaemon == 0) { /* rs232-demon */

        lockPath += ".rs232d.pid";

    } else if (whichDaemon == 1) { /* http-demom */

        lockPath += ".httpd.pid";

    }

    /* Tworzy uchwyt do pliku z PID-em bieżącego procesu */
    int fd = open(lockPath.c_str(), O_RDONLY, 0600);

    saved_err = errno;

    if (fd == -1) {

        meteoLog.err("Restarting daemons::open(lockPath, o_rdonly, 0600) = -1:", saved_err);

        std::exit(EXIT_FAILURE);
    }

    /* Zwalnia 'lock' z pliku z PID-em bieżącego procesu */
    if (lockf(fd, F_ULOCK, 0) == -1) {

        /* Zapisuje błąd zwrócony przez 'lockf', żeby go nie utracić */
        saved_err = errno;

        meteoLog.err("Restarting daemons::lockf(fd,f_ulock,0) = -1: ", saved_err);

        std::exit(EXIT_FAILURE);

    }

    std::string msg = "restartOnBrokenPipe() -> " + lockPath + "::execv(execPath,newArgv) = -1: ";

    /* Restartuje program */
    if (execv(configManager.getConfig("execPath", configManager.APP).c_str(), newArgv) == -1) {

        /* Zapisuje błąd zwrócony przez 'execv', żeby go nie utracić */
        saved_err = errno;

        /* Zapisuje komunikat o błędzie w dzienniku systemowym */
        meteoLog.err(msg.c_str(), saved_err);

        std::exit(EXIT_FAILURE);
    }

}

