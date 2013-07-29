/*
 * InstableCPUcoreOperationDetection.hpp
 *
 *  Created on: 22.04.2012
 *      Author: Stefan
 */

#ifndef INSTABLECPUCOREVOLTAGEDETECTION_HPP_
#define INSTABLECPUCOREVOLTAGEDETECTION_HPP_

//#include "VoltageAndFreq.hpp" //class VoltageAndFreq
#include <Controller/multithread/thread_type.hpp>
//THREAD_PROC_CALLING_CONVENTION
#include <preprocessor_macros/thread_proc_calling_convention.h>
#include <preprocessor_macros/logging_preprocessor_macros.h>
//struct external_caller, StartInstableVoltageDetectionFunctionPointer,
// StopInstableVoltageDetectionFunctionPointer
#include <InstableCPUcoreVoltageDefintions.h>
#include <UserInterface/UserInterface.hpp>
#include <Controller/multithread/condition_type.hpp>
//#include ""

//Forward decl.
class CPUcoreData;
class I_CPUcontroller;

/** Used for displaying the progress of finding the lowest voltage for an
 *  instable CPU operation detection status in GUI. */
struct InstableCPUoperationDetectionData
{
//protected:
public:
  unsigned m_secondsUntilCPUcoreVoltageDecrease;
  float m_fCPUcoreUsageOfDynLibThread;
};

class wxX86InfoAndControlApp;

class InstableCPUcoreOperationDetection
  : public InstableCPUoperationDetectionData
{
public:
  /** Set to true if executable was called by instable CPU core op detection dyn lib */
  volatile bool m_bVoltageWasTooLowCalled;
  volatile bool m_bStopFindingLowestStableCPUcoreVoltageRequestedViaUI;
  //Is set to true when "VoltageTooLow" was called.
  volatile bool m_vbExitFindLowestStableVoltage;
  static InstableCPUcoreOperationDetection * s_p_instableCPUcoreOperationDetection;
  uint32_t m_ui32CPUcoreMask;
  CPUcoreData & m_r_cpucoredata;
  I_CPUcontroller * m_p_cpucontroller;
//  float m_fCPUcoreUsageOfDynLibThread;
  /** Use unsigned datatype because same bit width as CPU arch.->fast. */
  unsigned m_secondsUntilVoltageDecrease;
  unsigned m_milliSecondsToWaitBeforeSecondsCountDown;
#ifdef _WIN32
  //TODO replace with wxDynLib
  typedef HMODULE dynlibType;
#endif
  typedef wxX86InfoAndControlApp userinterface_type;
  //UserInterface * m_p_userinterface;
  userinterface_type * m_p_userinterface;
  dynlibType m_hmoduleUnstableVoltageDetectionDynLib;
  StartInstableVoltageDetectionFunctionPointer
    m_pfnStartInstableCPUcoreVoltageDetection;
  StopInstableVoltageDetectionFunctionPointer
    m_pfnStopInstableCPUcoreVoltageDetection;
  struct external_caller m_external_caller;

  condition_type m_conditionFindLowestStableVoltage;

  //http://forums.wxwidgets.org/viewtopic.php?t=4824:
//  wxMutex m_wxmutexFindLowestStableVoltage;
//  wxCondition m_wxconditionFindLowestStableVoltage;
  x86IandC::thread_type m_x86iandc_threadFindLowestStableVoltage;
  x86IandC::thread_type m_x86iandc_threadFindLowestStableCPUcoreOperationInDLL;

  unsigned m_uiNumberOfSecondsToWaitUntilVoltageIsReduced;
  //std::wstring m_std_wstrInstableCPUcoreVoltageDynLibPath;
  std::wstring m_std_wstrDynLibPath;
//  VoltageAndFreq m_lastSetVoltageAndFreq;

  //TODO if the reference clock changes after starting to find an unstable
  //CPU core voltage then (if the ref. clock in heightened) it can be still
  //instable. But on the other hand: if the calculation of the ref. clock is
  //inaccurate then it is better to set to the same multiplier as at start of
  //find lowest voltage.

  //Use separate values for both multi and ref clock rather than a single
  //composed value for the frequency. because the mult
  float m_lastSetCPUcoreMultiplier;
  float m_lastSetCPUcoreVoltageInVolt;
  float m_fReferenceClockInMHz;
  /** min CPU core usage for unstable CPU operation detection thread needed */
  float m_fMinCPUcoreUsage;

  InstableCPUcoreOperationDetection(/*UserInterface * p_ui*/
    CPUcoreData & r_cpucoredata);

  /** Must be virtual because else g++ warning:
   * `class InstableCPUcoreOperationDetection' has virtual functions
   * but non-virtual destructor */
  virtual ~InstableCPUcoreOperationDetection() {}

  void CountSecondsDown();
  float DecreaseVoltageStepByStep(
    unsigned wCPUcoreVoltageArrayIndex, const float fMultiplier);

  bool DynLibAccessInitialized()
  {
    return m_hmoduleUnstableVoltageDetectionDynLib &&
      m_pfnStartInstableCPUcoreVoltageDetection &&
      m_pfnStopInstableCPUcoreVoltageDetection;
  }

  void ExitFindLowestStableVoltageThread()
  {
    //Exit the "find lowest stable voltage" thread.
    m_vbExitFindLowestStableVoltage = true;
  //          wxGetApp().m_wxconditionFindLowestStableVoltage.Signal();
    LOGN( FULL_FUNC_NAME << "--signalling the condition to end finding the"
        " lowest stable voltage thread")
    //Wake up all threads waiting on the condition.
    m_conditionFindLowestStableVoltage.Broadcast();
    LOGN( FULL_FUNC_NAME << " after signalling the condition to end finding the"
        " lowest stable voltage thread")
  }

  virtual dynlibType LoadDynLib() = 0;

  bool DynLibSuccessFullyLoaded()
  {
    return m_hmoduleUnstableVoltageDetectionDynLib != NULL;
  }

  static DWORD THREAD_PROC_CALLING_CONVENTION FindLowestStableVoltage_ThreadProc(
    void * p_v );
  DWORD FindLowestStableVoltage();

  virtual ULONG64 GetThreadUserModeStartTime(/*void **/) = 0;
  virtual float GetCPUcoreUsageForDynLibThread() = 0;

  void HandleVoltageTooLow();

  bool IsRunning()
  {
    return m_x86iandc_threadFindLowestStableVoltage.IsRunning();
  }

  void SetStopRequestedViaGUI(bool b)
  {
    m_bStopFindingLowestStableCPUcoreVoltageRequestedViaUI = b;
  }

  void SetUserInterface(/*UserInterface * p_ui*/ userinterface_type * p_ui);

  const x86IandC::thread_type & StartInDynLibInSeparateThread();

  void StartInDynLib(/*uint16_t m_ui32CPUcoreMask*/);
  /** Must be static as thread proc. */
  static DWORD THREAD_PROC_CALLING_CONVENTION
    StartInDynLib_ThreadProc(void * p_v );

  void Stop();

  virtual void UnloadDynLib() = 0;

  void ShowMessage(const wchar_t * const msg);
  BYTE Start();

  virtual StartInstableVoltageDetectionFunctionPointer AssignStartFunctionPointer() = 0;

  virtual StopInstableVoltageDetectionFunctionPointer AssignStopFunctionPointer() = 0;
  /** Called by "instable CPU core operation" dyn lib */
  static void VoltageTooLow();
};

#endif /* INSTABLECPUCOREVOLTAGEDETECTION_HPP_ */
