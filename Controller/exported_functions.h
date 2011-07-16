/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
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
  #define EXECUTABLE_EXPORT_DEFINITION __declspec(dllexport)
#else
  #define EXECUTABLE_EXPORT_DEFINITION //__attribute__ ((visibility("default")))
#endif // _WIN32

//EXPORT
EXECUTABLE_EXPORT_DEFINITION BOOL ReadMSR(
  DWORD dwIndex,		// MSR index
  PDWORD p_dweax,			// bit  0-31
  PDWORD p_dwedx,			// bit 32-63
  DWORD_PTR affinityMask	// Thread Affinity Mask
) __attribute__ ((visibility("default"))) ;


#ifdef	__cplusplus
}
#endif

#endif	/* _EXPORTED_FUNCTIONS_H */

