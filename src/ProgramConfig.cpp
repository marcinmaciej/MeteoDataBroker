//
// Created by Marcin Guziołek on 06.04.18.
//

#include "ProgramConfig.h"

ProgramConfig::ProgramConfig(const unsigned int *sleepTimes)
{

    this->daemonsSleepTimes = new unsigned int[2];
    this->daemonsSleepTimes = sleepTimes;
    this->daemonsNames = new string[2]{"rs232d", "httpd"};
    this->appName = new string("meteo-station");
    this->pidFileDirPath = new string("/etc/meteo-station/");
    this->offsetDirPath = new string("/etc/meteo-station/offset");
    this->configDirPath = new string("/etc/meteo-station/config");
    this->dataDirPath = new string("/var/meteo-data");
    this->offsetFilePath = new string("/etc/meteo-station/offset/.seek.offset");
    this->httpConfigPath = new string("/etc/meteo-station/config/http.config");
    this->rs232ConfigPath = new string("/etc/meteo-station/config/rs232.config");
    this->dataFilePath = new string("/var/meteo-data/meteo.data");

}

const string *ProgramConfig::getOffsetDirPath() const
{
    return this->offsetDirPath;
}

const string *ProgramConfig::getConfigDirPath() const
{
    return this->configDirPath;
}

const string *ProgramConfig::getDataDirPath() const
{
    return this->dataDirPath;
}

const string *ProgramConfig::getAppName() const
{
    return this->appName;
}

const string *ProgramConfig::getDataFilePath() const
{
    return this->dataFilePath;
}

const string *ProgramConfig::getOffsetFilePath() const
{
    return this->offsetFilePath;
}

const string *ProgramConfig::getPidFileDirPath() const
{
    return this->pidFileDirPath;
}

const string *ProgramConfig::getDaemonsNames() const
{
    return this->daemonsNames;
}

const unsigned int *ProgramConfig::getDaemonsSleepTimes() const
{
    return this->daemonsSleepTimes;
}

const string *ProgramConfig::getHttpConfigPath() const
{
    return this->httpConfigPath;
}

const string *ProgramConfig::getRs232ConfigPath() const
{
    return this->rs232ConfigPath;
}

void ProgramConfig::setRs232ConfigPath(const string *rs232ConfigPath)
{
    this->rs232ConfigPath = rs232ConfigPath;
}

void ProgramConfig::setHttpConfigPath(const string *httpConfigPath)
{
    this->httpConfigPath = httpConfigPath;
}

void ProgramConfig::setDaemonsSleepTimes(const unsigned int *daemonsSleepTimes)
{
    this->daemonsSleepTimes = daemonsSleepTimes;
}

ProgramConfig::~ProgramConfig()
{
    delete this->daemonsNames;
    delete this->daemonsSleepTimes;
    delete this->daemonsSleepTimes;
    delete this->pidFileDirPath;
    delete this->offsetFilePath;
    delete this->dataFilePath;
    delete this->appName;
    delete this->httpConfigPath;
    delete this->rs232ConfigPath;
}
