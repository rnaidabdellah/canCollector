//-----------------------------------------------------------------------------
/*!
   \internal
   \file          can.c
   \brief         AGV
   \implementation
   copyright   Leclanché
   license
   created     08.01.2019 Rachid Naid-Abdellah
   \endimplementation
   \internal
   \history
   Date(dd.mm.yyyy)  Author                  Description
   08.01.2019        Rachid Naid-Abdellah    file created
   \endhistory
*/
//-----------------------------------------------------------------------------/*

//-----------------------------------------------------------------------------/*

/* -- Includes ------------------------------------------------------------ */
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <time.h>
#include <sys/time.h>
#include <net/if.h>
#include <errno.h>
#include "can.h"
/* -- Types --------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------- */
static struct timeval mt_TimeSinceInit;
/* -- Module Global Function Prototypes ----------------------------------- */

signed short       ms16_SockFDCAN_1 = -1;
signed short       ms16_SockFDCAN_2 = -1;

/* -- Implementation ------------------------------------------------------ */

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
signed long ms32_InitCAN (int channel)
{
    signed long s32_retval = CAN_NO_ERR;

    if(channel == 0)
        s32_retval = gs32_CAN_InitBus (&ms16_SockFDCAN_1, 0);

    if(channel == 1)
        s32_retval = gs32_CAN_InitBus (&ms16_SockFDCAN_2, 1);

    return (s32_retval);
}


