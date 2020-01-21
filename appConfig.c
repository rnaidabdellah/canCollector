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
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <syslog.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <libconfig.h>

#include "types.h"
#include "definitions.h"
#include "service.h"
#include "global.h"
#include "tasks.h"
#include "appConfig.h"



VAR_TEMPLATE ALL_VAR_ARRAY[MAX_NUMBER_OF_VARIABLE];
FRAME_TEMPLATE  ALL_FRAMES_ARRAY[MAX_NUMBER_OF_FRAMES];

// System Identification
uint8 configVersion[20];
uint8 businessUnit[20];
uint8 edgeHWRSN[20];
uint8 edgeSWRVer[20];
uint8 bmsHWRSN[20];
uint8 bmsSWRVer[20];

// CAN Info
uint8 CANChannelName[5];
uint16 CANSpeed  = 500;
uint8 NumberOfFrames = 2;

//Sampling Rate in seconds
uint8 LifetimeDataRate = 5;
uint8 LoggingSampleRate = 10;

//Logging Info
uint8 log_folder[50];
uint8 max_log_file = 30;
uint16 max_file_size = 10000;



uint8 maxDefVars = 0;
uint8 maxDefFrames = 0;



//static void registerVar();
//static void registerFrame();
static void printCANMapping();

// Read Config File
int readAppConfigFile()
{
  config_t cfg;
  config_setting_t *allframes;
  config_setting_t *setting;
  const char *str;
  int length = 0;
  int nbrFrames = 0, nbrFields = 0;
  char frameName[50];
  int itmp;
  bbool isConfigFileValid = TRUE;


  config_init(&cfg);

  /* Read the file. If there is an error, report it and exit. */
  if(! config_read_file(&cfg, "AppConfig.cfg"))
  {
    syslog(LOG_INFO,"%s:%d - %s\n", config_error_file(&cfg), config_error_line(&cfg), config_error_text(&cfg));
    config_destroy(&cfg);
    return(EXIT_FAILURE);
  }


  /* Get CAN Info. */
  syslog(LOG_INFO,"/***** System Identification ******/");
  //System Identification:: ConfigVersion
  if(config_lookup_string(&cfg, "ConfigVer", &str))
  {
    strcpy(configVersion,str);
    syslog(LOG_INFO,"ConfigVer: %s\n", configVersion);
  }
  else
  {
    syslog(LOG_INFO,"No 'ConfigVer' setting in configuration file.\n");
    syslog(LOG_INFO,"Config File not valid.\n");
    config_destroy(&cfg);
    return(EXIT_FAILURE);
  }

  //System Identification:: BusinessUnit
  if(config_lookup_string(&cfg, "BusinessUnit", &str))
  {
    strcpy(businessUnit,str);
    syslog(LOG_INFO,"BusinessUnit: %s\n", businessUnit);
  }
  else
  {
    syslog(LOG_INFO,"No 'BusinessUnit' setting in configuration file.\n");
    syslog(LOG_INFO,"Config File not valid.\n");
    config_destroy(&cfg);
    return(EXIT_FAILURE);
  }
  //System Identification:: EdgeHWRSN
  if(config_lookup_string(&cfg, "EdgeHWRSN", &str))
  {
    strcpy(edgeHWRSN,str);
    syslog(LOG_INFO,"EdgeHWRSN: %s\n", edgeHWRSN);
  }
  else
  {
    syslog(LOG_INFO,"No 'EdgeHWRSN' setting in configuration file.\n");
    syslog(LOG_INFO,"Config File not valid.\n");
    config_destroy(&cfg);
    return(EXIT_FAILURE);
  }
  //System Identification:: EdgeSWRVer
  if(config_lookup_string(&cfg, "EdgeSWRVer", &str))
  {
    strcpy(edgeSWRVer,str);
    syslog(LOG_INFO,"EdgeSWRVer: %s\n", edgeSWRVer);
  }
  else
  {
    syslog(LOG_INFO,"No 'EdgeSWRVer' setting in configuration file.\n");
    syslog(LOG_INFO,"Config File not valid.\n");
    config_destroy(&cfg);
    return(EXIT_FAILURE);
  }
  //System Identification:: BMSHWRSN
  if(config_lookup_string(&cfg, "BMSHWRSN", &str))
  {
    strcpy(bmsHWRSN,str);
    syslog(LOG_INFO,"BMSHWRSN: %s\n", bmsHWRSN);
  }
  else
  {
    syslog(LOG_INFO,"No 'BMSHWRSN' setting in configuration file.\n");
    syslog(LOG_INFO,"Config File not valid.\n");
    config_destroy(&cfg);
    return(EXIT_FAILURE);
  }
  //System Identification:: BMSSWRVer
  if(config_lookup_string(&cfg, "BMSSWRVer", &str))
  {
    strcpy(bmsSWRVer,str);
    syslog(LOG_INFO,"BMSSWRVer: %s\n", bmsSWRVer);
  }
  else
  {
    syslog(LOG_INFO,"No 'BMSSWRVer' setting in configuration file.\n");
    syslog(LOG_INFO,"Config File not valid.\n");
    config_destroy(&cfg);
    return(EXIT_FAILURE);
  }

  /* Get CAN Info. */
  syslog(LOG_INFO,"/***** CAN Settings ******/");
  //CAN Channel Name
  if(config_lookup_int(&cfg, "CANChannel", &itmp))
    syslog(LOG_INFO,"CANChannel: %d\n", itmp);
  else
  {
    syslog(LOG_INFO,"No 'CANChannel' setting in configuration file.\n");
    syslog(LOG_INFO,"Config File not valid.\n");
    config_destroy(&cfg);
    return(EXIT_FAILURE);
  }
  //CAN Speed
  if(config_lookup_int(&cfg, "CANSpeed", &itmp))
    syslog(LOG_INFO,"CANSpeed: %d\n", itmp);
  else
  {
    syslog(LOG_INFO,"No 'CANSpeed' setting in configuration file.\n");
    syslog(LOG_INFO,"Config File not valid.\n");
    config_destroy(&cfg);
    return(EXIT_FAILURE);
  }
  //CAN Number of Defined Frames
  if(config_lookup_int(&cfg, "NumberOfFrames", &itmp))
    syslog(LOG_INFO,"NumberOfFrames: %d\n", itmp);
  else
  {
    syslog(LOG_INFO,"No 'NumberOfFrames' setting in configuration file.\n");
    syslog(LOG_INFO,"Config File not valid.\n");
    config_destroy(&cfg);
    return(EXIT_FAILURE);
  }
  //CAN Timeout
  if(config_lookup_int(&cfg, "CANTimeout", &itmp))
    syslog(LOG_INFO,"CANTimeout: %d\n", itmp);
  else
  {
    syslog(LOG_INFO,"No 'CANTimeout' setting in configuration file.\n");
    syslog(LOG_INFO,"Config File not valid.\n");
    config_destroy(&cfg);
    return(EXIT_FAILURE);
  }


  /* Get Sampling Info. */
  syslog(LOG_INFO,"/***** Sampling Info ******/");
  //CAN Number of Defined Frames
  if(config_lookup_int(&cfg, "LifetimeDataRate", &itmp))
    syslog(LOG_INFO,"LifetimeDataRate: %d\n", itmp);
  else
  {
    syslog(LOG_INFO,"No 'LifetimeDataRate' setting in configuration file.\n");
    syslog(LOG_INFO,"Config File not valid.\n");
    config_destroy(&cfg);
    return(EXIT_FAILURE);
  }
  //Logging sample rate
  if(config_lookup_int(&cfg, "LoggingSampleRate", &itmp))
    syslog(LOG_INFO,"LoggingSampleRate: %d\n", itmp);
  else
  {
    syslog(LOG_INFO,"No 'LoggingSampleRate' setting in configuration file.\n");
    syslog(LOG_INFO,"Config File not valid.\n");
    config_destroy(&cfg);
    return(EXIT_FAILURE);
  }


  // Output a list of all fields in frame1.
  allframes = config_lookup(&cfg, "allfields");
  if(allframes != NULL)
  {
    nbrFrames = config_setting_length(allframes);
    syslog(LOG_INFO,"/***** Number of expected frames : %d ******/",nbrFrames);

    for(itmp = 0; itmp < nbrFrames; itmp++)
    {
        //Update Frame Counter
        maxDefFrames++;
        ALL_FRAMES_ARRAY[maxDefFrames-1].isSet = TRUE;


        sprintf(frameName,"allfields.frame%d",itmp+1);
        setting = config_lookup(&cfg, frameName);
          if(setting != NULL)
          {

            int count = config_setting_length(setting);
            int i;
            syslog(LOG_INFO,"/***** %s Info (Nbre of fields %d) ******/",frameName, count);

            length = 0;
            syslog(LOG_INFO,"%s,%s,%s,%s,%s,%s,%s,%s,%s", "NAME", "Tag", "UNIT", "Size", "Type", "CANId", "CANPos", "Format","Primary");

            for(i = 0; i < count; ++i)
            {
              config_setting_t *field = config_setting_get_elem(setting, i);

              // Only output the record if all of the expected fields are present.
              const char *name, *shn, *unit, *type, *format, *primary;
              int size,id,pos;
              bbool isfieldvalid = 1;

              isfieldvalid = isfieldvalid && config_setting_lookup_string(field, "Name", &name);
              isfieldvalid = isfieldvalid && config_setting_lookup_string(field, "Tag", &shn);
              isfieldvalid = isfieldvalid && config_setting_lookup_string(field, "Unit", &unit);
              isfieldvalid = isfieldvalid && config_setting_lookup_string(field, "Type", &type);
              isfieldvalid = isfieldvalid && config_setting_lookup_string(field, "Format", &format);
              isfieldvalid = isfieldvalid && config_setting_lookup_string(field, "Primary", &primary);

              isfieldvalid = isfieldvalid && config_setting_lookup_int(field, "Size", &size);
              isfieldvalid = isfieldvalid && config_setting_lookup_int(field, "CANId", &id);
              isfieldvalid = isfieldvalid && config_setting_lookup_int(field, "CANPos", &pos);

              if(!isfieldvalid)
                continue;
              //Update Var List
                maxDefVars++;
                ALL_VAR_ARRAY[maxDefVars-1].isSet = TRUE;
                strcpy(ALL_VAR_ARRAY[maxDefVars-1].name, name);
                strcpy(ALL_VAR_ARRAY[maxDefVars-1].tag, shn);
                strcpy(ALL_VAR_ARRAY[maxDefVars-1].unit, unit);
                strcpy(ALL_VAR_ARRAY[maxDefVars-1].type, type);
                strcpy(ALL_VAR_ARRAY[maxDefVars-1].format, format);
                strcpy(ALL_VAR_ARRAY[maxDefVars-1].primary, primary);
                ALL_VAR_ARRAY[maxDefVars-1].length = size;
                ALL_VAR_ARRAY[maxDefVars-1].canid = id;
                ALL_VAR_ARRAY[maxDefVars-1].canpos = pos;
                ALL_FRAMES_ARRAY[maxDefFrames-1].varsCounter++;
                ALL_FRAMES_ARRAY[maxDefFrames-1].refvars[ALL_FRAMES_ARRAY[maxDefFrames-1].varsCounter-1] = maxDefVars-1;
                ALL_FRAMES_ARRAY[maxDefFrames-1].canid = id;
              syslog(LOG_INFO,"%s,%s,%s,%s,%s,%s,%d,%d,%d", name, shn, unit, type, format, primary, size, id, pos);
              //Update List of frames and List of variables
            }
            syslog(LOG_INFO,"\n");

            length = 0;
          }
    }

    printCANMapping();
  }
  else
    syslog(LOG_INFO,"No CAN Mapping Defined \n");

  config_destroy(&cfg);
  return(EXIT_SUCCESS);
}

