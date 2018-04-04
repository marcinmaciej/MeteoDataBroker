#ifndef _BUFFERMANAGER_H
#define _BUFFERMANAGER_H

#include <fcntl.h>
#include <ctime>

#include "MeteoLog.h"

class BufferManager
{

public:

    explicit BufferManager(int mode);

    virtual ~BufferManager();

    const char * getBuffer();

    void readBuffer();

    void writeBuffer(const char * data);

    void updateOffset(off_t offset);


private:

    const char * BUFFER_DIR =  "/var/meteo-data/",
                 * BUFFER_FILE_NAME =  "meteo.data",
                   * OFFSET_PATH=  "/etc/meteo-station/offset/seek.offset";

    char * buffer_path = new char[2048];;

    string * buffer = new string();

    string currentDate;

    const size_t BYTES_TO_READ = 10;

    int buffFileDesc,
        offsetFileDesc;

    off_t seekOffset;

    MeteoLog * meteoLog;

    /* Private methods */

    /* Getters for buffer */
    /* string getCurrentDate(); */
    int getBuffFileDesc();
    const char * getBufferFilePath();

    /* Getters for seek offset */
    off_t getOffset();
    int getOffsetFileDesc();
    const char * getOffsetFilePath();

    /* Setters for buffer */
    /* void setCurrentDate(); */
    void setBuffFilePath();
    void setBuffFileDesc(int fd);
    void openBuffer(int mode);

    /* Setters for seek offset */
    void setOffsetFileDesc(int fd);
    void loadOffset();
    void moveOffset();
    void setOffset(off_t offset);
    void saveOffset(off_t offset);
    void openOffsetFile(int mode);

};
#endif
