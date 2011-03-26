/*
 * calling_convention.h
 *
 *  Created on: 05.02.2011
 *      Author: Stefan
 */

#ifndef CALLING_CONVENTION_H_
#define CALLING_CONVENTION_H_

#ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
  #define CALLING_CONVENTION _cdecl
#else
  #define CALLING_CONVENTION /* ->empty */
#endif
//#define CALLING_CONVENTION __stdcall

#endif /* CALLING_CONVENTION_H_ */
