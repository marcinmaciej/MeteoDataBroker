//
// Created by marcin on 06.04.18.
//

#include <fcntl.h>
#include "ConfigManager.h"

void serialSetPort(ConfigManager &manager, const string &paramVal)
{
    manager.serialPort->setPort(paramVal);
}

void serialSetSpeed(ConfigManager &manager, const string &paramVal)
{
    manager.serialPort->setPort(paramVal);
}

void serialSetDataBits(ConfigManager &manager, const string &paramVal)
{
    manager.serialPort->setPort(paramVal);
}

void serialSetStopBits(ConfigManager &manager, const string &paramVal)
{
    manager.serialPort->setPort(paramVal);
}

void serialSetParity(ConfigManager &manager, const string &paramVal)
{
    manager.serialPort->setPort(paramVal);
}

void serialSetFlowControl(ConfigManager &manager, const string &paramVal)
{
    manager.serialPort->setPort(paramVal);
}

void httpSetHostIp(ConfigManager &manager, const string &paramVal)
{
    manager.connectionConfig->setHostIp(paramVal);
}

void httpSetHostName(ConfigManager &manager, const string &paramVal)
{
    manager.connectionConfig->setHostName(paramVal);
}

void httpSetPort(ConfigManager &manager, const string &paramVal)
{
    manager.connectionConfig->setPort(atoi(paramVal.c_str()));
}

void httpSetAddressFamily(ConfigManager &manager, const string &paramVal)
{
    manager.connectionConfig->setAddressFamily(paramVal);
}

void httpSetSocketType(ConfigManager &manager, const string &paramVal)
{
    manager.connectionConfig->setSocketType(paramVal);
}

void httpSetProtocol(ConfigManager &manager, const string &paramVal)
{
    manager.connectionConfig->setProtocol(paramVal);
}

void httpSetRequest(ConfigManager &manager, const string &paramVal)
{
    manager.connectionConfig->setRequest(paramVal);
}

ConfigManager::ConfigManager()
{
    this->setParams.insert(pair<string, void (*)(ConfigManager &, const string &)>("port", serialSetPort));
    this->setParams.insert(pair<string, void (*)(ConfigManager &, const string &)>("speed", serialSetSpeed));
    this->setParams.insert(
            pair<string, void (*)(ConfigManager &, const string &)>("databits", serialSetDataBits));
    this->setParams.insert(
            pair<string, void (*)(ConfigManager &, const string &)>("stopbits", serialSetStopBits));
    this->setParams.insert(pair<string, void (*)(ConfigManager &, const string &)>("parity", serialSetParity));
    this->setParams.insert(
            pair<string, void (*)(ConfigManager &, const string &)>("flowcontrol", serialSetFlowControl));

    this->setParams.insert(pair<string, void (*)(ConfigManager &, const string &)>("kdkdk", httpSetHostIp));
    this->setParams.insert(pair<string, void (*)(ConfigManager &, const string &)>("port", httpSetHostName));
    this->setParams.insert(pair<string, void (*)(ConfigManager &, const string &)>("speed", httpSetPort));
    this->setParams.insert(
            pair<string, void (*)(ConfigManager &, const string &)>("databits", httpSetAddressFamily));
    this->setParams.insert(
            pair<string, void (*)(ConfigManager &, const string &)>("stopbits", httpSetSocketType));
    this->setParams.insert(pair<string, void (*)(ConfigManager &, const string &)>("parity", httpSetProtocol));
    this->setParams.insert(
            pair<string, void (*)(ConfigManager &, const string &)>("flowcontrol", httpSetRequest));

    this->meteoLog = new MeteoLog("MS-ConfigManager");

    this->programConfig = new ProgramConfig();

    this->serialPort = new SerialPort();

    this->connectionConfig = new ConnectionConfig();

    if (!this->loadSerialPort(*this->programConfig->getRs232ConfigPath()))
    {
        this->setDefaultSerial();
    }

    if (!this->loadHttp(*this->programConfig->getHttpConfigPath()))
    {
        this->setDefaultHttp();
    }
}

const char *ConfigManager::getDefaultSerialToSave()
{
    string config;

    config = "Port=" + this->serialPort->getPort();
    config += ",Speed=" + this->serialPort->getSpeed();
    config += ",DataBits=" + this->serialPort->getDataBits();
    config += ",StopBits=" + this->serialPort->getStopBits();
    config += ",Parity=" + this->serialPort->getParity();
    config += ",FlowControl=" + this->serialPort->getFlowControl();

    return config.c_str();
}

