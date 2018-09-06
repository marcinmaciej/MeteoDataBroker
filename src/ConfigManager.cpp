/** @author  Created by Marcin Guziołek on 06.04.18.
 *  @brief Class ConfigManager manages app configuration
 *
 */

#include "ConfigManager.h"

/** @brief function for configuring serial port
 *
 *  @param manager
 *   @param paramVal
 */
void serialSetPort(const ConfigManager *manager, const string paramVal)
{
    manager->serialConfig->setPort(paramVal);
}

void serialSetSpeed(const ConfigManager *manager, const string paramVal)
{
    manager->serialConfig->setSpeed(paramVal);
}

void serialSetDataBits(const ConfigManager *manager, const string paramVal)
{
    manager->serialConfig->setDataBits(paramVal);
}

void serialSetStopBits(const ConfigManager *manager, const string paramVal)
{
    manager->serialConfig->setStopBits(paramVal);
}

void serialSetParity(const ConfigManager *manager, const string paramVal)
{
    manager->serialConfig->setParity(paramVal);
}

void serialSetFlowControl(const ConfigManager *manager, const string paramVal)
{
    manager->serialConfig->setFlowControl(paramVal);
}


void serialSetSleepTime(const ConfigManager *manager, const string paramVal)
{
    manager->serialConfig->setSleepTime(paramVal);
}

void serialSetDataLength(const ConfigManager *manager, const string paramVal)
{
    manager->serialConfig->setDataLength(paramVal.c_str());
}

/** @brief functions for configuring http connection
 *
 *  @param manager
 *  @param paramVal
 */
void httpSetHostIp(const ConfigManager *manager, const string paramVal)
{
    manager->httpConfig->setHostIp(paramVal);
}

void httpSetHostName(const ConfigManager *manager, const string paramVal)
{
    manager->httpConfig->setHostName(paramVal);
}

void httpSetPort(const ConfigManager *manager, const string paramVal)
{
    manager->httpConfig->setPort(paramVal);
}

void httpSetAddressFamily(const ConfigManager *manager, const string paramVal)
{
    manager->httpConfig->setAddressFamily(paramVal);
}

void httpSetSocketType(const ConfigManager *manager, const string paramVal)
{
    manager->httpConfig->setSocketType(paramVal);
}

void httpSetProtocol(const ConfigManager *manager, const string paramVal)
{
    manager->httpConfig->setProtocol(paramVal);
}

void httpSetRequest(const ConfigManager *manager, const string paramVal)
{
    manager->httpConfig->setRequest(paramVal);
}

void httpSetSleepTime(const ConfigManager *manager, const string paramVal)
{
    manager->httpConfig->setSleepTime(paramVal);
}

void httpSetWaitNetwork(const ConfigManager *manager, const string paramVal)
{
    manager->httpConfig->setWaitNetwork(paramVal);
}

void httpSetDataLength(const ConfigManager *manager, const string paramVal)
{
    manager->httpConfig->setDataLength(paramVal);
}

/** @brief ConfigManager constructor
 *
 */
