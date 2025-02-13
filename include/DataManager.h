/**
 * @author  Created by Marcin Guziołek on 14.12.21.
 */

#ifndef _BUFFERMANAGER_H
#define _BUFFERMANAGER_H

#include <unistd.h>  /* write(), close(), access(), F_OK */
#include <fcntl.h> /* open(),  O_WRONLY, O_APPEND, O_CREAT*/
#include <string> /* std::string */
#include <ctime> /* ctime(), time(), typ time_t */

#include "MyLog.h"


class DataManager {

public:

    explicit DataManager(std::string datFilePath);

    void saveData(const std::string &data);

private:

    std::string dataFilePath;

    MyLog meteoLog;

    int dataFileDesc = -1;

    int getDataFileDesc() const;

    std::string getDataFilePath() const;

    void setDataFileDesc(int fd);

    void openDataFile();

};

#endif
