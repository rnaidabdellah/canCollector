//-----------------------------------------------------------------------------
/*!
   \internal
   \file          can.h
   \brief         AGV
   \implementation
   copyright   LeclanchÃ©
   license
   created     08.01.2020 Rachid Naid-Abdellah
   \endimplementation
   \internal
   \history
   Date(dd.mm.yyyy)  Author                  Description
   08.01.2020        Rachid Naid-Abdellah    file created
   \endhistory
*/
//-----------------------------------------------------------------------------

#ifndef CAN_H_INCLUDED
#define CAN_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/* -- Defines ------------------------------------------------------------- */
//"At time of writing, these constants are not defined in the headers"
#ifndef PF_CAN
  #define PF_CAN 29
#endif

#ifndef AF_CAN
  #define AF_CAN PF_CAN
#endif

#ifndef MAX_WAIT_TIME_MS
  #define MAX_WAIT_TIME_MS 10 // max socket read wait time in milliseconds
#endif

//
#define CAN_NO_ERR           ( 0 )
#define CAN_UNKNOWN_ERR      (-1 )
#define CAN_WARN             (-2 )
#define CAN_DEFAULT          (-3 )
#define CAN_BUSY             (-4 )
#define CAN_RANGE            (-5 )
#define CAN_OVERFLOW         (-6 )
#define CAN_RD_WR            (-7 )
#define CAN_NOACT            (-8 )
#define CAN_COM              (-9 )
#define CAN_CONFIG           (-10)
#define CAN_CHECKSUM         (-11)
#define CAN_TIMEOUT          (-12)
#define CAN_NOACT1           (-13 )
#define CAN_NOACT2           (-14 )




/* -- Includes ------------------------------------------------------------ */
//#include "stwtypes.h"

/* -- Types --------------------------------------------------------------- */
typedef struct
{
   unsigned long u32_Id; // CAN identifier 11/29bit
   unsigned char  u8_Xtd; // u8_XTD == TRUE: 29bit ID, u8_XTD == FALSE: 11bit ID
} T_can_id;

typedef struct
{
   T_can_id t_Id;   // CAN message ID
   unsigned char u8_Dlc;      // Data Length Code (0..8)
   unsigned char au8_Data[8]; // message data bytes
   unsigned long u32_time;   // timestamp of reception in milliseconds (system time), not used for TX messages
   unsigned long long   msg64bits;
} T_can_msg;



/* -- Global Variables ---------------------------------------------------- */

extern signed short       ms16_SockFDCAN_1;
extern signed short       ms16_SockFDCAN_2;

/* -- Function Prototypes ------------------------------------------------- */
extern signed long ms32_InitCAN (int channel);
extern signed long gs32_CAN_InitBus (signed short * const ops16_Socket, const unsigned char ou8_Bus);
extern signed long gs32_CAN_SendMsg(const signed short * const ops16_Socket, const T_can_msg * const opt_Message);
extern signed long gs32_CAN_RecvMsg(const signed short * const ops16_Socket, T_can_msg * const opt_Message);
extern signed long gs32_CAN_CloseBus(signed short * const ops16_Socket);


#ifdef __cplusplus
}
#endif


/* -- Implementation ------------------------------------------------------ */

#endif // CAN_H_INCLUDED