/*static void registerFrame()
{

    maxDefFrames++;
    ALL_FRAMES_ARRAY[maxDefFrames-1].isSet = TRUE;

}

static void registerVar()
{

    maxDefVars++;
    ALL_VAR_ARRAY[maxDefVars-1].isSet = TRUE;
    ALL_FRAMES_ARRAY[maxDefFrames-1].varsCounter++;
    ALL_FRAMES_ARRAY[maxDefFrames-1].refvars[varsCounter-1] = maxDefVars-1;
	strcpy(ALL_FRAMES_ARRAY[maxDefVars-1].name, name);
	strcpy(ALL_FRAMES_ARRAY[maxDefVars-1].tag, tag);
	strcpy(ALL_FRAMES_ARRAY[maxDefVars-1].unit, unit);
	strcpy(ALL_FRAMES_ARRAY[maxDefVars-1].type, type);
	strcpy(ALL_FRAMES_ARRAY[maxDefVars-1].format, format);
	strcpy(ALL_FRAMES_ARRAY[maxDefVars-1].primary, primary);
	ALL_FRAMES_ARRAY[maxDefVars-1].length = size;
	ALL_FRAMES_ARRAY[maxDefVars-1].canid = id;
	ALL_FRAMES_ARRAY[maxDefVars-1].canpos = pos;

}*/

