/**
 * @author  Created by Marcin Guziołek on 06.04.18.
 */

#include "SerialTask.h"

SerialTask::SerialTask(const ConfigManager *configManager) : Task(configManager)
{

    Task::logName = "MS-RS232TaskClass";

    Task::meteoLog = new MyLog("MS-RS232TaskClass");

    this->setSerialPortPath(this->configManager->serialConfig->getPort().c_str());

    this->setBytesNumToRead(this->configManager->serialConfig->getDataLength());


    this->mapDataBits.insert(pair<const string, int>("5", CS5));
    this->mapDataBits.insert(pair<const string, int>("6", CS6));
    this->mapDataBits.insert(pair<const string, int>("7", CS7));
    this->mapDataBits.insert(pair<const string, int>("8", CS8));

    this->mapSpeed.insert(pair<const string, speed_t>("0", B0));
    this->mapSpeed.insert(pair<const string, speed_t>("50", B50));
    this->mapSpeed.insert(pair<const string, speed_t>("75", B75));
    this->mapSpeed.insert(pair<const string, speed_t>("110", B110));
    this->mapSpeed.insert(pair<const string, speed_t>("134", B134));
    this->mapSpeed.insert(pair<const string, speed_t>("150", B150));
    this->mapSpeed.insert(pair<const string, speed_t>("200", B200));
    this->mapSpeed.insert(pair<const string, speed_t>("300", B300));
    this->mapSpeed.insert(pair<const string, speed_t>("600", B600));
    this->mapSpeed.insert(pair<const string, speed_t>("1200", B1200));
    this->mapSpeed.insert(pair<const string, speed_t>("1800", B1800));
    this->mapSpeed.insert(pair<const string, speed_t>("2400", B2400));
    this->mapSpeed.insert(pair<const string, speed_t>("4800", B4800));
    this->mapSpeed.insert(pair<const string, speed_t>("9600", B9600));
    this->mapSpeed.insert(pair<const string, speed_t>("19200", B19200));
    this->mapSpeed.insert(pair<const string, speed_t>("38400", B38400));
    this->mapSpeed.insert(pair<const string, speed_t>("57600", B57600));
    this->mapSpeed.insert(pair<const string, speed_t>("115200", B115200));



    /* Open port */
    this->openPort();
}

const ConfigManager *SerialTask::getConfigManager() const
{

    return this->configManager;
}

void SerialTask::task(const char *logName)
{
    try
    {

        /* Set port configuration to read */
        this->setPortConfig();

        /* Read from port */
        auto *readData = new string(this->readPort());

        this->bufferManager = new BufferManager(O_WRONLY | O_CREAT | O_APPEND, this->getConfigManager());

        if (!readData->empty())
        {
            /* Write to buffer before sending */
            this->bufferManager->writeBuffer(readData->c_str());
        }

        this->bufferManager->closeBuffer();

        delete readData;

        sleep(this->getConfigManager()->serialConfig->getSleepTime());

    }
    catch (...)
    {
        this->meteoLog->err(strerror(errno));
    }
}

const char * SerialTask::readPort()
{

    try
    {
        ssize_t bytesRead = 0;
        size_t len = this->getBytesNumToRead();
        auto *buf = new char[len + 1]();

#if SHOW_SERIAL
        ioctl(this->getFileDesc(), FIONREAD, &this->bytesAvailable);
        string ms("Bytes available = ");
        ms += to_string(this->bytesAvailable);
        this->meteoLog->warn(ms.c_str());
#endif // SHOW_SERIAL

        bytesRead = read(this->getFileDesc(), buf, len);

        if (bytesRead < 0)
        {
            throw errno;
        }

#if SHOW_SERIAL
        ms.clear();
        ms = "Content read from port = ";
        ms += buf;
        this->meteoLog->warn(ms.c_str());
#endif // SHOW_SERIAL
        return buf;
    }
    catch (...)
    {
        this->meteoLog->err(strerror(errno));
        return "";
    }
}

void SerialTask::closePort()
{
    close(this->fileDescriptor);
}

int SerialTask::getFileDesc()
{
    return this->fileDescriptor;
}

void SerialTask::setFileDesc(int fd)
{
    this->fileDescriptor = fd;
}

