/* 
 * File:   exported_functions.h
 * Author: Stefan
 *
 * Created on March 17, 2010, 1:27 PM
 */

#ifndef _EXPORTED_FUNCTIONS_H
#define	_EXPORTED_FUNCTIONS_H

#ifdef	__cplusplus
extern "C" {
#endif

//#include <windef.h> //BOOL
//typedef int BOOL ;
#include <preprocessor_macros/Windows_compatible_typedefs.h> //DWORD etc.
//#include <basetsd.h> //DWORD_PTR

//from http://stackoverflow.com/questions/2094427/dll-export-as-a-c-c-function:
#ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
  #define AM_LIB_EXPORT __declspec(dllexport)
#else
  #define AM_LIB_EXPORT //__attribute__ ((visibility("default")))
#endif // _WIN32

//EXPORT
AM_LIB_EXPORT BOOL ReadMSR(
  DWORD dwIndex,		// MSR index
  PDWORD p_dweax,			// bit  0-31
  PDWORD p_dwedx,			// bit 32-63
  DWORD_PTR affinityMask	// Thread Affinity Mask
) __attribute__ ((visibility("default"))) ;


#ifdef	__cplusplus
}
#endif

#endif	/* _EXPORTED_FUNCTIONS_H */

