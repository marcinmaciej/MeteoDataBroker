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
    this->pidFilePath = new string("/etc/meteo-station/");
    this->offsetFilePath = new string("/etc/meteo-station/offset/.seek.offset");
    this->httpConfigPath = new string("/etc/meteo-station/config/http.config");
    this->rs232ConfigPath = new string("/etc/meteo-station/config/rs232.config");
    this->dataFilePath = new string("/var/meteo-data/meteo.data");

}

const string &ProgramConfig::getAppName() const
{
    return *this->appName;
}

const string &ProgramConfig::getDataFilePath() const
{
    return *this->dataFilePath;
}

const string &ProgramConfig::getOffsetFilePath() const
{
    return *this->offsetFilePath;
}

const string &ProgramConfig::getPidFilePath() const
{
    return *this->pidFilePath;
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
    delete this->pidFilePath;
    delete this->offsetFilePath;
    delete this->dataFilePath;
    delete this->appName;
    delete this->httpConfigPath;
    delete this->rs232ConfigPath;
}
