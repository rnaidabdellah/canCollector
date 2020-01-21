/*!
   \internal
   \file          service.h
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

#ifndef SERVICE_H_INCLUDED
#define SERVICE_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

///*--------- Global Variable MODULE --------------------- */


///* -- Module Global Function Prototypes ----------------------------------- */


///* custom functions */


/* -- CAN Operations ----------------------------------- */
void svc_disp_local_time();
void svc_init_logging();
void svc_log_event(int EVENT_ID, bbool OnOff);
//void svc_log_dbg();

void display_frames();

//extern char info_log_name[100];

//
void svc_open_csv_stream();

#ifdef __cplusplus
}
#endif

#endif // SERVICE_H_INCLUDED
