/**
 * @author  Created by Marcin Guziołek on 10.12.21.
 */


#include "SerialTask.h"

#define TESTING_SERIAL false
#define TESTING_PIPE false
#define TESTING_SERIAL_WITH_MSSIMULATOR true


SerialTask::SerialTask(const ConfigManager &configManager, int pipeDesc) : Task(configManager),
                                                                           PIPEDESC(pipeDesc),
                                                                           bufferManager(new DataManager(
                                                                                   this->getConfigManager().getConfig(
                                                                                           "dataFilePath",
                                                                                           this->getConfigManager().APP))) {

    this->meteoLog = MyLog("CLASS::SerialTask");

    this->setSerialPortPath(this->getConfigManager().getConfig("SerialPort", this->getConfigManager().SERIAL));

    std::string msg;

    try {

        this->setBytesNumToRead(
                std::stoi(this->getConfigManager().getConfig("SerialDataLength", this->getConfigManager().SERIAL)));

    } catch (std::exception &e) {

        this->meteoLog.err(("Constructor::setBytesNumToRead(std::stoi) = " + std::string(e.what())).c_str());
    }

    /* Przechwytuje błąd contenera stl map 'mapDataBits' */
    try {

        this->mapDataBits.insert(pair<const std::string, int>("5", CS5));
        this->mapDataBits.insert(pair<const std::string, int>("6", CS6));
        this->mapDataBits.insert(pair<const std::string, int>("7", CS7));
        this->mapDataBits.insert(pair<const std::string, int>("8", CS8));

    } catch (std::exception &e) {

        this->meteoLog.err(
                ("Constructor::mapDataBits.insert(pair<const std::string,int>) = " + std::string(e.what())).c_str());

    }

    /* Przechwytuje błąd contenera stl map 'mapSpeed' */
    try {

        this->mapSpeed.insert(pair<const std::string, speed_t>("0", B0)); /* zawieszenie */
        this->mapSpeed.insert(pair<const std::string, speed_t>("50", B50));
        this->mapSpeed.insert(pair<const std::string, speed_t>("75", B75));
        this->mapSpeed.insert(pair<const std::string, speed_t>("110", B110));
        this->mapSpeed.insert(pair<const std::string, speed_t>("134", B134));
        this->mapSpeed.insert(pair<const std::string, speed_t>("150", B150));
        this->mapSpeed.insert(pair<const std::string, speed_t>("200", B200));
        this->mapSpeed.insert(pair<const std::string, speed_t>("300", B300));
        this->mapSpeed.insert(pair<const std::string, speed_t>("600", B600));
        this->mapSpeed.insert(pair<const std::string, speed_t>("1200", B1200));
        this->mapSpeed.insert(pair<const std::string, speed_t>("1800", B1800));
        this->mapSpeed.insert(pair<const std::string, speed_t>("2400", B2400));
        this->mapSpeed.insert(pair<const std::string, speed_t>("4800", B4800));
        this->mapSpeed.insert(pair<const std::string, speed_t>("9600", B9600));
        this->mapSpeed.insert(pair<const std::string, speed_t>("19200", B19200));
        this->mapSpeed.insert(pair<const std::string, speed_t>("38400", B38400));
        this->mapSpeed.insert(pair<const std::string, speed_t>("57600", B57600));
        this->mapSpeed.insert(pair<const std::string, speed_t>("115200", B115200));

    } catch (std::exception &e) {

        this->meteoLog.err(
                ("Constructor::mapSpeed.insert(pair<const std::string,speed_t>) = " + std::string(e.what())).c_str());

    }

}

/* Zadanie wykonywane przez demona */
void SerialTask::task() {

    /* Otwiera port szeregowy (plik urządzenia) */
    this->openSerialPort();

    /* Ustawia konfigurację portu szeregowego */
    this->setSerialPortConfig();

    /* Czyta z bufora portu szeregowego */
    std::string *readData = this->readSerialPort();

    if (!readData[0].empty() && !readData[1].empty()) {

        /* Archiwizuje lokalnie w pliku dane z portu szeregowego
         * dodając do danych znak nowej lini */
        this->bufferManager->saveData((readData[0] + readData[1] + "\r\n"));

        /* Zapisuje dane z portu szeregowego do potoku */
        this->writePipe(readData[1]);


    }

    /* Przechwytuje błąd 'std::stoi' */
    try {

        /* Usypia proces na zadaną liczbę sekund */
        sleep(std::stoi(this->getConfigManager().getConfig("SerialSleepTime", this->getConfigManager().SERIAL)));

    } catch (std::exception &e) {

        this->meteoLog.err(("task(){ sleep(std::stoi()) } = " + std::string(e.what())).c_str());
    }

}

