#include "Rs232Task.h"

Rs232Task::Rs232Task():Task()
{
    Task::logName = "MS-RS232TaskClass";

    Task::meteoLog = new MeteoLog("MS-RS232TaskClass");

    this->buffer = new string();

}

void Rs232Task::task(const char * logName)
{
    try
    {
        Task::logName = logName;

        auto * readData = new string();

        this->bufferManager = new BufferManager(O_WRONLY | O_CREAT | O_APPEND);

        /* Read from port */
        this->readPort();

        *readData = this->buffer->c_str();

        if(!readData->empty())
        {
            /* Write to buffer before sending */
            this->bufferManager->writeBuffer(readData->c_str());
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

void Rs232Task::readPort()
{
    try
    {


        /* Open port and set port configuration to read */
        this->setPortConfig();

        this->buffer->clear();

        if(read(this->getFileDesc(), const_cast<char *>(this->buffer->c_str()), static_cast<size_t>(this->BYTES_TO_READ)) <  0)
        {
            throw errno;
        }

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

void Rs232Task::closePort()
{
    close(this->fileDescriptor);
}

int Rs232Task::getFileDesc()
{
    return this->fileDescriptor;
}

void Rs232Task::setFileDesc(int fd)
{
    this->fileDescriptor = fd;
}

void Rs232Task::setPortConfig()
{
    try
    {

        /* Open port for blocking reading */
        this->setFileDesc(open(this->PATH, O_RDONLY | O_NOCTTY | O_NDELAY)); /* Add O_NDELAY or O_NONBLOCK for nonblocking mode */


        if(this->getFileDesc() < 0)
        {
            throw errno;
        }
        else
        {
            /* Get current settings for port */
            tcgetattr(this->getFileDesc(), &this->options);

            /* Set baud rate */
            cfsetispeed(&this->options, B9600);

            /* Set control options by modifying c_cflag to typical frame: 8n1 */
            this->options.c_cflag &= ~(PARENB | CSTOPB | CSIZE | CRTSCTS);
            this->options.c_cflag |= CREAD;
            this->options.c_cflag |= CS8;
            this->options.c_cflag |= CLOCAL;

            /* Set local options by modifying c_lflag to raw input */
            this->options.c_lflag &= ~(ICANON | ECHO | ISIG | IEXTEN);

            /* Disable writing */
            this->options.c_oflag &= ~OPOST;

            /* Set input options by modifying c_iflag by turning off mapping of characters */
            this->options.c_iflag &= ~(INPCK | INLCR |  ICRNL | IXOFF);
            this->options.c_iflag |= IUTF8;
            this->options.c_iflag |= IGNBRK;

            /* Set blocking read until MIN number bytes are available and returns up to number of bytes requested */
            this->options.c_cc[VMIN] = static_cast<cc_t>(this->BYTES_TO_READ);
            this->options.c_cc[VTIME] = 0;


            /* Set port settings modified immediately */
            tcsetattr(this->getFileDesc(), TCSANOW, &this->options);


            /* Set non blocking reading from port */
            // fcntl(this->getFileDesc(), F_SETFL, FNDELAY);

        }

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

Rs232Task::~Rs232Task()
{
    this->closePort();

    this->meteoLog->err("BYE BYE");

    delete this->buffer;
    delete this->bufferManager;
    delete this->meteoLog;
}
