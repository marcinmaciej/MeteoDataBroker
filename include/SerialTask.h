#ifndef RS232TASK_H
#define RS232TASK_H

#define TESTING true
#define RASPBERRYPI false

#include "Task.h"

#include "BufferManager.h"


class Rs232Task : public Task
{

public:
    explicit Rs232Task(const ConfigManager *configManager);

    ~Rs232Task() override;

    void task(const char *logName) override;


private:

    BufferManager *bufferManager;

    const char *serialPortPath;

    int fileDescriptor,
            bytesAvailable;

    char *buffer;

    const size_t *bytesNumToRead;

    map<const string, int> mapDataBits;
    map<const string, speed_t> mapSpeed;

    struct termios options;

    const char *getSerialPortPath() const;

    void setSerialPortPath(const char *serialPortPath);

public:
    const size_t getBytesNumToRead() const;

    void setBytesNumToRead(const size_t *bytesNumToRead);

private:

    const ConfigManager *getConfigManager() const;

    int getFileDesc();

    void setFileDesc(int fd);

    void readPort();

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
