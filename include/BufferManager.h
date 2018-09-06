/**
 * @author  Created by Marcin Guziołek on 06.04.18.
 */

#ifndef _BUFFERMANAGER_H
#define _BUFFERMANAGER_H

#define SHOW_BUFFER false


#include "MyLog.h"
#include "ConfigManager.h"


class BufferManager
{

public:

    explicit BufferManager(const int mode, const ConfigManager *configManager);

    virtual ~BufferManager();

    const char *readBuffer() ;

    void writeBuffer(const char *data);

    void updateOffset(off_t offset);

    void closeBuffer();

private:

    const char *offsetPath,
            *bufferPath;

    size_t bytesNumToRead;

    int buffFileDesc,
            offsetFileDesc;

    off_t seekOffset;

    MyLog *meteoLog;


    size_t getBytesNumToRead() const;

    int getBuffFileDesc() const;

    const char *getBufferFilePath() const;

    off_t getOffset() const;

    int getOffsetFileDesc() const;

    const char *getOffsetFilePath() const;

    void setOffsetFilePath(const char *offsetPath);

    void setBufferFilePath(const char *bufferPath);

    void setBytesNumToRead(size_t bytesNumToRead);

    void setOffsetFileDesc(int fd);

    void setBuffFileDesc(int fd);

    void openBuffer(int mode);

    void loadOffset();

    void moveOffset() const;

    void setOffset(off_t offset);

    void saveOffset(off_t offset);

    void openOffsetFile(int mode);

};

#endif