/* Otwiera urządzenie (plik urządzenia) do odczytu */
void SerialTask::openSerialPort() {

    std::string message,
            msgTitle = "openSerialPort()";

    int fd;

    /* Flaga 'O_NONBLOCK' tylko podczas testowania z quasi symulatorem stacji meteo 'mssimulator' */
    fd = open(this->getSerialPortPath().c_str(), O_RDONLY | O_NOCTTY

                                                 #if TESTING_SERIAL_WITH_MSSIMULATOR

                                                 | O_NONBLOCK

#endif

    );

    /* Zapisuje błąd zwracany przez 'open', żeby go nie utracić */
    int saved_errno = errno;

    if (fd == -1) {

        /* Zadanie nie wykonane */
        this->isExecuted = false;

        /*
           Obsługuje trzy błędy 'ENOENT', 'ENXIO' i 'ENODEV',
           w ich przypadku nie przerywa działania programu.
           ENOENT — plik lub folder nie istnieje,
           ENXIO — nie ma takiego urządzenia (pliku urządzenia) lub adresu,
           ENODEV — nie ma takiego urządzenia (pliku urządzenia).
        */
        if (saved_errno == ENOENT || saved_errno == ENXIO || saved_errno == ENODEV) {

            /* Zapisuje komunikat o błędzie w dzienniku systemowym oraz informację
               o oczekiwaniu na urządzenie zadaną liczbę sekund */
            messageAndSleep(saved_errno, msgTitle);

        } else { /* Pozostałe błędy */

            /* Zapisuje komunikat o innym błędzie w dzienniku systemowym */
            this->meteoLog.err((msgTitle + "{ open(fd,mode) = INNY BŁĄD }: ").c_str(), saved_errno);

            /* Kończy działanie programu ze statusem niepowodzenie */
            std::exit(EXIT_FAILURE);

        }

    } else {

        /* Urządzenie otwarte do odczytu.
         * Ustawia wskaźnik do urządzenia (pliku urządzenia) */
        this->setSerialFileDesc(fd);

        /* Wyświetla komunikat tylko przy pierwszym udanym działaniu */
        if (!this->isExecuted) {

            /* Zapisuje w dzienniku systemowym komunikat o udanym otwarciu urządzenia (pliku urządzenia) */
            this->meteoLog.notice(("Source opened: " + this->getSerialPortPath()).c_str());

            /* Zadanie wykonane nie wyświetlaj już komunikatu o otwartym urządzeniu */
            this->isExecuted = true;
        }
    }
}


/* Odczytuje dane z bufora urządzenia */
std::string *SerialTask::readSerialPort() {

    std::string *result =  new string[2],
            msgTitle = "readSerialPort()";

    ssize_t bytesRead;

    size_t len = this->getBytesNumToRead();


    char *buf = new char[len + 1];


#if TESTING_SERIAL

    this->meteoLog.notice("TESTING_SERIAL::Zadana liczba bajtów do odczytu: ", (ssize_t *) &len);

    int bytesAvailable = 0;

    /* Odczytuje ilość bajtów dostępnych w buforze odczytu urządzenia */
    ioctl(this->getSerialFileDesc(), FIONREAD, &bytesAvailable);

    /* Zapisuje komunikat w dzienniku systemowym o ilości dostępnych bajtów do odczytu */
    this->meteoLog.warn(("TESTING_SERIAL::Bytes available = "+ to_string(bytesAvailable)).c_str());

#endif // TESTING_SERIAL


    /* Zapisuje czas jako liczbę sekund */
    time_t timeInSeconds = time(nullptr);

    bytesRead = read(this->getSerialFileDesc(), buf, len);

    /* Zapisuje błąd zwrócony przez 'read', żeby go nie utracić */
    int saved_err = errno;


    /* Funkcja 'read' zwróciła błąd */
    if (bytesRead == -1) {

        /* Zapisuje komunikat o błędzie do dziennika systemowego */
        this->meteoLog.err((msgTitle + "{ read(fd,buf,len) }: ").c_str(), saved_err);

        exit(EXIT_FAILURE);
    }

    /* Dodaje znak (null) końca ciągu znaków */
    *(buf + bytesRead) = '\0';

#if TESTING_SERIAL

    /* Zapisuje w dzienniku systemowym ilość odczytanych bajtów */
    std::string temp = "TESTING_SERIAL::Odczytano z urządzenia: ";
    temp += to_string(bytesRead);
    temp += " bajtów.";

    this->meteoLog.notice(temp.c_str());


    /* Zapisuje w dzienniku systemowym dane odczytane z buforu urządzenia */
    ms.clear();
    ms = "TESTING_SERIAL::Content read from port = ";
    ms += buf;

    this->meteoLog.warn(ms.c_str());

#endif // TESTING_SERIAL



    /* Zwraca bieżącą datę i czas ze znakiem nowej lini na końcu */
    result[0] = ctime(&timeInSeconds);

    /* Zamienia znak nowej lini na końcu daty na spację */
    result[0].back() = ' ';

    /* Dodaje dane pobrane ze stacji meteo */
    result[1] += buf;

    delete[] buf;

    return result;

}


