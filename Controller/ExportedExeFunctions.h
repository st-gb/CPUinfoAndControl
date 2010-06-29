/*
 * ExportedExeFunctions.h
 *
 *  Created on: Jun 4, 2010
 *      Author: Stefan
 *  Copyright 2010 by Trilobyte SE GmbH, Berlin, Germany
 */

#ifndef EXPORTEDEXEFUNCTIONS_H_
#define EXPORTEDEXEFUNCTIONS_H_

//#ifndef _MSC_VER
#include <windef.h> //for BOOL etc.
//#endif

  typedef BOOL (//WINAPI
    * ReadMSR_func_type)
     (
      DWORD dwIndex,    // MSR index
      PDWORD p_dweax,     // bit  0-31
      PDWORD p_dwedx,     // bit 32-63
      DWORD_PTR affinityMask  // Thread Affinity Mask
    ) ;
  typedef BOOL (//WINAPI
    * WriteMSR_func_type)
     (
      DWORD dwIndex,    // MSR index
      DWORD dwEAX,      // bit  0-31
      DWORD dwEDX,      // bit 32-63
      DWORD_PTR affinityMask  // Thread Affinity Mask
    ) ;

#endif /* EXPORTEDEXEFUNCTIONS_H_ */
