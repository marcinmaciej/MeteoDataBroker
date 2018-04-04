#ifndef TASK_H
#define TASK_H

#include <termios.h> /* termios structure */
#include <sys/ioctl.h>

#include "MeteoLog.h"

class Task
{
	public:
		Task();
        virtual ~Task() = default;

	virtual void task(const char * logName) = 0;

	protected:
		const char * logName;
		MeteoLog * meteoLog;
};

#endif // TASK_H
