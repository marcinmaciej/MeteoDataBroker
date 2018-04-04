#include "HttpTask.h"

HttpTask::HttpTask():Task()
{

    this->http = new HttpConnection();
    Task::meteoLog = new MeteoLog("MS-HttpdTaskClass");

}

void HttpTask::task(const char * logName)
{
    Task::logName = logName;

    size_t readBytesNum = 0;
    auto * readData = new string();

    this->bufferManager = new BufferManager(O_RDONLY);

    try
    {
        /* Connect to the server */
        this->http->httpConnect();

        /* Read data from buffer file */
        this->bufferManager->readBuffer();

        /* Get data from buffer */
        *readData = this->bufferManager->getBuffer();

        /* Get number of bytes to change reading buffer offset */
        readBytesNum = readData->length();

        if(!readData->empty())
        {
            if(readBytesNum == this->BYTES_TO_READ)
            {
            /* Send data to the server */
                if(this->http->sendData(readData->c_str()))
                {

                    /* Update seek offset if data sent */
                    this->bufferManager->updateOffset(readBytesNum);
                }

            }
        }

        delete readData;

    }
    catch(const char * s)
    {
        this->meteoLog->err(s);
    }
    catch(...)
    {
        this->meteoLog->err(strerror(errno));
    }
}

HttpTask::~HttpTask()
{
    this->meteoLog->err("BYE BYE");

    delete this->http;
    delete this->meteoLog;
}