/* Zapisuje dane odczytane z urządzenia do potoku */
void SerialTask::writePipe(const std::string &serialData) {

#if TESTING_PIPE

    std::string ss = "TESTING_PIPE::The write PIPE descriptor--> ";
    ss += to_string(this->PIPEDESC);
    this->meteoLog.notice(ss.c_str());

#endif

    std::string msgTitle = "writePipe(data)";

    int saved_err;

    ssize_t rs;

    sigset_t sigpipe_mask;
    sigemptyset(&sigpipe_mask);
    sigaddset(&sigpipe_mask, SIGPIPE);

    /* Maskuje sygnał 'SIGPIPE' jako blokowany */
    rs = sigprocmask(SIG_SETMASK, &sigpipe_mask, nullptr);

    /* Zapisuje błąd zwrócony przez 'sigprocmask', żeby go nie utracić */
    saved_err = errno;

    /* Funkcja 'sigprocmask' zwróciła błąd */
    if (rs == -1) {

        saved_err = errno;

        this->meteoLog.err((msgTitle + "{ sigprocmask(how,set,oldset) = -1 }: ").c_str(), saved_err);

        exit(EXIT_FAILURE);

    }

    /* Zapisuje dane do potoku, z którego odczytuje demon wysyłający dane na serwer */
    rs = write(this->PIPEDESC, serialData.c_str(), serialData.length());

    /* Zapisuje błąd zwracany przez 'write', żeby go nie utracić */
    saved_err = errno;

    if (rs == -1) {

        /* Przy błędzie 'EPIPE', przerwany potok, wszystkie uchwyty odczytu zamknięte,
           demon 'httpd' prawdopodobnie nie działa lub utracił dostęp do potoku,
           restartuje demony w celu utworzenia nowego potoku */
        if (saved_err == EPIPE) {

            this->meteoLog.err((msgTitle + "{ write(fd,data,len) = EPIPE }: ").c_str(), saved_err);

            /* Restartuje demony */
            restartOnBrokenPipe(this->getConfigManager(), this->meteoLog, 0, msgTitle);

        } else {

            /* Zamyka zapis do potoku */
            close(this->PIPEDESC);

            /* Zapisuje do dziennika systemowego domyślny komunikat o błędzie inny niż 'EPIPE' */
            this->meteoLog.err((msgTitle + "{ write(fd,data,len)} = INNY BŁĄD --> ").c_str(), saved_err);

            exit(EXIT_FAILURE);
        }
    }

#if TESTING_PIPE

    ss = "TESTING_PIPE::PIPE data written length--> ";
    ss += to_string(rs);
    this->meteoLog.warn(ss.c_str());

#endif
}


void SerialTask::messageAndSleep(int saved_errno, std::string &msgTitle) {

    /* Zapisuje w dzienniku systemowym domyślny tekst błędu */
    this->meteoLog.err((msgTitle + "{ write(pipeDesc,buf,len) = -1 }: ").c_str(), saved_errno);

    /* Dodatkowo zapisuje komunikat o uśpieniu procesu na zadaną liczbę sekund */
    std::string message = "Can't open the source: "
                          + this->getSerialPortPath()
                          + ". Waiting "
                          + this->getConfigManager().getConfig("SerialWaitPort", this->getConfigManager().SERIAL)
                          + " seconds.";

    this->meteoLog.info(message.c_str());

    try {
        /* Usypia proces na zadaną liczbę sekund w oczekiwaniu na podłączenie urządzenia */
        sleep(std::stoi(this->getConfigManager().getConfig("SerialWaitPort", this->getConfigManager().SERIAL)));

    } catch (std::exception &e) {

        std::string msg = "serial task messageAndSleep sleep stoi -> " + std::string(e.what());
        this->meteoLog.err(msg.c_str());
    }

}


