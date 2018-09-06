/**
 * @author  Created by Marcin Guziołek on 06.04.18.
 */

#ifndef HTTPTASK_H
#define HTTPTASK_H

#define SHOW_HTTP true


#include "HttpConnection.h"
#include "BufferManager.h"

#include "Task.h"

class HttpTask: public Task
{

public:
    explicit HttpTask(const ConfigManager * configManager);
    ~HttpTask() override;

    void task(const char * logName) override;


private:

    size_t bytesToRead;

    HttpConnection * http;
    BufferManager * bufferManager;

    const ConfigManager * getConfigManager();

    const size_t getBytesToRead() const;

    void setBytesToRead(size_t bytesToRead);

};

#endif // HTTPTASK_H
