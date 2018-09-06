/**
 * @author  Created by Marcin Guziołek on 06.04.18.
 */

#ifndef TASK_H
#define TASK_H

#include <termios.h> /* termios structure */
#include <sys/ioctl.h>
#include "MyLog.h"
#include "ConfigManager.h"


class Task
{
	public:
		Task(const ConfigManager * configManager);
        virtual ~Task() = default;


		virtual void task(const char * logName) = 0;

	protected:
		const char * logName;
		MyLog * meteoLog;
		const ConfigManager * configManager;
};

#endif // TASK_H
