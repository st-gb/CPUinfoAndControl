/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
/*
 * CPUcoreUsageGetterAndControllerBase.h
 *
 *  Created on: 10.04.2010
 *      Author: Stefan
 */

#ifndef USAGEGETTERANDCONTROLLERBASE_H_
#define USAGEGETTERANDCONTROLLERBASE_H_

//#ifdef _WINDOWS
  #include <windef.h>
#include <stdint.h> //uint32_t
//#endif
#ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
   #include <basetsd.h> //DWORD_PTR
#endif //#ifdef _WIN32

class I_CPUaccess ;
//TODO comment out because this class is not being used
class Model;

/** Contains the member that may be needed by each usage getter and controller. */
class CPUcoreUsageGetterAndControllerBase
{
//protected:
public:
//  Model * mp_model ;
  /** The CPU access should be protected against direct access by
  * not inherited classes because writing to an MSR may damage the CPU.
  * A good place for direct write to MSR is the concrete CPU controller
  * (NOT the CPU access class because there may be many of them->
  * against Don't Repeat Yourself principle) class. */
  I_CPUaccess * mp_cpuaccess ;
public:
  CPUcoreUsageGetterAndControllerBase();
  virtual ~CPUcoreUsageGetterAndControllerBase();
  I_CPUaccess * GetCPUaccess() ;
  /** Advantage for a RdmsrEx() inside CPU controller: one does not
  * need to MANUALLY check if cpuaccess is assigned. (not NULL)
  * because this is done by this class' RdmsrEx() method. 
  * @param lowmostBits : Use data type "uint32_t" because this is the exact 
+   *  width of the CPU register  */
  //inline
  bool // TRUE: success, FALSE: failure
   RdmsrEx(
      DWORD dwIndex,        // MSR index
      uint32_t & lowmostBits,            // bit  0-31 (register "EAX")
      uint32_t & highmostBits,           // bit 32-63 (register "EDX")
      DWORD_PTR dwAffinityMask  // Thread Affinity Mask
    ) ;
  /** Possible handling for resume from standby (ACPI S3) or
  * hibernate  (S4). E.g. with Griffin CPUs undervolting
  * is possible without an own DynVoltFreqScal. But  then
  * the performance state registers muste be rewritten after
  * resume. */
  virtual void ResumeFromS3orS4() {} ;
  void SetCPUaccess(
    /** By using a pointer instead of a reference one can pass NULL
    * to show that there is no object. */
    I_CPUaccess * ) ;
};

#endif /* USAGEGETTERANDCONTROLLERBASE_H_ */
