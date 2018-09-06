/**
 * @author  Created by Marcin Guziołek on 06.04.18.
 */

#include "Task.h"

Task::Task(const ConfigManager * configManager)
{
    this->configManager = configManager;
    this->logName = new char[1024];
    this->logName = this->configManager->programConfig->getAppName().c_str();
    this->meteoLog = new MyLog(this->logName);

}

