//
// Created by Marcin Guziołek on 06.04.18.
//

#include "ProgramConfig.h"

ProgramConfig::ProgramConfig(const unsigned int *sleepTimes)
{
    const size_t * dataLength =  new size_t(10);

    this->setDaemonsSleepTimes(sleepTimes);
    this->setDaemonsNames(new string[2]{"rs232d", "httpd"});
    this->setAppName("meteo-station");
    this->setPidFileDirPath("/etc/meteo-station/");
    this->setOffsetDirPath("/etc/meteo-station/offset");
    this->setConfigDirPath("/etc/meteo-station/config");
    this->setDataDirPath("/var/meteo-data/");
    this->setOffsetFilePath("/etc/meteo-station/offset/seek.offset");
    this->setHttpConfigPath("/etc/meteo-station/config/http.config");
    this->setRs232ConfigPath("/etc/meteo-station/config/rs232.config");
    this->setDataFilePath("/var/meteo-data/meteo.data");
    this->setDataLength(dataLength);

}

void ProgramConfig::setDaemonsNames(string *daemonsNames)
{
    this->daemonsNames = daemonsNames;
}


ProgramConfig::~ProgramConfig()
{
    /*delete this->daemonsNames;
    delete this->daemonsSleepTimes;
    delete this->daemonsSleepTimes;
    delete this->pidFileDirPath;
    delete this->offsetFilePath;
    delete this->dataFilePath;
    delete this->appName;
    delete this->httpConfigPath;
    delete this->rs232ConfigPath;*/
}

const string &ProgramConfig::getAppName() const
{
    return this->appName;
}

void ProgramConfig::setAppName(const string &appName)
{
    this->appName = appName;
}

const string &ProgramConfig::getDataFilePath() const
{
    return this->dataFilePath;
}

void ProgramConfig::setDataFilePath(const string &dataFilePath)
{
    this->dataFilePath = dataFilePath;
}

const string &ProgramConfig::getOffsetFilePath() const
{
    return this->offsetFilePath;
}

void ProgramConfig::setOffsetFilePath(const string &offsetFilePath)
{
    this->offsetFilePath = offsetFilePath;
}

const string &ProgramConfig::getPidFileDirPath() const
{
    return this->pidFileDirPath;
}

void ProgramConfig::setPidFileDirPath(const string &pidFileDirPath)
{
    this->pidFileDirPath = pidFileDirPath;
}

const string *ProgramConfig::getDaemonsNames() const
{
    return this->daemonsNames;
}

const string &ProgramConfig::getHttpConfigPath() const
{
    return this->httpConfigPath;
}

void ProgramConfig::setHttpConfigPath(const string &httpConfigPath)
{
    this->httpConfigPath = httpConfigPath;
}

const string &ProgramConfig::getRs232ConfigPath() const
{
    return this->rs232ConfigPath;
}

void ProgramConfig::setRs232ConfigPath(const string &rs232ConfigPath)
{
    this->rs232ConfigPath = rs232ConfigPath;
}

const string &ProgramConfig::getOffsetDirPath() const
{
    return this->offsetDirPath;
}

void ProgramConfig::setOffsetDirPath(const string &offsetDirPath)
{
    this->offsetDirPath = offsetDirPath;
}

const string &ProgramConfig::getConfigDirPath() const
{
    return this->configDirPath;
}

void ProgramConfig::setConfigDirPath(const string &configDirPath)
{
    this->configDirPath = configDirPath;
}

const string &ProgramConfig::getDataDirPath() const
{
    return this->dataDirPath;
}

void ProgramConfig::setDataDirPath(const string &dataDirPath)
{
    this->dataDirPath = dataDirPath;
}

const unsigned int *ProgramConfig::getDaemonsSleepTimes() const
{
    return this->daemonsSleepTimes;
}

void ProgramConfig::setDaemonsSleepTimes(const unsigned int *daemonsSleepTimes)
{
    this->daemonsSleepTimes = daemonsSleepTimes;
}

const size_t *ProgramConfig::getDataLength() const
{
    return this->dataLength;
}

void ProgramConfig::setDataLength(const size_t *dataLength)
{
    this->dataLength = dataLength;
}
