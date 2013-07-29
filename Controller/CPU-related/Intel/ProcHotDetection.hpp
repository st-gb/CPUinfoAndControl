/*
 * ProcHotDetection.hpp
 *
 *  Created on: Jul 14, 2013
 *      Author: Stefan
 */

#ifndef PROCHOTDETECTION_HPP_
#define PROCHOTDETECTION_HPP_

#include <multithread/nativeThreadType.hpp>
#include <Controller/Sleep.hpp>
#include <Controller/CPU-related/Intel/Intel_registers.h> //IA32_THERM_STATUS
#include <Controller/multithread/thread_function_calling_convention.h>

nativeThread_type g_PROCHOTdetectionThread;
//Changed via multiple thread-> volatile.
volatile bool g_vbProcHotIsCurrentlyActive = false;
volatile bool g_vbRunProcHotDetectionThread = true;

//TODO possibly insert this code (functions, variables) into a class so that
//Proc hot detection may work for running multiple threads for multiple CPU
// cores .
void EndPROCHOTdetectionThread()
{
  DEBUGN( "EndPROCHOTdetectionThread begin")
  g_vbRunProcHotDetectionThread = false;
  DEBUGN( "Waiting for the termination of the PROCHOTdetection thread")
  //Must wait for the other thread to end, else crash of x86I&C process?!
  void * p_v = g_PROCHOTdetectionThread.WaitForTermination(1000);
  DEBUGN( "EndPROCHOTdetectionThread ret code of g_PROCHOTdetectionThread:"
    << p_v);
}

DWORD THREAD_FUNCTION_CALLING_CONVENTION PROCHOTdetectionThreadFunc(void *)
{
  uint32_t dwEAX, dwEDX;
  DEBUGN( "PROCHOTdetectionThreadFunc begin")
#ifdef _DEBUG
  unsigned times = 0;
#endif
  do
  {
#ifdef _DEBUG
    ++ times;
    if( times % 100 == 0)
      DEBUGN( "PROCHOTdetectionThreadFunc loop")
#endif
    ReadMSR(
      IA32_THERM_STATUS ,
      & dwEAX,     // bit  0-31
      & dwEDX,     // bit 32-63
      //1bin =core 0; 10bin=2dec= core 1
      1  // Thread Affinity Mask
      );
    if( dwEAX & THERMAL_STATUS )
    {
      DEBUGN( "PROCHOT is currently active")
      g_vbProcHotIsCurrentlyActive = true;
    }
    //Does not sleep exactly 1 ms? But waits until thread's time slice ends?
    OperatingSystem::Sleep(1);
  }while(g_vbRunProcHotDetectionThread);
  DEBUGN( "PROCHOTdetectionThreadFunc return 0")
  return 0;
}

void StartPROCHOTdetectionThread()
{
  DEBUGN( "StartPROCHOTdetectionThread")
  g_PROCHOTdetectionThread.start(PROCHOTdetectionThreadFunc, NULL);
}

#endif /* PROCHOTDETECTION_HPP_ */