void SerialTask::setPortConfig()
{
    try
    {

        /* Get current settings for port */
        tcgetattr(this->getFileDesc(), &this->options);

        this->setPortSpeed();

        /* Set control options by modifying c_cflag to typical frame: 8n1 */
        this->setPortStopBits();

        this->setPortParity();

        this->setPortFlowControl();

        this->setPortDataBits();

        this->setPortRawInput();

        this->setPortNoMappingChars();

        this->setPortReadMin();

        /* Set port modified settings immediately */
        tcsetattr(this->getFileDesc(), TCSANOW, &this->options);

        /* Set non blocking reading from port */
        fcntl(this->getFileDesc(), F_SETFL, FNDELAY);

    }
    catch (...)
    {
        this->meteoLog->err(strerror(errno));

    }
}


void SerialTask::setPortSpeed()
{
    /* Set baud rate */
    cfsetispeed(&this->options, mapSpeed.find(this->getConfigManager()->serialConfig->getSpeed())->second);
}

void SerialTask::setPortReadMin()
{
    /* Set blocking read until MIN number bytes are available */
    this->options.c_cc[VMIN] = 0;
    this->options.c_cc[VTIME] = 0;
}

void SerialTask::setPortNoMappingChars()
{
    /* Set input options by modifying c_iflag by turning off mapping of characters */
    this->options.c_iflag &= ~(INPCK | INLCR | ICRNL | IGNCR);
    this->options.c_iflag |= IUTF8;
    this->options.c_iflag |= IGNBRK;
}

void SerialTask::setPortRawInput()
{
    /* Set local options by modifying c_lflag to raw input */
    this->options.c_lflag &= ~(ICANON | ECHO | ISIG | IEXTEN);

    /* Disable writing */
    this->options.c_oflag &= ~OPOST;
}

void SerialTask::setPortDataBits()
{
    this->options.c_cflag &= ~CSIZE;
    this->options.c_cflag |= CREAD;
    this->options.c_cflag |= mapDataBits.find(this->getConfigManager()->serialConfig->getDataBits())->second;
    this->options.c_cflag |= CLOCAL;
}

void SerialTask::setPortFlowControl()
{
    string temp = this->getConfigManager()->serialConfig->getFlowControl();
    /* Change all letters to lower */
    transform(temp.begin(), temp.end(), temp.begin(), ::tolower);

    if (temp == "none")
    {
        this->meteoLog->err("flow control none");
        this->options.c_cflag &= ~CRTSCTS;
        this->options.c_iflag &= ~(IXOFF | IXON);
    }
    else if (temp == "xoff/xon" || temp == "xon/xoff")
    {
        this->options.c_cflag &= ~CRTSCTS;
        this->options.c_iflag |= (IXOFF | IXON);
    }
    else if (temp == "rts/cts" || temp == "cts/rts")
    {
        this->options.c_cflag |= CRTSCTS;
        this->options.c_iflag &= ~(IXOFF | IXON);
    }
}

void SerialTask::setPortParity()
{
    string temp = this->getConfigManager()->serialConfig->getParity();
    /* Change all letters to lower */
    transform(temp.begin(), temp.end(), temp.begin(), ::tolower);

    if (temp == "none")
    {
        this->meteoLog->err("parity none");
        this->options.c_cflag &= ~PARENB;

    }
    else if (temp == "odd")
    {
        this->options.c_cflag |= (PARENB | PARODD);

    }
    else if (temp == "even")
    {
        this->options.c_cflag |= PARENB;
        this->options.c_cflag &= ~PARODD;
    }
}

void SerialTask::setPortStopBits()
{
    string temp = this->getConfigManager()->serialConfig->getStopBits();

    if (temp == "1")
    {
        this->options.c_cflag &= ~CSTOPB;

    }
    else if (temp == "2")
    {
        this->options.c_cflag |= CSTOPB;
    }
}

void SerialTask::openPort()
{
    /* Open port for blocking reading */
    /* Add O_NDELAY or O_NONBLOCK for nonblocking mode */
    this->meteoLog->err(this->getSerialPortPath());
    this->setFileDesc(open(this->getSerialPortPath(), O_RDONLY | O_NOCTTY | O_NDELAY));


    if (this->getFileDesc() < 0)
    {
        throw errno;
    }
}

SerialTask::~SerialTask()
{
    this->closePort();

    this->meteoLog->err("BYE BYE");

    delete this->buffer;
    delete this->bufferManager;
    delete this->meteoLog;
}

const char *SerialTask::getSerialPortPath() const
{
    return this->serialPortPath;
}

void SerialTask::setSerialPortPath(const char *serialPortPath)
{
    this->serialPortPath = serialPortPath;
}

const size_t SerialTask::getBytesNumToRead() const
{
    return this->bytesNumToRead;
}

void SerialTask::setBytesNumToRead(const size_t bytesNumToRead)
{
    this->bytesNumToRead = bytesNumToRead;
}

