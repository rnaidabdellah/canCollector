//-----------------------------------------------------------------------------
/*!
   \internal
   \file          stwtypes.h
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
//-----------------------------------------------------------------------------

#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED


#ifdef __cplusplus
extern "C" {
#endif

/* -- Includes ------------------------------------------------------------ */

/* -- Defines ------------------------------------------------------------- */
#define INLINE inline

/* -- Types --------------------------------------------------------------- */
typedef unsigned char         uint8;      // data type unsigned 8bit
typedef signed char           sint8;      // data type signed 8bit
typedef unsigned short        uint16;     // data type unsigned 16bit
typedef signed short          sint16;     // data type signed 16bit
typedef unsigned long         uint32;     // data type unsigned 32bit
typedef signed long           sint32;     // data type signed 32bit
typedef unsigned long long    uint64;     // data type unsigned 64bit (requires compiler option "--long-long")
typedef signed long long      sint64;     // data type signed 64bit (requires compiler option "--long-long")

typedef float                 float32;    // data type IEEE float 32bit
typedef double                float64;    // data type IEEE float 64bit
typedef long double           float80;    // data type IEEE float 80bit

// native data types
typedef unsigned int          uintn;      // native unsigned int
typedef signed int            sintn;      // native signed int
typedef char                  charn;      // native char type

/* -- Global Variables ---------------------------------------------------- */
typedef unsigned char         bbool;      // native char type
/* -- Function Prototypes ------------------------------------------------- */

/* -- Implementation ------------------------------------------------------ */

#ifdef __cplusplus
} /* end of extern "C" */
#endif

#endif  /* _STWTYPES_H */
