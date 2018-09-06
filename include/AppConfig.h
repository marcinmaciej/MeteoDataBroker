/** @author  Created by Marcin Guziołek on 06.04.18.
 *
 */

#ifndef METEOSTATIONDRIVERS_PROGRAMCONFIG_H
#define METEOSTATIONDRIVERS_PROGRAMCONFIG_H

#include <iostream>

using namespace std;

class AppConfig
{
public:
    AppConfig();

    virtual ~AppConfig();

    const string &getAppName() const;

    void setAppName(const string &appName);

    const string &getDataFilePath() const;

    void setDataFilePath(const string &dataFilePath);

    const string &getOffsetFilePath() const;

    void setOffsetFilePath(const string &offsetFilePath);

    const string &getPidFileDirPath() const;

    void setPidFileDirPath(const string &pidFileDirPath);

    const string *getDaemonsNames() const;

    const string &getHttpConfigPath() const;

    void setHttpConfigPath(const string &httpConfigPath);

    const string &getRs232ConfigPath() const;

    void setRs232ConfigPath(const string &rs232ConfigPath);

    const string &getOffsetDirPath() const;

    void setOffsetDirPath(const string &offsetDirPath);

    const string &getConfigDirPath() const;

    void setConfigDirPath(const string &configDirPath);

    const string &getDataDirPath() const;

    void setDataDirPath(const string &dataDirPath);

    void setDaemonsNames(string *daemonsNames);

private:
    string appName,
            dataFilePath,
            offsetFilePath,
            pidFileDirPath,
            *daemonsNames,
            httpConfigPath,
            rs232ConfigPath,
            offsetDirPath,
            configDirPath,
            dataDirPath;

};


#endif //METEOSTATIONDRIVERS_PROGRAMCONFIG_H
