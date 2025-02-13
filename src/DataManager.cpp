/**
 * @author  Created by Marcin Guziołek on 06.04.18.
 */

#include "BufferManager.h"


BufferManager::BufferManager(const int mode, const ConfigManager *configManager)
{
    try
    {

        this->meteoLog = new MyLog("MS-BufferManager");

        this->setBufferFilePath(configManager->appConfig->getDataFilePath().c_str());

        this->openBuffer(mode);


    }
    catch (const char *s)
    {
        this->meteoLog->err(s);
    }
    catch (...)
    {
        this->meteoLog->err(strerror(errno));
    }
}

void BufferManager::writeBuffer(const char *data)
{
    try
    {
        ssize_t numWritten;

        size_t numToWrite;

        numToWrite = strlen(data);

// TESTING BLOCK
#if SHOW_BUFFER
string ms = "writeBuffer(bytes to write) = ";
ms += to_string(numToWrite);
this->meteoLog->err(ms.c_str());
ms = "Content to write = ";
ms += data;
this->meteoLog->err(ms.c_str());
this->meteoLog->err("-----");
this->meteoLog->warn(to_string(this->getBuffFileDesc()).c_str());
this->meteoLog->err("-----");
ms.clear();
#endif // SHOW_BUFFER


        numWritten = write(this->getBuffFileDesc(), data, numToWrite);



// TESTING BLOCK
#if SHOW_BUFFER
ms = ("writeBuffer(bytes written) = ");
ms += to_string(numWritten);
this->meteoLog->err(ms.c_str());
ms = "Content written = ";
auto mms = new string(data);
this->meteoLog->warn(mms->c_str());
ms.clear();
#endif // SHOW_BUFFER


        if (numWritten != numToWrite)
        {
            throw errno;
        }

    }
    catch (...)
    {
        this->meteoLog->err(strerror(errno));
    }
}

void BufferManager::openBuffer(int mode)
{

    /* open buffer file to write  appending new data */
    int fd = open(this->getBufferFilePath(), mode, 0777);
    this->setBuffFileDesc(fd);

#if SHOW_BUFFER
this->meteoLog->err("........");
this->meteoLog->err(to_string(fd).c_str());
this->meteoLog->err("........");
#endif

    if (this->getBuffFileDesc() < 0)
    {
        throw errno;
    }
}

int BufferManager::getBuffFileDesc() const
{
    return this->buffFileDesc;
}

const char *BufferManager::getBufferFilePath() const
{
    return this->bufferPath.c_str();
}

void BufferManager::setBufferFilePath(const char *bufferPath)
{
    this->bufferPath = bufferPath;
}

void BufferManager::setBuffFileDesc(int fd)
{
    this->buffFileDesc = fd;
}

void BufferManager::closeBuffer()
{
    close(this->getBuffFileDesc());
}

BufferManager::~BufferManager()
{
    this->closeBuffer();
    delete this->meteoLog;
}

BufferManager::BufferManager(BufferManager &bufferManager)
{
    this->meteoLog = new MyLog("MS-BufferManagerClass");
    this->meteoLog = bufferManager.meteoLog;
}