static void printCANMapping()
{
    int i,j;
    char buf[512];
    int ibuf = 0;
    int indxVar;
    int indxFrm;

    syslog(LOG_INFO,"/***** Defined Frames (%d) and Variables (%d) : ******/",maxDefFrames,maxDefVars);
    for(i=0;i<(maxDefFrames);i++)
    {
        indxFrm = i;
        //if(ALL_FRAMES_ARRAY[indxFrm].isSet)
        {
            //syslog(LOG_INFO,"%ld:",ALL_FRAMES_ARRAY[i-1].canid);
            ibuf += sprintf(buf+ibuf,"%lx:",ALL_FRAMES_ARRAY[indxFrm].canid);
            for(j=0;j<(ALL_FRAMES_ARRAY[indxFrm].varsCounter);j++)
            {
                //syslog(LOG_INFO,"%s(%d),",ALL_VAR_ARRAY[i-1].tag,ALL_VAR_ARRAY[i-1].length);
                indxVar = ALL_FRAMES_ARRAY[indxFrm].refvars[j];
                ibuf += sprintf(buf+ibuf,"%s(%d),",ALL_VAR_ARRAY[indxVar].tag,ALL_VAR_ARRAY[indxVar].length);
            }
            syslog(LOG_INFO,"%s\n",buf);
            ibuf = 0;
        }
    }

}