//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
signed long gs32_CAN_InitBus (signed short * const ops16_Socket, const unsigned char ou8_Bus)
{
   signed long s32_retval              = CAN_NO_ERR;
   static unsigned char hu8_FirstTimeFlag = 1;
   struct can_filter t_CanFilter;
   struct sockaddr_can t_addr;
   struct ifreq t_ifr;

   // store the actual time on init
   if (hu8_FirstTimeFlag == 1)
   {
      (void) gettimeofday (&mt_TimeSinceInit, NULL);
      hu8_FirstTimeFlag = 0;
   }
   // set the can filter to zero, so that we receive everything!
   t_CanFilter.can_id = CAN_EFF_MASK;
   t_CanFilter.can_mask = 0; // Mask which is applied to incoming IDs
   // Check for CAN bus number correctnes
   // check CAN_BUS
   if (ou8_Bus == 0) // CAN0 selected
   {
      (void) memcpy (t_ifr.ifr_name, "can0\0", sizeof (t_ifr.ifr_name));
      //(void) memcpy (t_ifr.ifr_name, 0, sizeof (t_ifr.ifr_name));
   }
   else if (ou8_Bus == 1) //CAN1 selected
   {
      (void) memcpy (t_ifr.ifr_name, "can1\0", sizeof (t_ifr.ifr_name));
   }
   else
   {
      // CAN Bus Number to high for TC3
      s32_retval = CAN_CONFIG;
   }
   // If no error occured, go on
   if (s32_retval == CAN_NO_ERR)
   {
      // ************************* RxTx Socket configuration*************************
      // open socket
      // function shall create an unbound socket in a communications domain, and return a file descriptor
      // CAN_RAW = Raw protocol of PF_CAN, applicable to socket type SOCK_RAW
      *ops16_Socket = socket (PF_CAN, SOCK_RAW, CAN_RAW);
      if (*ops16_Socket >= 0)
      {
         // SIOCGIFINDEX is an macro, which retrieve the interface index of the interface into t_ifr.ifr_index
         if (ioctl(*ops16_Socket, SIOCGIFINDEX, &t_ifr) >= 0)
         {
            if (setsockopt (*ops16_Socket, SOL_CAN_RAW, CAN_RAW_FILTER, &t_CanFilter, sizeof (t_CanFilter)) == 0)
            {
               memset(&t_addr, 0, sizeof(t_addr));//added by rachid 09.01.2020
               t_addr.can_family = AF_CAN;
               t_addr.can_ifindex = t_ifr.ifr_ifindex;
               // bind socket
               if (bind(*ops16_Socket, (struct sockaddr *)&t_addr, sizeof(t_addr)) < 0)
               {
                  // close socket
                  (void) gs32_CAN_CloseBus(ops16_Socket);
                  // set return value
                  s32_retval = CAN_CONFIG;
               }
            }
            else
            {
               // close socket
               (void) gs32_CAN_CloseBus(ops16_Socket);
               // set return value
               s32_retval = CAN_CONFIG;
            }
         }
         else
         {
            // close socket
            (void) gs32_CAN_CloseBus(ops16_Socket);
            // set return value
            s32_retval = CAN_CONFIG;
         }
      }
      else
      {
         // error occured while creating new Socket
         s32_retval = CAN_CONFIG;
      }
   }
   return (s32_retval);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
signed long gs32_CAN_RecvMsg(const signed short * const ops16_Socket, T_can_msg * const opt_Message)
{
   signed long s32_retval = CAN_NO_ERR;
   fd_set t_readfds;
   struct timeval t_Timeout = {1, 0};
   static struct can_frame ht_frame;
   // Set timeout to seconds
   t_Timeout.tv_sec  = 0;
   t_Timeout.tv_usec = MAX_WAIT_TIME_MS * 1000;

   // check if he have a filedescriptor
   if (*ops16_Socket >= 0)
   {
      // clear fds
      FD_ZERO (&t_readfds);
      // set read fds
      FD_SET (*ops16_Socket, &t_readfds);
      // check if we are ready to read
      if (select ((*ops16_Socket) + 1, &t_readfds, NULL, NULL, &t_Timeout) > 0)
      //if (1)
      {
         // check if corresponding bit is set in the fds
         if (FD_ISSET (*ops16_Socket, &t_readfds))
         {
            if (read (*ops16_Socket, &ht_frame, sizeof (ht_frame)) >= 0)
            {
               // new message received
               opt_Message->t_Id.u32_Id = (ht_frame.can_id & 0x1FFFFFFFu); // get received ID
               opt_Message->t_Id.u8_Xtd = (0x01 & (ht_frame.can_id >> 31));  // get extended flag
               opt_Message->u8_Dlc = ht_frame.can_dlc;                     // get data length code
               (void) memcpy (opt_Message->au8_Data, ht_frame.data, sizeof(opt_Message->au8_Data));   // copy received data

               //
               unsigned long long arr64 = 0;
                int i;

                for(i=0;i<8;i++)
                    arr64 = arr64 | (((unsigned long long)opt_Message->au8_Data[i])<<(i*8));
                opt_Message->msg64bits = arr64;
            }
            else
            {
               // no new message
               s32_retval = CAN_NOACT;
            }
         }
         else
         {
            // no new message
            s32_retval = CAN_NOACT1;
         }
      }
      else
      {
          // no new message
          s32_retval = CAN_NOACT2;
      }
   }
   else
   {
      s32_retval = CAN_CONFIG;
   }
   return (s32_retval);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
signed long gs32_CAN_SendMsg(const signed short * const ops16_Socket, const T_can_msg * const opt_Message)
{
   signed long s32_retval     = CAN_NO_ERR;
   signed short s16_NumOfBytes = 0;
   static struct can_frame ht_frame;
   unsigned char u8_retries = 0;
   unsigned char u8_retry = 0;
   // check if the socket is correct bind
   if (*ops16_Socket >= 0)
   {
      ht_frame.can_id =  (0x1FFFFFFFu & opt_Message->t_Id.u32_Id);  // set id
      ht_frame.can_id |= (0x80000000u & (opt_Message->t_Id.u8_Xtd << 31));   // set extended flag
      ht_frame.can_dlc = opt_Message->u8_Dlc;                       // set data length code
      (void) memcpy (ht_frame.data, opt_Message->au8_Data, sizeof (ht_frame.data));                              // copy data

      s32_retval = CAN_OVERFLOW;
      do
      {
         u8_retries++;
         u8_retry = 0;
         s16_NumOfBytes = write (*ops16_Socket, &ht_frame, sizeof (ht_frame));
         if (s16_NumOfBytes < 0)
         {
            if (errno == ENOBUFS) // No buffer space available (txqueuelen of CAN netdevice is exhausted)
            {
               usleep (10000); //10ms, it is only an example value which have to be added for your needs
               u8_retry = 1;
            }
         }
         else
         {
            if (s16_NumOfBytes >= (signed short) sizeof (ht_frame))
            {
               s32_retval = CAN_NO_ERR;
            }
         }
      } while ((u8_retry == 1) && (u8_retries < 5u));
   }
   else
   {
      // socket error
      s32_retval = CAN_CONFIG;
   }
   return (s32_retval);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
signed long gs32_CAN_CloseBus(signed short * const ops16_Socket)
{
   // check if the socket is correct bind
   if (*ops16_Socket >= 0)
   {
      // close previously initialized socket
      (void) close(*ops16_Socket);
   }
   return (CAN_NO_ERR);
}


