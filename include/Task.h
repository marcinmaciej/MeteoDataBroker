/**
 * @author  Created by Marcin Guziołek on 06.04.18.
 */

#ifndef TASK_H
#define TASK_H



#include "MyLog.h"
#include "ConfigManager.h"


class Task {

public:
    explicit Task(const ConfigManager &configManager);

    virtual ~Task() = default;


    virtual void task() = 0;

protected:
    MyLog meteoLog;
    const ConfigManager &configManager;
    bool isExecuted = false;
};

#endif // TASK_H
