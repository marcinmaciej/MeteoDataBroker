/**
 * @author  Created by Marcin Guziołek on 06.04.18.
 */

#ifndef _BUFFERMANAGER_H
#define _BUFFERMANAGER_H

#include "MyLog.h"
#include "ConfigManager.h"


class BufferManager
{

public:

    explicit BufferManager(const int mode, const ConfigManager *configManager);

    // Copying constructor
    BufferManager(BufferManager & bufferManager);

    virtual ~BufferManager();

    void writeBuffer(const char *data);

private:

    string bufferPath;

    MyLog *meteoLog;

    int buffFileDesc;

    void closeBuffer();

    int getBuffFileDesc() const;

    const char *getBufferFilePath() const;

    void setBufferFilePath(const char *bufferPath);

    void setBuffFileDesc(int fd);

    void openBuffer(int mode);

};

#endif
