/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
#pragma once

#include <Controller/CPU-related/I_CPUcontroller.hpp>
#include <Controller/DynLibCPUcontroller.hpp> //class DynLibCPUcontroller
//#include <Controller/CPUcontrollerDynLib_function_definitions.h>
#include <wx/dynlib.h> 

class I_CPUaccess ;
class UserInterface ;

class wxDynLibCPUcontroller
  : public I_CPUcontroller
  , public DynLibCPUcontroller
{
private:
//  float m_fReferenceClockInMHz ;
//  dll_GetCurrentPstate_type m_pfngetcurrentpstate ;
//  pfn_GetCurrentVoltageAndFrequency_type m_pfnGetCurrentVoltageAndFrequency ;
//  dll_GetNumberOfCPUcores_type m_pfnGetNumberOfCPUcores ;
////  dll_SetCurrentPstate_type m_pfnsetcurrentpstate ;
//  pfnSetCurrentVoltageAndMultiplier_type m_pfnSetCurrentVoltageAndMultiplier ;
//  pfnGetAvailableMultipliers_type m_pfnGetAvailableMultipliers ;
//  pfnGetAvailableMultipliers_type m_pfnGetAvailableVoltages ;
////  dll_GetMaximumFrequencyInMHz_type m_pfnGetMaximumFrequencyInMHz ;
////  dll_GetMaximumVoltageID_type m_pfnGetMaximumVoltageID ;
////  dll_GetMinimumVoltageID_type m_pfnGetMinimumVoltageID ;
////  dll_GetMaximumFrequencyInMHz_type m_pfnGetMinimumFrequencyInMHz ;
//  dll_GetTemperatureInCelsius_type m_pfngettemperatureincelsius ;
//  dll_PrepareForNextPerformanceCounting
//    m_pfn_preparefornextperformancecounting ;
//  dll_GetVoltageInVolt_type m_pfn_GetVoltageInVolt ;
//  dll_GetVoltageID_type m_pfn_GetVoltageID ;
////  dynlib_SetPstateFromFreq m_pfn_set_pstate_from_freq ;
//  dll_WriteMSR_type m_pfn_write_msr ;

  UserInterface * mp_userinterface ;
  WORD m_wNumberOfLogicalCPUcores ;
  wxDynamicLibrary m_wxdynamiclibraryCPUctl ;
public:
  wxDynLibCPUcontroller( 
    wxString & r_wxstrFilePath 
    , I_CPUaccess * p_cpuaccess 
    , UserInterface * p_userinterface
    , Model * p_model
    ) ;
  ~wxDynLibCPUcontroller() ;

  inline void AssignPointersToDynLibFunctions(I_CPUaccess * p_cpuaccess);
  inline void AssignPointerToDynLibsInitFunction(I_CPUaccess * p_cpuaccess);
  inline void AssignPointerToDynLibsGetAvailableMultipliersFunction();
  inline void AssignPointerToDynLibsGetAvailableVoltagesFunction();
  inline void AssignPointerToDynLibsGetTemperatureInCelsiusFunction();

  void DecreaseVoltageBy1Step(float & r_fVoltage) ;
  void GetAvailableMultipliers(
    //Use a std::set because: in this set the elements are sorted, also:
    //not the releasing memory problem when using this container.
    std::set<float> & r_stdset_float ) ;
  void GetAvailableVoltagesInVolt(
    //Use a std::set because: in this set the elements are sorted, also:
    //not the releasing memory problem when using this container.
    std::set<float> & r_stdset_float ) ;
  BYTE GetCurrentPstate(
    WORD & r_wFreqInMHz 
    , float & Volt
    , BYTE byCoreID 
    ) ;
  //inline
  BYTE GetClosestMultplierAndSetVoltageAndMultiplier(
    float fVoltageInVolt
    , float fMultiplier
    , WORD byCoreID
    ) ;
  inline void GetCurrentReferenceClock();
  BYTE GetCurrentVoltageAndFrequency(
    float & r_fVoltageInVolt
    , float & fMultiplier
    , float & m_fReferenceClockInMHz
    , WORD wCoreID
    ) ;
  WORD GetMaximumFrequencyInMHz() ;
  WORD GetMinimumFrequencyInMHz() ;
  WORD GetMaximumVoltageID() ;
  WORD GetMinimumVoltageID() ;
  WORD GetNumberOfCPUcores() ;
  float GetVoltageInVolt(WORD wVoltageID ) ;
  float GetTemperatureInCelsius(WORD wVoltageID ) ;
  WORD GetVoltageID(float fVoltageInVolt ) ;
  void PrepareForNextPerformanceCounting(
     DWORD dwAffinityBitMask
     , BYTE byPerformanceEventSelectRegisterNumber
     ) ;
  //inline for call from SetVoltageAndFrequency(...)
  //inline
  BYTE SetCurrentVoltageAndMultiplier(
    float fVoltageInVolt
    //multipliers can also be floats: e.g. 5.5 for AMD Griffin.
    , float fMultiplier
    , WORD wCoreID
    ) ;
  BYTE 
    //Let voltage be the first element from name because the same in
    //"Dyn Voltage And Freq. Scaling"
    SetVoltageAndFrequency(
      float fVolt    
      , WORD wFreqInMHz 
      , BYTE byCoreID 
      ) //{return 0 ; }
      ;
  BYTE GetCPUcoreTooHot() ;
  BOOL // TRUE: success, FALSE: failure
  //In g++ virtual methods can't be declared as stdcall
  //WINAPI
  WrmsrEx(
    DWORD index,		// MSR index
    DWORD dwLow ,//eax,			// bit  0-31
    DWORD dwHigh, //edx,			// bit 32-63
    DWORD affinityMask	// Thread Affinity Mask
    ) ;
} ;
