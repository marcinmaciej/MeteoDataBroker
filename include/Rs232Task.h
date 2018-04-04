#ifndef RS232TASK_H
#define RS232TASK_H

#include "Task.h"

#include "BufferManager.h"


class Rs232Task:  public Task
{

public:
    Rs232Task();
    ~Rs232Task() override;

    void task(const char * logName) override;

private:

    /* For RASPBERRY PI ONLY */
    const char * PATH = "/dev/ttyAMA0";

    int fileDescriptor,
        bytesAvailable;

string * buffer;

    const int BYTES_TO_READ = 10;

    struct termios options;

    BufferManager * bufferManager;

    int getFileDesc();

    void setFileDesc(int fd);

    void readPort();
    void setPortConfig();
    void closePort();

};

#endif // RS232TASK_H
