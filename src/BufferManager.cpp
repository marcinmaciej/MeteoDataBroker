/**
 * @author  Created by Marcin Guziołek on 06.04.18.
 */

#include "BufferManager.h"

BufferManager::BufferManager(const int mode, const ConfigManager *configManager)
{
    try
    {

        this->meteoLog = new MyLog("MS-BufferManagerClass");

        this->setBytesNumToRead(configManager->httpConfig->getDataLength());

        this->setOffsetFilePath(configManager->programConfig->getOffsetFilePath().c_str());

        this->setBufferFilePath(configManager->programConfig->getDataFilePath().c_str());


        if (mode == O_RDONLY)
        {
            this->loadOffset();
        }

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

size_t BufferManager::getBytesNumToRead() const
{
    return this->bytesNumToRead;
}

void BufferManager::setBytesNumToRead(const size_t bytesNumToRead)
{
    this->bytesNumToRead = bytesNumToRead;
}

void BufferManager::writeBuffer(const char *data)
{
    try
    {
        ssize_t numWritten;

        size_t numToWrite;

        numToWrite = strlen(data);

#if SHOW_BUFFER
        /* TESTING BLOCK */
        auto *ms = new string("writeBuffer(bytes to write) = ");
        *ms += to_string(numToWrite);
        this->meteoLog->err(ms->c_str());
        *ms = "Content to write = ";
        *ms += data;
        this->meteoLog->warn(ms->c_str());
        /*--------------------------------------------------------------------------------*/
#endif // SHOW_BUFFER

        numWritten = write(this->getBuffFileDesc(), data, numToWrite);

        close(this->getBuffFileDesc());

#if SHOW_BUFFER
        /* TESTING BLOCK */
        *ms = ("writeBuffer(bytes written) = ");
        *ms += to_string(numWritten);
        this->meteoLog->err(ms->c_str());
        *ms = "Content written = ";
        *ms += data;
        this->meteoLog->warn(ms->c_str());
        delete ms;
        /*--------------------------------------------------------------------------------*/
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

const char *BufferManager::readBuffer()
{
    try
    {

        ssize_t bytesRead = 0;
        size_t len = this->getBytesNumToRead();
        char *buf = new char[len + 1]();

        this->moveOffset();


        bytesRead = read(this->getBuffFileDesc(), buf, len);

        close(this->getBuffFileDesc());

        if (bytesRead < 0)
        {
            throw errno;
        }

        return buf;

    }
    catch (...)
    {
        this->meteoLog->err(strerror(errno));
    }
}

void BufferManager::openBuffer(int mode)
{

    /* open buffer file to write  appending new data */
    this->setBuffFileDesc(open(this->getBufferFilePath(), mode, 0777));

    if (this->getBuffFileDesc() < 0)
    {
        throw errno;
    }
}

int BufferManager::getBuffFileDesc() const
{
    return this->buffFileDesc;
}

int BufferManager::getOffsetFileDesc() const
{
    return this->offsetFileDesc;
}

const char *BufferManager::getBufferFilePath() const
{
    return this->bufferPath;
}

const char *BufferManager::getOffsetFilePath() const
{
    return this->offsetPath;
}


void BufferManager::setOffsetFilePath(const char *offsetPath)
{
    this->offsetPath = offsetPath;
}

void BufferManager::setBufferFilePath(const char *bufferPath)
{
    this->bufferPath = bufferPath;
}

off_t BufferManager::getOffset() const
{
    return this->seekOffset;
}

void BufferManager::setBuffFileDesc(int fd)
{
    this->buffFileDesc = fd;
}

void BufferManager::setOffsetFileDesc(int fd)
{
    this->offsetFileDesc = fd;
}

void BufferManager::loadOffset()
{
    char buf[32] = {0};
    auto len = static_cast<size_t>(sizeof(buf));

    this->openOffsetFile(O_RDONLY);

    if (read(this->getOffsetFileDesc(), buf, len) < 0)
    {
        throw errno;
    }

    close(this->getOffsetFileDesc());

#if TESTING
    /* TESTING BLOCK */
    string tm = "offset loaded = ";
    tm += buf;
    this->meteoLog->err(tm.c_str());
    /*-----------------------------------------------------------*/
#endif // TESTING

    this->setOffset(stoi(buf));

}

void BufferManager::openOffsetFile(int mode)
{

    /* open offset file to write  or read, depends on need */
    this->setOffsetFileDesc(open(this->getOffsetFilePath(), mode, 0666));

    /* Create offset file if not created yet and fill with value 0 */
    if (this->getOffsetFileDesc() < 0)
    {
        if (errno == ENOENT)
        {
            this->setOffsetFileDesc(open(this->getOffsetFilePath(), O_CREAT | mode));
            if (this->getOffsetFileDesc() < 0)
            {
                throw errno;
            }

            char *buf = new char{'0'};

            ssize_t bytesWritten = write(this->getOffsetFileDesc(), buf, 1);

            if (bytesWritten != 1)
            {
                throw errno;
            }

        }
        else
        {
            throw errno;
        }
    }

}

void BufferManager::setOffset(off_t offset)
{
    this->seekOffset = offset;
}

void BufferManager::moveOffset() const
{
    if (this->getOffset() > 0)
    {
        if (lseek(this->getBuffFileDesc(), this->getOffset(), SEEK_SET) < 0)
        {
            throw errno;
        }
    }
}

void BufferManager::updateOffset(off_t offset)
{
    off_t o = (this->getOffset() + offset);

    this->saveOffset(o);
}

void BufferManager::saveOffset(off_t offset)
{
    /*
     *   Type off_t is a type used for measure the file offset in bytes from the beginning of the file.
     *   Usually it is signed 32-bit integer but can be also signed 64-bit integer. Defined in the "sys/types".
     */
    string offsetAsString = to_string(offset);
    size_t len = offsetAsString.length();

#if TESTING
    /* TESTING BLOCK */
    auto *ms = new string("Offset to save = ");
    *ms += offsetAsString;
    this->meteoLog->err(ms->c_str());
    delete ms;
    /*-------------------------------------------------------------------------------*/
#endif // TESTING

    this->openOffsetFile(O_WRONLY);

    if (write(this->getOffsetFileDesc(), offsetAsString.c_str(), len) < 0)
    {
        throw errno;
    }

    close(this->getOffsetFileDesc());
}

void BufferManager::closeBuffer()
{
    close(this->getBuffFileDesc());
}

BufferManager::~BufferManager()
{
    this->meteoLog->err("BYE BYE");

    delete this->meteoLog;
}
