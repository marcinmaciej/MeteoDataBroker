/**
 * @author  Created by Marcin Guziołek on 06.04.18.
 */

#include "HttpTask.h"

HttpTask::HttpTask(const ConfigManager *configManager) : Task(configManager)
{

    Task::logName = "MS-HttpdTaskClass";

    this->meteoLog = new MyLog("MS-HttpTaskClass");

    this->http = new HttpConnection(this->getConfigManager());

    this->setBytesToRead(this->getConfigManager()->httpConfig->getDataLength());
}

void HttpTask::task(const char *logName)
{

    Task::logName = logName;

    /* Connect to the server */
    if (this->http->httpConnect())
    {
        size_t readBytesNum = 0;
        auto *readData = new string();

        this->bufferManager = new BufferManager(O_RDONLY, this->getConfigManager());

        /* Read data from buffer file */
        *readData = this->bufferManager->readBuffer();

        /* Get number of data length */
        readBytesNum = readData->length();

#if SHOW_HTTP
        auto *ms = new string("Bytes to send = ");
        *ms += to_string(readBytesNum);
        *ms += " content = ";
        *ms += *readData;
        this->meteoLog->warn(ms->c_str());
        delete ms;
#endif // SHOW_HTTP

        try
        {
            if (!readData->empty())
            {

                if (readBytesNum == this->getBytesToRead())
                {
                    /* Send data to the server */
                    if (this->http->sendData(readData->c_str()))
                    {

                        /* Update seek offset if data sent */
                        this->bufferManager->updateOffset(this->getBytesToRead());
                    }

                }
                else
                {
                    sleep((this->getConfigManager()->httpConfig->getSleepTime()));
                }
            }

            this->http->httpClose();

            delete readData;

            sleep(this->getConfigManager()->httpConfig->getSleepTime());

        }
        catch (...)
        {
            this->meteoLog->err(strerror(errno));
        }
    }
    else
    {
        sleep(this->getConfigManager()->httpConfig->getWaitNetwork());
    }

}

const ConfigManager *HttpTask::getConfigManager()
{
    return this->configManager;
}

const size_t HttpTask::getBytesToRead() const
{
    return this->bytesToRead;
}

void HttpTask::setBytesToRead(const size_t bytesToRead)
{
    this->bytesToRead = bytesToRead;
}

HttpTask::~HttpTask()
{
    this->meteoLog->err("BYE BYE from HttpTaskClass");

    delete this->http;
    delete this->meteoLog;
    delete this->bufferManager;
}