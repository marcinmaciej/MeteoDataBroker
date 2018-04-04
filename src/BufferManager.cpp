#include "BufferManager.h"

BufferManager::BufferManager(int mode)
{
    try
    {
        this->meteoLog = new MeteoLog("MS-BufferManagerClass");

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

void BufferManager::writeBuffer(const char *data)
{
    try
    {
        ssize_t numWritten;

        size_t numToWrite;

        auto *temp = new string(data);

        *temp += '\n';

        numToWrite = temp->length();

#if TESTING
        /* TESTING BLOCK */
        auto * ms = new string("writeBuffer(bytes to write) = ");
        *ms += to_string(numToWrite);
        this->meteoLog->err(ms->c_str());
        *ms = "Content to write = ";
        *ms += *temp;
        this->meteoLog->warn(ms->c_str());
        /*--------------------------------------------------------------------------------*/
#endif // TESTING

        numWritten = write(this->getBuffFileDesc(), temp->c_str(), numToWrite);

#if TESTING
        /* TESTING BLOCK */
        *ms = ("writeBuffer(bytes written) = ");
        *ms += to_string(numWritten);
        this->meteoLog->err(ms->c_str());
        *ms = "Content written = ";
        *ms += *temp;
        this->meteoLog->warn(ms->c_str());
        delete ms;
        /*--------------------------------------------------------------------------------*/
#endif // TESTING

        if (numWritten != numToWrite)
        {
            // throw "Write error! No all bytes written!";
            throw errno;
        }

        close(this->getBuffFileDesc());

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

void BufferManager::readBuffer()
{
    try
    {

        this->moveOffset();

        this->buffer->clear();

        if (read(this->getBuffFileDesc(), const_cast<char *>(this->buffer->c_str()), this->BYTES_TO_READ) < 0)
        {
            throw errno;
        }

#if TESTING
        /* TESTING BLOCK */
        auto * ms = new string("From buffer should read = 10 bytes and read = ");
        *ms += to_string(strlen(this->buffer->c_str()));
        this->meteoLog->warn(ms->c_str());
        delete ms;

        auto * ms2 = new string("Buffer content is = ");
        *ms2 += this->buffer->c_str();
        this->meteoLog->err(ms2->c_str());
        delete ms2;
        /*------------------------------------------------------------------------------------*/
#endif // TESTING

        close(this->getBuffFileDesc());

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

void BufferManager::openBuffer(int mode)
{

    /* open buffer file to write  appending new data */
    this->setBuffFileDesc(open(this->getBufferFilePath(), mode, 0666));

    if (this->getBuffFileDesc() < 0)
    {
        throw errno;
    }
}

/* GETTERS */

const char * BufferManager::getBuffer()
{
    return this->buffer->c_str();
}

int BufferManager::getBuffFileDesc()
{
    return this->buffFileDesc;
}

int BufferManager::getOffsetFileDesc()
{
    return this->offsetFileDesc;
}
/*
string BufferManager::getCurrentDate()
{
    return this->currentDate;
}
*/
const char *BufferManager::getBufferFilePath()
{
   /* return this->buffer_path; */
    return this->BUFFER_DIR;
}

const char *BufferManager::getOffsetFilePath()
{
    return this->OFFSET_PATH;
}

off_t BufferManager::getOffset()
{
    return this->seekOffset;
}


/* SETTERS */


void BufferManager::setBuffFileDesc(int fd)
{
    this->buffFileDesc = fd;
}
/*
void BufferManager::setBuffFilePath()
{

    this->setCurrentDate();

    string path = this->BUFFER_DIR;
    path += this->getCurrentDate();
    path += this->BUFFER_FILE_NAME;

    strcpy(this->buffer_path, path.c_str());

}

void BufferManager::setCurrentDate()
{

    time_t t = time(nullptr);   // get time now
    struct tm *now = localtime(&t);

    this->currentDate = to_string(now->tm_mday);
    this->currentDate += "-";
    this->currentDate += to_string(now->tm_mon + 1);
    this->currentDate += "-";
    this->currentDate += to_string(now->tm_year + 1900);

}
*/
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

#if TESTING
    /* TESTING BLOCK */
    string tm = "offset loaded = ";
    tm += buf;
    this->meteoLog->err(tm.c_str());
    /*-----------------------------------------------------------*/
#endif // TESTING

    this->setOffset(atoi(buf));

}

void BufferManager::openOffsetFile(int mode)
{

    /* open buffer file to write  appending new data */
    this->setOffsetFileDesc(open(this->getOffsetFilePath(), mode, 0666));

    if (this->getOffsetFileDesc() < 0)
    {
        throw errno;
    }

}

void BufferManager::setOffset(off_t offset)
{
    this->seekOffset = offset;
}

void BufferManager::moveOffset()
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
    off_t o = (this->getOffset() + offset + 1);

    this->saveOffset(o);
}

void BufferManager::saveOffset(off_t offset)
{
    /*
     *   Type off_t is a type used for measure the file offset in bytes from the beginning of the file.
     *   Usually it is signed 32-bit integer but can be also signed 64-bit integer. Defined in the "sys/types".
     */
    string offsetAsString = to_string(offset);
    auto len = static_cast<size_t>(offsetAsString.length());

#if TESTING
    /* TESTING BLOCK */
    auto * ms = new string("Offset to save = ");
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

BufferManager::~BufferManager()
{
    this->meteoLog->err("BYE BYE");

    delete this->buffer;
    delete this->meteoLog;
    /* delete this->buffer_path;  */
}
