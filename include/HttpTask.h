/**
 * @author  Created by Marcin Guziołek on 06.12.21.
 */

#ifndef HTTPTASK_H
#define HTTPTASK_H

#include <cerrno>   /* errno */
#include <unistd.h> /* read(), sleep(), close(), */
#include <string> /* std::string */


#include "Connection.h"
#include "Task.h"

extern void
restartOnBrokenPipe(const ConfigManager &configManager, MyLog &meteoLog, int whichDaemon, std::string &msgTitle);

class HttpTask : public Task
{

public:

    HttpTask(const ConfigManager &configManager, int pipeDesc);

    ~HttpTask() override;

    void task() override;


private:

    int PIPEDESC;

    Connection http;

    const ConfigManager & getConfigManager() const;

    std::string readPipe();

};

#endif // HTTPTASK_H
