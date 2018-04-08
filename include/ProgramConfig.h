//
// Created by Marcin Guziołek on 06.04.18.
//

#ifndef METEOSTATIONDRIVERS_PROGRAMCONFIG_H
#define METEOSTATIONDRIVERS_PROGRAMCONFIG_H

#include <iostream>

using namespace std;

class ProgramConfig
{
public:
    explicit ProgramConfig(const unsigned int *sleepTimes = new unsigned int[2]{2, 5});

    virtual ~ProgramConfig();

    const string *getAppName() const;

    const string *getDataFilePath() const;

    const string *getOffsetFilePath() const;

    const string *getPidFileDirPath() const;

    const string *getDaemonsNames() const;

    const unsigned int *getDaemonsSleepTimes() const;

    const string *getHttpConfigPath() const;

    const string *getRs232ConfigPath() const;

    const string *getOffsetDirPath() const;

    const string *getConfigDirPath() const;

    const string *getDataDirPath() const;


    void setHttpConfigPath(const string *httpConfigPath);

    void setRs232ConfigPath(const string *rs232ConfigPath);

    void setDaemonsSleepTimes(const unsigned int *daemonsSleepTimes);

private:
    const string *appName,
            *dataFilePath,
            *offsetFilePath,
            *pidFileDirPath,
            *daemonsNames,
            *httpConfigPath,
            *rs232ConfigPath,
            *offsetDirPath,
            *configDirPath,
            *dataDirPath;

    const unsigned int *daemonsSleepTimes;

};


#endif //METEOSTATIONDRIVERS_PROGRAMCONFIG_H
