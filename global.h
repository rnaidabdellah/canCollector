/*!
   \internal
   \file          global.h
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

#ifndef GLOBAL_AGV_H_INCLUDED
#define GLOBAL_AGV_H_INCLUDED



//General
#define sampleTaskTimeInMS   	    				10  //[ms]






typedef struct AGV_DUALBATTERY_INFO_STRUCT
{
	bbool xLogStatus;
	bbool xOffLine;
}AGV_DUALBATTERY;




extern AGV_DUALBATTERY agv_dualstring;


#endif // GLOBAL_AGV_H_INCLUDED