ConfigManager::ConfigManager()
{
    this->meteoLog = new MyLog("MS-ConfigManager");

    this->programConfig = new AppConfig();

    this->serialConfig = new SerialConfig();

    this->httpConfig = new HttpConfig();

/** @brief A map with serial port params of type string and setters functions
*
*/
    this->mapParamToMethod.insert(
            pair<const string, void (*)(const ConfigManager *, const string)>("serialport", serialSetPort));
    this->mapParamToMethod.insert(
            pair<const string, void (*)(const ConfigManager *, const string)>("serialspeed", serialSetSpeed));
    this->mapParamToMethod.insert(
            pair<const string, void (*)(const ConfigManager *, const string)>("serialdatabits", serialSetDataBits));
    this->mapParamToMethod.insert(
            pair<const string, void (*)(const ConfigManager *, const string)>("serialstopbits", serialSetStopBits));
    this->mapParamToMethod.insert(
            pair<const string, void (*)(const ConfigManager *, const string)>("serialparity", serialSetParity));
    this->mapParamToMethod.insert(
            pair<const string, void (*)(const ConfigManager *, const string)>("serialflowcontrol", serialSetFlowControl));
    this->mapParamToMethod.insert(
            pair<const string, void (*)(const ConfigManager *, const string)>("serialsleeptime", serialSetSleepTime));
    this->mapParamToMethod.insert(
            pair<const string, void (*)(const ConfigManager *, const string)>("serialdatalength", serialSetDataLength));

/** @brief The map with http connection params of type string and setters functions
 *
 */
    this->mapParamToMethod.insert(
            pair<const string, void (*)(const ConfigManager *, const string)>("httphostip", httpSetHostIp));
    this->mapParamToMethod.insert(
            pair<const string, void (*)(const ConfigManager *, const string)>("httphostname", httpSetHostName));
    this->mapParamToMethod.insert(
            pair<const string, void (*)(const ConfigManager *, const string)>("httpport", httpSetPort));
    this->mapParamToMethod.insert(
            pair<const string, void (*)(const ConfigManager *, const string)>("httpaddressfamily",
                                                                              httpSetAddressFamily));
    this->mapParamToMethod.insert(
            pair<const string, void (*)(const ConfigManager *, const string)>("httpsockettype", httpSetSocketType));
    this->mapParamToMethod.insert(
            pair<const string, void (*)(const ConfigManager *, const string)>("httpprotocol", httpSetProtocol));
    this->mapParamToMethod.insert(
            pair<const string, void (*)(const ConfigManager *, const string)>("httprequest", httpSetRequest));
    this->mapParamToMethod.insert(
            pair<const string, void (*)(const ConfigManager *, const string)>("httpsleeptime", httpSetSleepTime));
    this->mapParamToMethod.insert(
            pair<const string, void (*)(const ConfigManager *, const string)>("httpwaitnetwork", httpSetWaitNetwork));
    this->mapParamToMethod.insert(
            pair<const string, void (*)(const ConfigManager *, const string)>("httpdatalength", httpSetDataLength));

/** @brief The params loading and initiating block for serial port
 *
 */
    if (!this->loadSerialPort(this->programConfig->getRs232ConfigPath()))
    {
        this->makeAllPaths();
        this->saveDefault(this->programConfig->getRs232ConfigPath(), this->getDefaultSerialToSave());
    }

/** @brief The params loading and initiating block for http connection
 *
 */
    if (!this->loadHttp(this->programConfig->getHttpConfigPath()))
    {
        this->makeAllPaths();
        this->saveDefault(this->programConfig->getHttpConfigPath(), this->getDefaultHttpToSave());
    }
}

const char *ConfigManager::getDefaultSerialToSave()
{
    string config;

    config = "SerialSleepTime=" + to_string(this->serialConfig->getSleepTime());
    config += ",SerialDataLength=" + to_string(this->serialConfig->getDataLength());
    config += ",SerialPort=" + this->serialConfig->getPort();
    config += ",SerialSpeed=" + this->serialConfig->getSpeed();
    config += ",SerialDataBits=" + this->serialConfig->getDataBits();
    config += ",SerialStopBits=" + this->serialConfig->getStopBits();
    config += ",SerialParity=" + this->serialConfig->getParity();
    config += ",SerialFlowControl=" + this->serialConfig->getFlowControl();
    /* Add character marker closing the string of parameters */
    config += ';';

    return config.c_str();
}

