/*!
   \internal
   \file          bmudriver.h
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

#ifndef BMUDRIVER_H_INCLUDED
#define BMUDRIVER_H_INCLUDED


#ifdef __cplusplus
extern "C" {
#endif



//void mv_can2claytonbmu (T_can_msg const * const opt_CAN_msg);
void can2data (T_can_msg const * const opt_CAN_msg);
//void send_tx_frames();


#ifdef __cplusplus
}
#endif

#endif // BMUDRIVER_H_INCLUDED
