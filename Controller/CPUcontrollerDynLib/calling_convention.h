/*
 * calling_convention.h
 *
 *  Created on: 05.02.2011
 *      Author: Stefan
 */

#ifndef CALLING_CONVENTION_H_
#define CALLING_CONVENTION_H_

#ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
  /** Calling convention--must both be the same in function signature between the
  * dynamic libary (DLL, .so) and the executable's function that calls it?! */
  #define DYN_LIB_CALLING_CONVENTION _cdecl
#else
  #define DYN_LIB_CALLING_CONVENTION /* ->empty */
#endif
//#define DYN_LIB_CALLING_CONVENTION __stdcall

#define FUNCTION_NAME_FOR_INIT Init

#endif /* CALLING_CONVENTION_H_ */
