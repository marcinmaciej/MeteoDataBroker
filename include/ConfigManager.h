/**
 * @author  Created by Marcin Guziołek on 06.04.18.
 */

#ifndef METEOSTATIONDRIVERS_CONFIGMANAGER_H
#define METEOSTATIONDRIVERS_CONFIGMANAGER_H


#include "MyLog.h"
#include "AppConfig.h"
#include "SerialConfig.h"
#include "HttpConfig.h"
#include <map>
#include <iterator>
#include <fcntl.h>
#include <sys/stat.h>

class ConfigManager
{
public:
    ConfigManager();

    virtual ~ConfigManager();

    AppConfig *programConfig;

    SerialConfig *serialConfig;

    HttpConfig *httpConfig;

private:

    map<const string, void (*)(const ConfigManager *, const string)> mapParamToMethod;

    MyLog *meteoLog;

    const char COMMA = ',',
            EQUALSIGN = '=';


    const char *loadParams(const string &path) const;

    const char *getDefaultSerialToSave();

    const char *getDefaultHttpToSave();

    void makeAllPaths();

    void makeDir(const string & path);

    void saveDefault(const string &path, const char *config);

    bool loadSerialPort(const string &path);

    bool loadHttp(const string &path);

    bool getParams(const string &path) const;

    void substringParams(const string *params, string::size_type *parStart, const string::size_type *parEnd, string *paramValue,
                         string *paramName) const;
};


#endif //METEOSTATIONDRIVERS_CONFIGMANAGER_H