void SerialTask::closeSerialPort() const {

    close(this->serialFileDescriptor);
}

int SerialTask::getSerialFileDesc() const {

    return this->serialFileDescriptor;
}

void SerialTask::setSerialFileDesc(int fd) {

    this->serialFileDescriptor = fd;
}

void SerialTask::setSerialPortConfig() {

    std::string msgTitle = "setSerialPortConfig()";

    /* Pobiera aktualne parametry urządzenia  */
    int rs = tcgetattr(this->getSerialFileDesc(), &this->options);

    /* Jeżeli funkcja 'tcgetattr' zwróciła błąd, zapisuje go, żeby nie utracić */
    int saved_err = errno;

    /* Funkcja 'tcgetattr' zwróciła błąd */
    if (rs == -1) {

        /* Zapisuje komunikat o błędzie do systemowego dziennika  */
        this->meteoLog.err((msgTitle + "{ tcgetattr(serialDesc,options) = -1 }: ").c_str(),
                           saved_err);

        exit(EXIT_FAILURE);
    }

    /* Ustawia prędkość pracy urządzenia */
    this->setSerialPortSpeed();

    /* Ustawia bity stopu */
    this->setSerialPortStopBits();

    /* Modyfikuje flagi bitów kontrolnych */
    this->setSerialPortParity();

    /* Modyfikuje flagi synchronizacji przesyłu danych */
    this->setSerialPortFlowControl();

    /* Ustawia ilość bitów na znak */
    this->setSerialPortDataBits();

    this->setSerialPortRawInput();

    this->setSerialPortNoMappingChars();

    this->setSerialPortReadMin();


    /* Ustawia parametry urządzenia, zmiany powinny nastąpić natychmiast 'TCSANOW' */
    rs = tcsetattr(this->getSerialFileDesc(), TCSANOW, &this->options);

    /* Jeżeli funkcja 'tcsetattr' zwróciła błąd, zapisuje go, żeby nie utracić */
    saved_err = errno;

    /* Funkcja 'tcsetattr' zwróciła błąd */
    if (rs == -1) {

        /* Zapisuje komunikat o błędzie do systemowego dziennika  */
        this->meteoLog.err((msgTitle + "{ tcsetattr(serialDesc,opt_act,options) = -1 }: ").c_str(),
                           saved_err);

        exit(EXIT_FAILURE);
    }


    /* Ustawia flagę dla pliku urządzenia na nie blokujący */
    rs = fcntl(this->getSerialFileDesc(), F_SETFL, FNDELAY);

    /* Jeżeli funkcja 'tcsetattr' zwróciła błąd, zapisuje go, żeby nie utracić */
    saved_err = errno;

    /* Funkcja 'tcsetattr' zwróciła błąd */
    if (rs == -1) {

        /* Zapisuje komunikat o błędzie do systemowego dziennika  */
        this->meteoLog.err((msgTitle + "{ fcntl(serialDesc,F_SETFL, FNDELAY) = -1 }: ").c_str(),
                           saved_err);

        std::exit(EXIT_FAILURE);
    }

}

void SerialTask::setSerialPortSpeed() {

    /* Ustawia baud rate */
    int rs = cfsetispeed(&this->options,
                         this->mapSpeed.find(this->getConfigManager().getConfig("SerialSpeed",
                                                                                this->getConfigManager().SERIAL))->second);

    /* Jeżeli funkcja 'cfsetispeed' zwróciła błąd, zapisuje go, żeby nie utracić */
    int saved_err = errno;

    /* Funkcja 'cfsetispeed' zwróciła błąd */
    if (rs == -1) {

        this->meteoLog.err("setSerialPortSpeed(){ cfsetispeed(options,speed) = -1 }: ",
                           saved_err);

        std::exit(EXIT_FAILURE);
    }
}

void SerialTask::setSerialPortReadMin() {

    /* Set blocking read until MIN number bytes are available */
    this->options.c_cc[VMIN] = 0;
    this->options.c_cc[VTIME] = 0;

}

void SerialTask::setSerialPortNoMappingChars() {

    /* Set input options by modifying c_iflag by turning off mapping of characters */
    this->options.c_iflag &= ~(INPCK | INLCR | ICRNL | IGNCR);
    this->options.c_iflag |= IUTF8;
    this->options.c_iflag |= IGNBRK;
}

