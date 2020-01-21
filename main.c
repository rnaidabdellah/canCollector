/*!
   \internal
   \file          daemonInterface.c
   \brief         AGV
   \implementation
   copyright   LeclanchÃ©
   license
   created     07.01.2019 Rachid Naid-Abdellah
   \endimplementation
   \internal
   \history
   Date(dd.mm.yyyy)  Author                  Description
   07.01.2019        Rachid Naid-Abdellah    file created
   \endhistory
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <signal.h>
#include <getopt.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <time.h>
#include <string.h>


#include "types.h"
#include "definitions.h"
#include "can.h"
#include "service.h"
#include "global.h"
#include "tasks.h"
#include "canConverter.h"
#include "appConfig.h"



/* -- Defines ------------------------------------------------------------- */


#define DELAY_US (5000)

static int  running = 0;
static char *sys_log_name = NULL;
//char info_log_name[100];
static char *pid_file_name = NULL;
static int   pid_fd = -1;
static char *app_name = NULL;

//
AGV_DUALBATTERY agv_dualstring;


int test_log();


/* Main function */
int main(int argc, char *argv[])
{
    T_can_msg t_CAN_msg;
    time_t now = time(NULL);
    char buff[20];
    signed long retCan;
    bbool isConfigFileValid;
    int r;

    //time (&rawtime);
	app_name = argv[0];
	strftime(buff, 20, "%Y-%m-%d %H:%M:%S", localtime(&now));
	//sprintf(info_log_name,"./agv/agvappLogFile.log");


	/* Open system log and write message to it */
	openlog(argv[0], LOG_PID|LOG_CONS, LOG_DAEMON);
	syslog(LOG_INFO, "Started %s", app_name);

	/* Initialization */
	isConfigFileValid = readAppConfigFile();
    retCan = ms32_InitCAN(0); //initialize can0
    task_initialize(); //Init System Variables

    //
    syslog(LOG_INFO, "Open CAN status %ld", retCan);

    //Open csv file
    svc_open_csv_stream();




	/* This global variable can be changed in function handling signal */
	running = 1;
	running = (isConfigFileValid == EXIT_SUCCESS);

	/* Never ending loop of server */
	while (running == 1) {

		/* TODO: dome something useful here */
        {
            //ALL_TASKS_MODE[TASK_RXBMU_ID].isOn = 0;
            retCan = gs32_CAN_RecvMsg (&ms16_SockFDCAN_1, &t_CAN_msg);
            if (retCan == CAN_NO_ERR)
            {
                //mv_can2claytonbmu (&t_CAN_msg);
                agv_dualstring.xOffLine = 0;
                can2data (&t_CAN_msg);

            }
            else
            {
                agv_dualstring.xOffLine = retCan;
            }
        }
        task_run_enabled_tasks();
        //
        usleep(DELAY_US);
	}

	/* Write system log and close it. */
	syslog(LOG_INFO, "Stopped %s", app_name);

	/* Clean up */
	closelog();

	/* Free allocated memory */
	if (sys_log_name != NULL)  free(sys_log_name);
	if (pid_file_name != NULL) free(pid_file_name);

	return EXIT_SUCCESS;
}

