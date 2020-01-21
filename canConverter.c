//-----------------------------------------------------------------------------
/*!
   \internal
   \file          canConverter.c
   \brief         AGV
   \implementation
   copyright   LeclanchÃ©
   license
   created     15.01.2017 Rachid Naid-Abdellah
   \endimplementation
   \internal
   \history
   Date(dd.mm.yyyy)  Author                  Description
   15.01.2017        Rachid Naid-Abdellah    file created
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
#include <syslog.h>
//#include <dbus/dbus.h>

#include "types.h"
#include "definitions.h"
#include "can.h"
#include "service.h"
#include "global.h"
#include "tasks.h"
#include "canConverter.h"
#include "appConfig.h"





AGV_DUALBATTERY agv_dualstring;



static void updateReceivedVar(uint8 *arr, uint8 varPos);
static void updateRcvdVar(T_can_msg *tcanmsg, uint8 varIndx);

//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void can2data (T_can_msg const * const opt_CAN_msg)
{
    uint8  u8t[8];
    uint16 u16t[4];

    int i,indxFrm = -1,indxVar = -1;

    if (opt_CAN_msg != NULL)
    {
        //Find the frame index
        for(i=0;i<MAX_NUMBER_OF_FRAMES;i++)
        {
            if(ALL_FRAMES_ARRAY[i].isSet && (ALL_FRAMES_ARRAY[i].canid==opt_CAN_msg->t_Id.u32_Id))
            {
                indxFrm = i;
            }
        }

        if(indxFrm>(-1))
        {
            //Update SOC for test purpose
            u8t[0]  = (uint8)(opt_CAN_msg->au8_Data[1]);
            for(i=0;i<ALL_FRAMES_ARRAY[indxFrm].varsCounter;i++)
            {
                indxVar = ALL_FRAMES_ARRAY[indxFrm].refvars[i];
                if(indxVar>(-1))
                {
                    updateRcvdVar(opt_CAN_msg, indxVar);
                }
            }
        }

        /*if(indxFrm==1)
        {
            //Update SOC for test purpose
            u8t[0]  = (uint8)(opt_CAN_msg->au8_Data[1]);
            indxVar = ALL_FRAMES_ARRAY[indxFrm].refvars[1];
            if(indxFrm>(-1))
            {
                updateReceivedVar(opt_CAN_msg->au8_Data, indxVar);
            }
        }*/

        //syslog(LOG_INFO, "Frame Index %d, Variable Index %d = %d", indxFrm, indxVar, (uint8)ALL_VAR_ARRAY[7].value);
    }
}


/***********************************************************************
* Useful functions
************************************************************************/

static void updateReceivedVar(uint8 *arr, uint8 varPos)
{
    /*int i;
    int k,l;
    uint64 arr64 = 0;
    uint32 ret = 0;
    uint64 mask = 0;

    k = ALL_VAR_ARRAY[varPos].canpos;
    l = ALL_VAR_ARRAY[varPos].length;

    for(i=0;i<8;i++)
        arr64 = arr64 | ((uint64)0xFF)<<(56-i*8));

    if(strcpy(ALL_VAR_ARRAY[varPos].format,"Byte")*/
        ALL_VAR_ARRAY[varPos].value = arr[0];
}



static void updateRcvdVar(T_can_msg *tcanmsg, uint8 varIndx)
{
    int position,length;
    uint64 arr64;

    position = ALL_VAR_ARRAY[varIndx].canpos;
    length = ALL_VAR_ARRAY[varIndx].length;
    arr64 = tcanmsg->msg64bits;

    ALL_VAR_ARRAY[varIndx].value = (uint32)((((uint64)1 << length) - 1) & (arr64 >> (position - 0)));

}

