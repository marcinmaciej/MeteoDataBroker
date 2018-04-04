#include "Task.h"

Task::Task()
{
    this->logName = "MS-TaskName";
    this->meteoLog = new MeteoLog(this->logName);
}

