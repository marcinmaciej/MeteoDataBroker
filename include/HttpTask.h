#ifndef HTTPTASK_H
#define HTTPTASK_H

#define TESTING true

#include "Task.h"

#include "HttpConnection.h"
#include "BufferManager.h"


class HttpTask: public Task
{

public:
    HttpTask();
    ~HttpTask() override;

    void task(const char * logName) override;

private:
    HttpConnection * http;
    BufferManager * bufferManager;

    const int BYTES_TO_READ = 10;

};

#endif // HTTPTASK_H
