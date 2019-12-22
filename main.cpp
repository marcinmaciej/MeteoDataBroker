/**
 * @author  Created by Marcin Guziołek on 06.04.18.
 */


#include "ConfigManager.h"
#include "DaemonsManager.h"

/* Functions declarations: */
int message(string arg);

int exit_status = EXIT_SUCCESS;

int main(int argc, char *argv[])
{
    /* Constants declarations: */
    const string RESTART = "-r",
            START = "-s",
            KILL = "-k",
            HELP = "help";

    /* Variables declarations: */
    string argument;

    auto *meteoLog = new MyLog("Meteo-daemons-Main");

    auto *configManager = new ConfigManager();

    auto *daemonsManager = new DaemonsManager(configManager);
    /* -------------------------------------- */


    if (argc > 1)
    {

        argument = argv[1];


        if (argument == START)
        {

            try
            {
                cout << "Starting daemons!" << endl;

                daemonsManager->startDaemons();

            }
            catch (...)
            {
                cout<<strerror(errno);
            }

        } else if (argument == RESTART)
        {

            try
            {
                cout << "Restarting daemons!" << endl;

                daemonsManager->restartDaemons();

            }
            catch (...)
            {
                cout<<strerror(errno);
            }

        } else if (argument == KILL)
        {

            try
            {
                cout << "Killing daemons!" << endl;

                daemonsManager->killAll();

            }
            catch (...)
            {
                cout<<strerror(errno);
            }

        }
    }

    delete meteoLog;

    return message(argument);
}

int message(string arg)
{

    if (arg != "-s" && arg != "-r" && arg != "-k")
    {
        cout << "Fail to start app!" << endl;
        exit_status = EXIT_FAILURE;
        cout << "You have to start app with one of below arguments:" << endl;
        cout << "-r    Restarting daemons." << endl;
        cout << "-s    Starting daemons" << endl;
        cout << "-k    Killing daemons" << endl;
    }

    return exit_status;
}