const char *ConfigManager::getDefaultHttpToSave()
{
    string config;

    config = "HostIp=" + this->connectionConfig->getHostIp();
    config += ",HostName=" + this->connectionConfig->getHostName();
    config += ",Port=" + this->connectionConfig->getPort();
    config += ",AddressFamily=" + this->connectionConfig->getAddressFamily();
    config += ",SocketType=" + this->connectionConfig->getSocketType();
    config += ",Protocol=" + this->connectionConfig->getProtocol();
    config += ",Request=" + this->connectionConfig->getRequest();

    return config.c_str();
}

void ConfigManager::setDefaultSerial()
{
    this->serialPort->setPort();
    this->serialPort->setSpeed();
    this->serialPort->setDataBits();
    this->serialPort->setStopBits();
    this->serialPort->setParity();
    this->serialPort->setFlowControl();

    this->saveDefaultSerial();
}

void ConfigManager::setDefaultHttp()
{
    this->connectionConfig->setHostIp();
    this->connectionConfig->setHostName();
    this->connectionConfig->setPort();
    this->connectionConfig->setSocketType();
    this->connectionConfig->setAddressFamily();
    this->connectionConfig->setProtocol();
    this->connectionConfig->setRequest();

    this->saveDefaultHttp();
}

void ConfigManager::saveDefaultSerial()
{
    try
    {
        int fd;
        ssize_t bw = 0, btw = 0;
        const char *config;

        config = this->getDefaultSerialToSave();

        btw = strlen(config);

        fd = open(this->programConfig->getRs232ConfigPath()->c_str(), O_CREAT | O_WRONLY, 0600);
        if (fd == -1)
        {
            throw errno;
        }

        bw = write(fd, config, btw);
        if (bw != btw)
        {
            throw errno;
        }

        close(fd);

    } catch (...)
    {
        this->meteoLog->err(strerror(errno));
    }
}

void ConfigManager::saveDefaultHttp()
{
    try
    {
        int fd;
        ssize_t bw = 0;
        size_t btw = 0;
        const char *config;

        config = this->getDefaultHttpToSave();

        btw = strlen(config);

        fd = open(this->programConfig->getHttpConfigPath()->c_str(), O_CREAT | O_WRONLY, 0600);
        if (fd == -1)
        {
            throw errno;
        }

        bw = write(fd, config, btw);
        if (bw != btw)
        {
            throw errno;
        }

        close(fd);

    } catch (...)
    {
        this->meteoLog->err(strerror(errno));
    }
}

bool ConfigManager::loadSerialPort(const string &path)
{
    return getParams(path);
}

bool ConfigManager::getParams(const string &path) const
{
    try
    {
        ConfigManager manager = ConfigManager();

        string params,
                paramValue,
                paramName;

        basic_string::size_type bPos = 0,
                ePos = 0,

                length = 0;

        params = loadParams(path);

        if (params.empty())
        {
            return false;
        }

        length = params.length();

        while ((ePos = params.find_first_of(COMMA)))
        {
            substringParams(&params, &bPos, &ePos, &paramValue, &paramName);

            setParams.find(paramName)->second(manager, paramValue);
        }

        /* Get last param behind the last comma */
        substringParams(&params, &bPos, &length, &paramValue, &paramName);

        setParams.find(paramName)->second(manager, paramValue);

        return true;
        //temp = temp.erase(temp.find_first_of(ENDDELIMITER));

    } catch (...)
    {
        meteoLog->err(strerror(errno));
    }
}

void
ConfigManager::substringParams(const string *params, basic_string::size_type *bPos, const basic_string::size_type *ePos,
                               string *paramValue,
                               string *paramName) const
{
    string param;
    string::size_type pos = 0;

    param = params->substr(*bPos, *ePos - 1); /* Get substring to first comma key=value */

    pos = param.find_first_of(ASSIGNMENT); /* Find position of equal sign */

    *paramValue = param.substr(pos + 1, param.length()); /* Get substring of value */

    *paramName = param.substr(*bPos, pos - 1);

    *bPos += *ePos;

    transform(paramName->begin(), paramName->end(), paramName->begin(),
              tolower); /* Change all letters to lower */
}

bool ConfigManager::loadHttp(const string &path)
{
    return getParams(path);
}

const char *ConfigManager::loadParams(const string &path) const
{
    FILE *fd;
    char *line = nullptr;
    ssize_t br = 0;
    size_t len = 0;

    try
    {
        fd = fopen(path.c_str(), "r");
        if (!fd)
        {
            return "";
        }

        br = getline(&line, &len, fd);

        if (br != len)
        {
            throw errno;
        }
    } catch (...)
    {
        this->meteoLog->err(strerror(errno));
    }

    return line;
}

ConfigManager::~ConfigManager()
{
    delete this->programConfig;

    delete this->serialPort;

    delete this->connectionConfig;
}
