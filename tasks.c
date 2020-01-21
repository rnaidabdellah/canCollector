//-----------------------------------------------------------------------------
/*!
   \internal
   \file          tasks.c
   \brief         AGV
   \implementation
   copyright   LeclanchÃ©
   license
   created     07.01.2019 Rachid Naid-Abdellah
   \endimplementation
   \internal
   \history
   Date(dd.mm.yyyy)  Author                  Description
   07.01.2019       Rachid Naid-Abdellah    file created
   \endhistory
*/
//-----------------------------------------------------------------------------/*

/* -- Includes ------------------------------------------------------------ */
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#include <time.h>
#include <sys/time.h>

#include "types.h"
#include "definitions.h"
#include "service.h"
#include "global.h"
#include "tasks.h"



TASK_DATA    ALL_TASKS_MODE[]=
/* index | Counter | Default | Timeout | Supported| isON  */
/*Timeout = Factor -->  ~Factor*sampleTaskTime --> 100 --> 1.1sec by measurement*/
/* Tests show that 500 ~= 5.5seconds   --> 1 -- > 11ms*/
{
		{TASK_ST001UNIT_ID 	,0 ,0 ,1  	,FALSE,FALSE},
		{TASK_ST005UNIT_ID 	,0 ,0 ,5    ,FALSE,FALSE},
		{TASK_ST010UNIT_ID 	,0 ,0 ,10  	,FALSE,FALSE},
		{TASK_ST020UNIT_ID  ,0 ,0 ,20  	,FALSE,FALSE},
		{TASK_ST050UNIT_ID  ,0 ,0 ,50  	,FALSE,FALSE},
		{TASK_ST100UNIT_ID 	,0 ,0 ,100  ,FALSE,FALSE}
};


uint32 u32_msCurrentTime      = 0;
uint32 u32_msLastTime         = 0;

static void task_refresh_timers();
static uint32 get_process_time();


void task_run_enabled_tasks()
{
    task_refresh_timers();

    /*if(ALL_TASKS_MODE[TASK_RXBMU_ID].isOn)
	{
		ALL_TASKS_MODE[TASK_RXBMU_ID].isOn = FALSE;
		if (gs32_CAN_RecvMsg (&ms16_SockFDCAN_1, &t_CAN_msg) == C_NO_ERR)
                mv_can2claytonbmu (&t_CAN_msg);
	}*/

	if(ALL_TASKS_MODE[TASK_ST001UNIT_ID].isOn)
	{
		ALL_TASKS_MODE[TASK_ST001UNIT_ID].isOn = FALSE;
	}

	if(ALL_TASKS_MODE[TASK_ST005UNIT_ID].isOn)
	{
		ALL_TASKS_MODE[TASK_ST005UNIT_ID].isOn = FALSE;

	}

	if(ALL_TASKS_MODE[TASK_ST010UNIT_ID].isOn)
	{
		ALL_TASKS_MODE[TASK_ST010UNIT_ID].isOn = FALSE;
	}

	if(ALL_TASKS_MODE[TASK_ST020UNIT_ID].isOn)
	{
		ALL_TASKS_MODE[TASK_ST020UNIT_ID].isOn = FALSE;
		//m16_traceAGVInfo();
		//display_frames();
	}

	if(ALL_TASKS_MODE[TASK_ST050UNIT_ID].isOn)
	{
		ALL_TASKS_MODE[TASK_ST050UNIT_ID].isOn = FALSE;
		display_frames();
	}
	if(ALL_TASKS_MODE[TASK_ST100UNIT_ID].isOn)
	{
		ALL_TASKS_MODE[TASK_ST100UNIT_ID].isOn = FALSE;
	}

    /*if(ALL_TASKS_MODE[TASK_STATEMACHINE_ID].isOn)
	{
		ALL_TASKS_MODE[TASK_STATEMACHINE_ID].isOn = FALSE;

	}

	if(ALL_TASKS_MODE[TASK_TXHOST_ID].isOn)
	{
		ALL_TASKS_MODE[TASK_TXHOST_ID].isOn = FALSE;
		m16_traceAGVInfo();

	}

	if(ALL_TASKS_MODE[TASK_RS232_ID].isOn)
	{
		ALL_TASKS_MODE[TASK_RS232_ID].isOn = FALSE;

	}*/

}


void task_enable_task(BATTERY_TASKS_ID_ENUM task_id)
{
    ALL_TASKS_MODE[task_id].Supported = TRUE;
}

void task_disable_task(BATTERY_TASKS_ID_ENUM task_id)
{
    ALL_TASKS_MODE[task_id].Supported = FALSE;
    ALL_TASKS_MODE[task_id].isOn = FALSE;
    ALL_TASKS_MODE[task_id].Counter = ALL_TASKS_MODE[task_id].Default;
}

void task_enable_all_tasks()
{
    int task_id;

    for(task_id=0;task_id<NUMBER_OF_TASKS;task_id++)
		task_enable_task(task_id);
}

void task_disable_all_tasks()
{
    int task_id;

    for(task_id=0;task_id<NUMBER_OF_TASKS;task_id++)
		task_disable_task(task_id);
}

//
void task_initialize()
{

	//Disable all tasks
	task_disable_all_tasks();

	//Do single operation
    task_enable_task(TASK_ST001UNIT_ID);
    task_enable_task(TASK_ST005UNIT_ID);
    task_enable_task(TASK_ST010UNIT_ID);
    task_enable_task(TASK_ST020UNIT_ID);
    task_enable_task(TASK_ST050UNIT_ID);
    task_enable_task(TASK_ST100UNIT_ID);
}


/*******************************************************************/
//
//
/*******************************************************************/
//Refresh General Time and Timers inside
static void task_refresh_timers()
{
    int i;

    for(i=0;i<NUMBER_OF_TASKS;i++)
    {
        ALL_TASKS_MODE[i].Counter++;
        if(ALL_TASKS_MODE[i].Counter == ALL_TASKS_MODE[i].Timeout)
        {
            ALL_TASKS_MODE[i].isOn    = ALL_TASKS_MODE[i].Supported;
            ALL_TASKS_MODE[i].Counter = ALL_TASKS_MODE[i].Default;
        }
    }
}

//Refresh General Time and Timers inside
static void task_refresh_timers_old()
{
    int i;
    static firstCall = 1;

    if(firstCall)
    {
        u32_msLastTime = get_process_time();
        firstCall = 0;
    }

    //u32_msCurrentTime = utils_get_time_ms();

    u32_msCurrentTime = get_process_time();

    if(u32_msCurrentTime>(u32_msLastTime+sampleTaskTimeInMS))
    {
        u32_msLastTime = u32_msCurrentTime;

        for(i=0;i<NUMBER_OF_TASKS;i++)
		{
			ALL_TASKS_MODE[i].Counter++;
			if(ALL_TASKS_MODE[i].Counter == ALL_TASKS_MODE[i].Timeout)
			{
				ALL_TASKS_MODE[i].isOn    = ALL_TASKS_MODE[i].Supported;
				ALL_TASKS_MODE[i].Counter = ALL_TASKS_MODE[i].Default;
			}
		}
    }
}



static uint32 get_process_time() {
    uint64 u64_time_ms;
    uint32 u32_time_ms;
    struct timeval end;
    gettimeofday(&end, NULL);

    u64_time_ms = (((unsigned long long)end.tv_sec * 1000) + ((unsigned long long)end.tv_usec / 1000));

    u32_time_ms = (uint32)u64_time_ms;

    return u32_time_ms;
}

