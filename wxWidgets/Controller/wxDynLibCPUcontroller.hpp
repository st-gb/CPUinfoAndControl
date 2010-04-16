#pragma once

#include <Controller/I_CPUcontroller.hpp>
#include <Controller/DynLibCPUcontroller.h>
#include <wx/dynlib.h> 

class I_CPUaccess ;

class wxDynLibCPUcontroller
  : public I_CPUcontroller
{
private:
  dll_GetCurrentPstate_type m_pfngetcurrentpstate ;
  dll_GetNumberOfCPUcores_type m_pfnGetNumberOfCPUcores ;
  dll_SetCurrentPstate_type m_pfnsetcurrentpstate ;
  dll_GetMaximumFrequencyInMHz_type m_pfnGetMaximumFrequencyInMHz ;
  dll_GetMaximumFrequencyInMHz_type m_pfnGetMinimumFrequencyInMHz ;
  dll_GetTemperatureInCelsius_type m_pfngettemperatureincelsius ;
  dll_PrepareForNextPerformanceCounting
    m_pfn_preparefornextperformancecounting ;
//  dynlib_SetPstateFromFreq m_pfn_set_pstate_from_freq ;
  dll_WriteMSR_type m_pfn_write_msr ;

  WORD m_wNumberOfLogicalCPUcores ;
  wxDynamicLibrary m_wxdynamiclibraryCPUctl ;
public:
  wxDynLibCPUcontroller( 
    wxString & r_wxstrFilePath 
    , I_CPUaccess * p_cpuaccess 
    ) ;
  ~wxDynLibCPUcontroller() ;
  void DecreaseVoltageBy1Step(float & r_fVoltage) ;
  BYTE GetCurrentPstate(
    WORD & r_wFreqInMHz 
    , float & Volt
    , BYTE byCoreID 
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
  BYTE 
    //Let voltage be the first element from name because the same in
    //"Dyn Voltage And Freq. Sclaing"
    SetVoltageAndFrequency(
      float fVolt    
      , WORD wFreqInMHz 
      , BYTE byCoreID 
      ) //{return 0 ; }
      ;
  BYTE TooHot() ;
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