void SerialTask::setSerialPortRawInput() {

    /* Set local options by modifying c_lflag to raw input */
    this->options.c_lflag &= ~(ICANON | ECHO | ISIG | IEXTEN);

    /* Wyłącza zapis do urządzenia */
    this->options.c_oflag &= ~OPOST;
}

void SerialTask::setSerialPortDataBits() {

    this->options.c_cflag &= ~CSIZE;
    this->options.c_cflag |= CREAD;

    try {

        this->options.c_cflag |= this->mapDataBits.find(
                this->getConfigManager().getConfig("SerialDataBits", this->getConfigManager().SERIAL))->second;

    } catch (exception &e) {

        this->meteoLog.err(("setSerialPortDataBits(){ map.find() }: " + std::string(e.what())).c_str());

    }

    this->options.c_cflag |= CLOCAL;
}

void SerialTask::setSerialPortFlowControl() {

    std::string temp = this->getConfigManager().getConfig("SerialFlowControl", this->getConfigManager().SERIAL);


    /* Zmienia litery na małe */
    toLowerCase(temp, this->meteoLog);


    if (temp == "none") {

        this->options.c_cflag &= ~CRTSCTS;
        this->options.c_iflag &= ~(IXOFF | IXON);

    } else if (temp == "xoff/xon" || temp == "xon/xoff") {

        this->options.c_cflag &= ~CRTSCTS;
        this->options.c_iflag |= (IXOFF | IXON);

    } else if (temp == "rts/cts" || temp == "cts/rts") {

        this->options.c_cflag |= CRTSCTS;
        this->options.c_iflag &= ~(IXOFF | IXON);

    } else {

        this->meteoLog.err(
                "setSerialPortFlowControl(){}: Błędny parameter: sprawdź ustawienia w pliku 'rs232.yaml'.");

        exit(EXIT_FAILURE);
    }
}

void SerialTask::setSerialPortParity() {

    std::string temp = this->getConfigManager().getConfig("SerialParity", this->getConfigManager().SERIAL);

    /* Zmienia litery na małe */
    toLowerCase(temp, this->meteoLog);

    if (temp == "none") {

        this->options.c_cflag &= ~PARENB;

    } else if (temp == "odd") {

        this->options.c_cflag |= (PARENB | PARODD);

    } else if (temp == "even") {

        this->options.c_cflag |= PARENB;
        this->options.c_cflag &= ~PARODD;

    } else {

        this->meteoLog.err(
                "setSerialPortParity(){}: Błędny parameter: sprawdź ustawienia w pliku 'rs232.yaml'.");

        std::exit(EXIT_FAILURE);
    }
}

void SerialTask::setSerialPortStopBits() {

    std::string temp = this->getConfigManager().getConfig("SerialStopBits", this->getConfigManager().SERIAL);

    if (temp == "1") {

        this->options.c_cflag &= ~CSTOPB;

    } else if (temp == "2") {

        this->options.c_cflag |= CSTOPB;

    } else {

        this->meteoLog.err(
                "setSerialPortStopBits(){}: Błędny parameter: sprawdź ustawienia w pliku 'rs232.yaml'.");

        std::exit(EXIT_FAILURE);
    }
}

const std::string &SerialTask::getSerialPortPath() const {

    return this->serialPortPath;
}

void SerialTask::setSerialPortPath(const std::string &pserialPortPath) {

    this->serialPortPath = pserialPortPath;
}

size_t SerialTask::getBytesNumToRead() const {

    return this->bytesNumToRead;
}

void SerialTask::setBytesNumToRead(const size_t pbytesNumToRead) {

    this->bytesNumToRead = pbytesNumToRead;
}

const ConfigManager &SerialTask::getConfigManager() const {

    return this->configManager;
}


/* Konstruktor kopiujący */
SerialTask::SerialTask(const SerialTask &serialTask) : Task(serialTask.configManager), PIPEDESC(serialTask.PIPEDESC) {

    this->meteoLog = MyLog("MS::SerialTask");
    this->meteoLog = serialTask.meteoLog;

    /* Nie podaję wszystkich takich samych argumentów, ponieważ zaraz kopiuję DataManagera z instancji SerialTaska */
    this->bufferManager = new DataManager(
            serialTask.getConfigManager().getConfig("dataFilePath", this->getConfigManager().APP));
    this->bufferManager = serialTask.bufferManager;

}


/* Destruktor */
SerialTask::~SerialTask() {

    this->closeSerialPort();

    close(this->PIPEDESC);

    delete this->bufferManager;

}


