/*
 * InstableCPUcoreOperationDetection.hpp
 *
 *  Created on: 19.06.2013
 *      Author: Stefan
 */

#ifndef INSTABLECPUCOREOPERATIONDETECTION_HPP_
#define INSTABLECPUCOREOPERATIONDETECTION_HPP_

#include <ModelData/InstableCPUcoreOperationDetection.hpp>

namespace Windows
{
  class InstableCPUcoreOperationDetection
    : public ::InstableCPUcoreOperationDetection
  {
  public:
    ULONG64 ul64ThreadTimeBefore;
    DWORD dwMilliSecondsToWait;
    InstableCPUcoreOperationDetection(CPUcoreData &);
    virtual
    ~InstableCPUcoreOperationDetection();

    void UnloadDynLib()
    {
      if( m_hmoduleUnstableVoltageDetectionDynLib != NULL)
      {
        ::FreeLibrary(m_hmoduleUnstableVoltageDetectionDynLib);
        //For loading the dyn lib in "InitUnstableVoltageDetectionDynLibAccess()".
        m_hmoduleUnstableVoltageDetectionDynLib = NULL;
      }
    }

    StartInstableVoltageDetectionFunctionPointer AssignStartFunctionPointer()
    {
      m_pfnStartInstableCPUcoreVoltageDetection =
        (StartInstableVoltageDetectionFunctionPointer)
        ::GetProcAddress(
          m_hmoduleUnstableVoltageDetectionDynLib, //_T(
          //"TortureTest"
          START_INSTABLE_CPU_CORE_VOLTAGE_DETECTION_FCT_NAME
          //)
          );
      LOGN( /* "--\"" << */ START_INSTABLE_CPU_CORE_VOLTAGE_DETECTION_FCT_NAME
        << "\" function pointer: "
        << (void *) m_pfnStartInstableCPUcoreVoltageDetection)
      return m_pfnStartInstableCPUcoreVoltageDetection;
    }

    StopInstableVoltageDetectionFunctionPointer AssignStopFunctionPointer()
    {
      m_pfnStopInstableCPUcoreVoltageDetection =
        (StopInstableVoltageDetectionFunctionPointer) ::GetProcAddress(
        m_hmoduleUnstableVoltageDetectionDynLib, //_T(
  //        "StopTortureTest"//)
        //"StopInstableVoltageDetection"
        STOP_INSTABLE_CPU_CORE_VOLTAGE_DETECTION_FCT_NAME
        );
      LOGN( /*"--\"" << */ STOP_INSTABLE_CPU_CORE_VOLTAGE_DETECTION_FCT_NAME
        << "\" function pointer: "
        << (void *) m_pfnStopInstableCPUcoreVoltageDetection )
      return m_pfnStopInstableCPUcoreVoltageDetection;
    }

    ULONG64 GetThreadUserModeStartTime(/*void **/);
    float GetCPUcoreUsageForDynLibThread();

    dynlibType LoadDynLib()
    {
      if(m_hmoduleUnstableVoltageDetectionDynLib == NULL)
        m_hmoduleUnstableVoltageDetectionDynLib = //::LoadLibrary(
          //lptstrUnstableVoltageDetectionDynLib
        ::LoadLibraryW( //m_std_wstrInstableCPUcoreVoltageDynLibPath.c_str()
          m_std_wstrDynLibPath.c_str() );
      LOGN( "after loading the dyn lib")
      return m_hmoduleUnstableVoltageDetectionDynLib;
    }
  };

} /* namespace Windows */
#endif /* INSTABLECPUCOREOPERATIONDETECTION_HPP_ */
