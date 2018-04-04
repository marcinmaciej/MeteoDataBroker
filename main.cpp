#include "DaemonsManager.h"

/* Functions declarations: */
int message(string arg);

int exit_status = EXIT_SUCCESS;

int main(int argc, char * argv[])
{
    /* Constants declarations: */
    const string RESTART = "-r",
                 START = "-s",
                 KILL = "-k",
                 HELP = "help";

    /* Paths to data files and offset file, if you change them here need to change in below files */
    const string APP_NAME = "meteo-station",
                 OFFSET_PATH = "/etc/" + APP_NAME + "/offset", /* BufferManager.h */
                 DATA_PATH = "/var/meteo-data"; /* BufferManager.h */

    /* Variables declarations: */
    string argument;

    MeteoLog * meteoLog;

    DaemonsManager * daemonsManager;

    meteoLog = new MeteoLog("Meteo-daemons-Main");

    daemonsManager = new DaemonsManager();

    struct stat st = {0};
    /* -------------------------------------- */

    if(argc>1)
    {

        argument = argv[1];


        if(argument == START)
        {

            try
            {
                /* Check if dir exists */
                if (stat(OFFSET_PATH.c_str(), &st) == -1)
                {
                    /* If not create dir */
                    mkdir(OFFSET_PATH.c_str(), 0666);
                    cout<<OFFSET_PATH<<" created!"<<endl;
                }

                st = {0};

                /* Check if dir exists */
                if (stat(DATA_PATH.c_str(), &st) == -1)
                {
                    /* If not create dir */
                    mkdir(DATA_PATH.c_str(), 0666);

                    cout<<DATA_PATH<<" created!"<<endl;
                }

                cout<<"Starting daemons!"<<endl;

                daemonsManager->startDaemons();

            }
            catch(...)
            {
                meteoLog->err(strerror(errno));
            }

        }
        else if(argument == RESTART)
        {

            try
            {
                cout<<"Restarting daemons!"<<endl;

                daemonsManager->restartDaemons();

            }
            catch(...)
            {
                meteoLog->err(strerror(errno));
            }

        }
        else if(argument == KILL)
        {

            try
            {
                cout<<"Killing daemons!"<<endl;

                daemonsManager->killAll();

            }
            catch(...)
            {
                meteoLog->err(strerror(errno));
            }

        }
    }

    delete daemonsManager;
    delete meteoLog;

    return message(argument);
}

int message(string arg)
{

    if(arg != "-s" && arg != "-r" && arg != "-k")
    {
        cout<<"Fail to start app!"<<endl;
        exit_status = EXIT_FAILURE;
        cout<<"You have to start app with one of below arguments:"<<endl;
        cout<<"-r    Restarting daemons."<<endl;
        cout<<"-s    Starting daemons"<<endl;
        cout<<"-k    Killing daemons"<<endl;
    }

    return exit_status;
}
