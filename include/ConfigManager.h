//
// Created by marcin on 06.04.18.
//

#ifndef METEOSTATIONDRIVERS_CONFIGMANAGER_H
#define METEOSTATIONDRIVERS_CONFIGMANAGER_H

#include "ProgramConfig.h"
#include "SerialPort.h"
#include "ConnectionConfig.h"
#include "MeteoLog.h"
#include <map>
#include <iterator>
#include <algorithm>


class ConfigManager
{
public:
    explicit ConfigManager();

    virtual ~ConfigManager();

    ProgramConfig *programConfig;

    SerialPort *serialPort;

    ConnectionConfig *connectionConfig;

private:

    map<string, void (*)(ConfigManager &, const string &)> setParams;

    MeteoLog *meteoLog;

    const char COMMA = ',',
            ASSIGNMENT = '=';

    const char *ENDDELIMITER = "\r\n";

    const char *loadParams(const string &path) const;

    const char *getDefaultSerialToSave();

    const char *getDefaultHttpToSave();

    void setDefaultSerial();

    void setDefaultHttp();

    void saveDefaultSerial();

    void saveDefaultHttp();

    bool loadSerialPort(const string &path);

    bool loadHttp(const string &path);

    bool getParams(const string &path) const;

    void substringParams(string *params, string::size_type *bPos, const string::size_type *ePos, string *paramValue,
                         string *paramName) const;
};


#endif //METEOSTATIONDRIVERS_CONFIGMANAGER_H
