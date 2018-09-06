/** @author  Created by Marcin Guziołek on 06.04.18.
 *
 */

#include "AppConfig.h"

AppConfig::AppConfig()
{
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

}

void AppConfig::setDaemonsNames(string *daemonsNames)
{
    this->daemonsNames = daemonsNames;
}

const string &AppConfig::getAppName() const
{
    return this->appName;
}

void AppConfig::setAppName(const string &appName)
{
    this->appName = appName;
}

const string &AppConfig::getDataFilePath() const
{
    return this->dataFilePath;
}

void AppConfig::setDataFilePath(const string &dataFilePath)
{
    this->dataFilePath = dataFilePath;
}

const string &AppConfig::getOffsetFilePath() const
{
    return this->offsetFilePath;
}

void AppConfig::setOffsetFilePath(const string &offsetFilePath)
{
    this->offsetFilePath = offsetFilePath;
}

const string &AppConfig::getPidFileDirPath() const
{
    return this->pidFileDirPath;
}

void AppConfig::setPidFileDirPath(const string &pidFileDirPath)
{
    this->pidFileDirPath = pidFileDirPath;
}

const string *AppConfig::getDaemonsNames() const
{
    return this->daemonsNames;
}

const string &AppConfig::getHttpConfigPath() const
{
    return this->httpConfigPath;
}

void AppConfig::setHttpConfigPath(const string &httpConfigPath)
{
    this->httpConfigPath = httpConfigPath;
}

const string &AppConfig::getRs232ConfigPath() const
{
    return this->rs232ConfigPath;
}

void AppConfig::setRs232ConfigPath(const string &rs232ConfigPath)
{
    this->rs232ConfigPath = rs232ConfigPath;
}

const string &AppConfig::getOffsetDirPath() const
{
    return this->offsetDirPath;
}

void AppConfig::setOffsetDirPath(const string &offsetDirPath)
{
    this->offsetDirPath = offsetDirPath;
}

const string &AppConfig::getConfigDirPath() const
{
    return this->configDirPath;
}

void AppConfig::setConfigDirPath(const string &configDirPath)
{
    this->configDirPath = configDirPath;
}

const string &AppConfig::getDataDirPath() const
{
    return this->dataDirPath;
}

void AppConfig::setDataDirPath(const string &dataDirPath)
{
    this->dataDirPath = dataDirPath;
}

AppConfig::~AppConfig()
{
    delete[] this->daemonsNames;
}