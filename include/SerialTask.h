/**
 * @author  Created by Marcin Guziołek on 10.12.21.
 */

#ifndef RS232TASK_H
#define RS232TASK_H


#include <termios.h> /* struct termios */
#include <sys/ioctl.h> /* ioctl(), FIONREAD */
#include <fcntl.h> /* open(), O_RDONLY, O_NOCTTY, O_NONBLOCK */
#include <csignal> /* sigemptyset(), SIGPIPE, SIG_SETMASK */
#include <cerrno>   /* errno */
#include <map> /* std::map */
#include <unistd.h> /* sleep(), */
#include <ctime> /* time(), ctime() , typ time_t */

#include "Task.h"
#include "DataManager.h"


extern std::string toLowerCase(const std::string &pstr, MyLog &meteoLog);
extern void
restartOnBrokenPipe(const ConfigManager &configManager, MyLog &meteoLog, int whichDaemon, std::string &msgTitle);


class SerialTask : public Task {

public:
    explicit SerialTask(const ConfigManager & configManager, int pipeDesc);

    SerialTask(const SerialTask &serialTask);

    ~SerialTask() override;

    void task() override;


private:

    DataManager *bufferManager;

    std::string serialPortPath;

    int serialFileDescriptor = -1,
        PIPEDESC;

    size_t bytesNumToRead = 0;

    std::map<const string, unsigned int> mapDataBits;
    std::map<const string, speed_t> mapSpeed;

    struct termios options{};

    void writePipe(const std::string &);

    const std::string &getSerialPortPath() const;

    void setSerialPortPath(const std::string &pserialPortPath);

    size_t getBytesNumToRead() const;

    void setBytesNumToRead(size_t pbytesNumToRead);

    const ConfigManager & getConfigManager() const;

    int getSerialFileDesc() const;

    void setSerialFileDesc(int fd);

    std::string * readSerialPort();

    void setSerialPortConfig();

    void closeSerialPort() const;

    void setSerialPortStopBits();

    void setSerialPortParity();

    void setSerialPortFlowControl();

    void setSerialPortDataBits();

    void setSerialPortRawInput();

    void setSerialPortNoMappingChars();

    void setSerialPortReadMin();

    void setSerialPortSpeed();

    void openSerialPort();

    void messageAndSleep(int saved_errno,std::string &msgTitle);

};

#endif // RS232TASK_H
