//-----------------------------------------------------------------------------
/*!
   \internal
   \file          service.c
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



//#define LOG_AGV_FOLDER             "/mnt/sd/agv"
#define LOG_AGV_FOLDER             "./agv"
#define MAXLOGFILES                30
#define MAX_FILE_SIZE              10000


//char info_log_name[100];
#define info_log_name             "./agv/agvappLogFile.log"
#define info_csv_name             "./agv/agvappLogFile.csv"




charn acn_LogString[512];
char  pathLogFile[128];
char  pathEventFile[128];
int   iTraceCounter = 0;
long  filesize;
int   iFileCounter = -1;
int   iFrameCounter = 0;

FILE *log_stream;
FILE *csv_stream;


static int count_dir_content();
static void svc_append_logFile(char *str2log,int size);
static void svc_open_log_stream();


void display_frames()
{
    uint8 i;
    int indxVar;
    int length = 0;
    char buff[20];
    time_t now = time(NULL);


    if(iFrameCounter==0)
    {
        strftime(buff, 20, "%d-%m-%Y %H:%M:%S", localtime(&now));
        length += sprintf(acn_LogString+length, "\nTime:  %s\n", buff);
    }

    if(ALL_FRAMES_ARRAY[iFrameCounter].isSet)
    {
        length += sprintf(acn_LogString+length, "%lx:  ", ALL_FRAMES_ARRAY[iFrameCounter].canid);
        for(i=0;i<ALL_FRAMES_ARRAY[iFrameCounter].varsCounter;i++)
        {
            indxVar = ALL_FRAMES_ARRAY[iFrameCounter].refvars[i];
            length += sprintf(acn_LogString+length, "%s = %lu; ", ALL_VAR_ARRAY[indxVar].tag,ALL_VAR_ARRAY[indxVar].value);
        }
        svc_append_logFile(acn_LogString,length);
        iFrameCounter = iFrameCounter+1;
    }
    else
        iFrameCounter = 0;
    return;
}




void svc_disp_local_time()
{
    char buff[20];
    time_t now = time(NULL);
    strftime(buff, 20, "%Y-%m-%d %H:%M:%S", localtime(&now));
}

/* let us make a recursive function to print the content of a given folder */
static int count_dir_content()
{
  DIR * d = opendir(LOG_AGV_FOLDER); // open the path
  int file_counter = 0;
  if(d==NULL)
        return file_counter; // if was not able return
  struct dirent * dir; // for the directory entries
  while ((dir = readdir(d)) != NULL) // if we were able to read somehting from the directory
    {
        file_counter++;
    }
    if(file_counter<0)
        file_counter = 0;
    closedir(d); // finally close the directory

  return file_counter; // if was not able return
}

void svc_init_logging()
{
  FILE  *logFilePointer;
  //char  regularlogfile[] = "agv_cvsfileXXXXX.log";
  char  regularlogfile[20];

  char buff[20];
  time_t now = time(NULL);

  strftime(regularlogfile, 20, "%Y-%m-%d %H:%M:%S", localtime(&now));
  //int file_counter = 0;

  agv_dualstring.xLogStatus = 0;

  if(iFileCounter<0)
    iFileCounter = count_dir_content() - 3;

  iFileCounter++;
  if(iFileCounter<0)
    iFileCounter = 0;
  if(iFileCounter>=MAXLOGFILES)
    iFileCounter = iFileCounter%MAXLOGFILES;

  //sprintf(regularlogfile,"agv_logfile%05d.txt",iFileCounter);
  sprintf(pathLogFile, "%s/agv_logfile%s", LOG_AGV_FOLDER,regularlogfile );
  logFilePointer = fopen(pathLogFile,"w");
  if(logFilePointer == NULL)
  {
    syslog(LOG_INFO,"Could not open log file: %s\n", pathLogFile);
  }
  else
  {
    syslog(LOG_INFO,"Opening this file was successful: %s\n", pathLogFile);

    fclose(logFilePointer);
    agv_dualstring.xLogStatus = 1;
  }
  return; // if was not able return
}

static void svc_open_log_stream()
{
  log_stream = fopen(info_log_name , "a+");
  if(log_stream == NULL)
  {
    syslog(LOG_INFO,"Could not open log file: %s\n", info_log_name);
  }
  else
  {
    syslog(LOG_INFO,"Opening log file was successful: %s\n", info_log_name);
  }
  return; // if was not able return
}

void svc_open_csv_stream()
{
  csv_stream = fopen(info_csv_name , "a+");
  if(csv_stream == NULL)
  {
    syslog(LOG_INFO,"Could not open csv file: %s\n", info_csv_name);

  }
  else
  {
    syslog(LOG_INFO,"Opening csv file was successful: %s\n", info_csv_name);
    //add header file
    int i;
    for(i=0;i<maxDefVars;i++)
        fprintf(csv_stream, "%s,", ALL_VAR_ARRAY[i].tag);
    fprintf(csv_stream, "\n");
    //close csv_file;
    fclose(csv_stream);
  }
  return; // if was not able return
}



static void svc_append_logFile(char *str2log,int size)
{
    FILE  *logFilePointer;
    int ch;//char ch;
    int tmpCounter = 0;

    if(log_stream!=NULL)
    {
        str2log[size] = '\0';
        fprintf(log_stream, "%s\n", str2log);
        //Check File Size
        fseek(log_stream, 0, SEEK_END);
        filesize = ftell(log_stream);

        if(filesize>MAX_FILE_SIZE)
        {
            svc_init_logging();
            logFilePointer = fopen(pathLogFile, "a+");
            if(logFilePointer==NULL)
            {
                syslog(LOG_INFO,"Could not open log file: %s\n", pathLogFile);
            }
            else
            {
                syslog(LOG_INFO,"Backup new file: %s\n", pathLogFile);
                fclose(log_stream);
                log_stream = fopen(info_log_name , "r");
                while( (( ch = fgetc(log_stream) ) != EOF) && (tmpCounter<MAX_FILE_SIZE) )
                {
                    fputc(ch, logFilePointer);
                    tmpCounter++;
                }
                fclose(logFilePointer);
                //
                fclose(log_stream);
                log_stream = fopen(info_log_name , "w");
                fclose(log_stream);
                log_stream = fopen(info_log_name , "a+");
            }
        }
        agv_dualstring.xLogStatus = 1;
    }
    else
    {
        svc_open_log_stream();
        agv_dualstring.xLogStatus = 0;
    }

    return;
}


