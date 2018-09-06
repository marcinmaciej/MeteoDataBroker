/**
 * @author  Created by Marcin Guziołek on 06.04.18.
 */

#ifndef RS232TASK_H
#define RS232TASK_H

#define SHOW_SERIAL false
#define RASPBERRYPI false

#include "Task.h"

#include "BufferManager.h"


class SerialTask : public Task
{

public:
    explicit SerialTask(const ConfigManager *configManager);

    ~SerialTask() override;

    void task(const char *logName) override;


private:

    BufferManager *bufferManager;

    const char *serialPortPath;

    int fileDescriptor,
            bytesAvailable;

    char *buffer;

    size_t bytesNumToRead;

    map<const string, int> mapDataBits;
    map<const string, speed_t> mapSpeed;

    struct termios options;

    const char *getSerialPortPath() const;

    void setSerialPortPath(const char *serialPortPath);

    const size_t getBytesNumToRead() const;

    void setBytesNumToRead(const size_t bytesNumToRead);

    const ConfigManager *getConfigManager() const;

    int getFileDesc();

    void setFileDesc(int fd);

    const char * readPort();

    void setPortConfig();

    void closePort();

    void setPortStopBits();

    void setPortParity();

    void setPortFlowControl();

    void setPortDataBits();

    void setPortRawInput();

    void setPortNoMappingChars();

    void setPortReadMin();

    void setPortSpeed();

    void openPort();
};

#endif // RS232TASK_H