const char *ConfigManager::getDefaultHttpToSave()
{
    string config;

    config = "HttpDataLength=" + to_string(this->httpConfig->getDataLength());
    config += ",HttpSleepTime=" + to_string(this->httpConfig->getSleepTime());
    config += ",HttpWaitNetwork=" + to_string(this->httpConfig->getWaitNetwork());
    config += ",HttpHostIp=" + this->httpConfig->getHostIp();
    config += ",HttpHostName=" + this->httpConfig->getHostName();
    config += ",HttpPort=" + this->httpConfig->getPort();
    config += ",HttpAddressFamily=" + this->httpConfig->getAddressFamily();
    config += ",HttpSocketType=" + this->httpConfig->getSocketType();
    config += ",HttpProtocol=" + this->httpConfig->getProtocol();
    config += ",HttpRequest=" + this->httpConfig->getRequest();
    /* Add character marker closing the string of parameters */
    config += ';';

    return config.c_str();
}

void ConfigManager::saveDefault(const string &path, const char *config)
{
    try
    {
        int fd;
        ssize_t bw = 0,
                btw = 0;

        btw = strlen(config);

        fd = open(path.c_str(), O_CREAT | O_WRONLY, 0777);
        if (fd == -1)
        {
            throw errno;
        }

        bw = write(fd, config, btw);

        close(fd);

        if (bw != btw)
        {
            throw errno;
        }

    }
    catch (...)
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
        string params,
                paramValue,
                paramName;

        string::size_type parStart = 0,
                parEnd = 0,
                length = 0;

        params = loadParams(path);

        if (params.empty())
        {
            return false;
        }

        length = params.length();

        while (string::npos != (parEnd = params.find(COMMA, parStart)))
        {
            substringParams(&params, &parStart, &parEnd, &paramValue, &paramName);

            mapParamToMethod.find(paramName)->second(this, paramValue);
        }

        /* Get last param behind the last comma */
        substringParams(&params, &parStart, &length, &paramValue, &paramName);

        mapParamToMethod.find(paramName)->second(this, paramValue);

        return true;

    } catch (...)
    {
        meteoLog->err(strerror(errno));
    }
}

void
ConfigManager::substringParams(const string *params, string::size_type *parStart, const string::size_type *parEnd,
                               string *paramValue,
                               string *paramName) const
{
    string param;
    string::size_type subStart = 0;
    string::size_type parLength = 0;

    parLength = *parEnd - *parStart;

    param = params->substr(*parStart, parLength); /* Get substring to first comma key=value */

    subStart = param.find(EQUALSIGN); /* Find position of equal sign */

    *paramValue = param.substr(subStart + 1, param.length()); /* Get substring of value */

    *paramName = param.substr(0, subStart);

    *parStart += parLength + 1;

    transform(paramName->begin(), paramName->end(), paramName->begin(),
              ::tolower); /* Change all letters to lower */
}

bool ConfigManager::loadHttp(const string &path)
{
    return getParams(path);
}

const char *ConfigManager::loadParams(const string &path) const
{
    FILE *fd;
    char *line = nullptr;
    ssize_t resultLen = 0;
    size_t len = 0;
    auto *allLines = new string();

    try
    {
        fd = fopen(path.c_str(), "r");
        if (!fd)
        {
            return "";
        }

        while ((resultLen = getline(&line, &len, fd)) > -1)
        {
            len = strlen(line);

            if (resultLen != len)
            {
                throw errno;
            }
            *allLines += line;
            line = nullptr;
        }

        fclose(fd);


    } catch (...)
    {
        this->meteoLog->err(strerror(errno));
    }

    allLines->erase(allLines->rfind(';'));
    fd = nullptr;

    return allLines->c_str();
}

void ConfigManager::makeDir(const string &path)
{
    try
    {

        mkdir(path.c_str(), 0744);

    } catch (...)
    {
        this->meteoLog->err(strerror(errno));
    }
}

void ConfigManager::makeAllPaths()
{
    this->makeDir(this->programConfig->getPidFileDirPath());
    this->makeDir(this->programConfig->getConfigDirPath());
    this->makeDir(this->programConfig->getOffsetDirPath());
    this->makeDir(this->programConfig->getDataDirPath());
}

ConfigManager::~ConfigManager()
{
    delete this->programConfig;

    delete this->serialConfig;

    delete this->httpConfig;
}
