/*!
   \internal
   \file          tasks.h
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

#ifndef TASKS_H_INCLUDED
#define TASKS_H_INCLUDED


#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************/
//
//
/*******************************************************************/
#define NUMBER_OF_TASKS  11

typedef enum BATTERY_TASKS_ID_ENUM_TAG
{
	TASK_ST001UNIT_ID,		// <00>- Sampletime 001 Unit
	TASK_ST005UNIT_ID,		// <00>- Sampletime 005 Unit
	TASK_ST010UNIT_ID,		// <00>- Sampletime 010 Unit
	TASK_ST020UNIT_ID,		// <00>- Sampletime 020 Unit
	TASK_ST050UNIT_ID,		// <00>- Sampletime 050 Unit
	TASK_ST100UNIT_ID,		// <00>- Sampletime 100 Unit
	TASK_ID_END				// End marker
} BATTERY_TASKS_ID_ENUM;



typedef struct TASK_DATA_STRUCT
{
	uint8       ID; //
	uint16      Counter;
	uint16      Default;
	uint16      Timeout;	//
	uint8       Supported;
	bbool        isOn;
} TASK_DATA;


extern TASK_DATA    ALL_TASKS_MODE[];

//
void task_initialize();
void task_run_enabled_tasks();



void task_enable_task(BATTERY_TASKS_ID_ENUM task_id);
void task_disable_task(BATTERY_TASKS_ID_ENUM task_id);
void task_enable_all_tasks();
void task_disable_all_tasks();

#ifdef __cplusplus
}
#endif

#endif // TASKS_H_INCLUDED
