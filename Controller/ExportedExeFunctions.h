/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
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

//from http://stackoverflow.com/questions/911035/uint32-int16-and-the-like-are-they-standard-c:
#include <stdint.h>
//#endif

  typedef BOOL (//WINAPI
    * ReadMSR_func_type)
     (
      DWORD dwIndex,    // MSR index
      uint32_t * p_dweax,     // bit  0-31
      uint32_t * p_dwedx,     // bit 32-63
      DWORD_PTR affinityMask  // Thread Affinity Mask
    ) ;
  typedef BOOL (//WINAPI
    * WriteMSR_func_type)
     (
      DWORD dwIndex,    // MSR index
      uint32_t dwEAX,      // bit  0-31
      uint32_t dwEDX,      // bit 32-63
      DWORD_PTR affinityMask  // Thread Affinity Mask
    ) ;
  typedef BOOL ( * ReadPCIconfigSpace_func_type)
    (
    BYTE byPCIbus , //"8-bit PCI bus",
    //Bits 0- 2 : Function Number
    //Bits 3- 7 : Device Number
    BYTE byDeviceAndFunction ,//"5-bit device, and 3-bit function"
    DWORD dwRegisterAddress ,
    PDWORD p_dwValue ) ;


#endif /* EXPORTEDEXEFUNCTIONS_H_ */
