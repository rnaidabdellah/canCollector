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
#ifndef APPCONFIG_H_INCLUDED
#define APPCONFIG_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif



#define MAX_NUMBER_OF_VARIABLE  100
#define MAX_NUMBER_OF_FRAMES    10


typedef struct VAR_TEMPLATE_STRUCT
{
    bbool        isSet;
	uint8       name[100]; //
	uint8       tag[25]; //Short name
	uint8       unit[10];
	uint8       type[10];
	uint8       length;
	uint32      value;//
	uint32      canid;
	uint8       canpos; //
	uint8       format[5]; //Short name
	uint8       primary[5];
	uint8       counter;//
	uint8       timeout;
	bbool        isFresh;
} VAR_TEMPLATE;

typedef struct FRAME_TEMPLATE_STRUCT
{
    bbool        isSet;
    uint32      canid;
    uint8       candlc;
	uint8       desc[100]; //
	uint8       tag[25]; //Short name
	uint8       refvars[MAX_NUMBER_OF_VARIABLE];
	uint8       varsCounter;
	uint8       counter;//
	uint8       timeout;
	bbool        isFresh;
} FRAME_TEMPLATE;

//
extern uint8 maxDefVars;
extern uint8 maxDefFrames;

// System Identification
extern uint8 configVersion[20];
extern uint8 businessUnit[20];
extern uint8 edgeHWRSN[20];
extern uint8 edgeSWRVer[20];
extern uint8 bmsHWRSN[20];
extern uint8 bmsSWRVer[20];


// CAN Info
extern uint8 CANChannelName[5];
extern uint16 CANSpeed;
extern uint8 NumberOfFrames;
extern uint8 CANTiemout;

//Sampling Rate in seconds
extern uint8 LifetimeDataRate;
extern uint8 LoggingSampleRate;

//Logging Info
extern uint8 log_folder[50];
extern uint8 max_log_file;
extern uint16 max_file_size;


extern VAR_TEMPLATE    ALL_VAR_ARRAY[];
extern FRAME_TEMPLATE  ALL_FRAMES_ARRAY[];


int readAppConfigFile();

#ifdef __cplusplus
}
#endif


#endif // APPCONFIG_H_INCLUDED
