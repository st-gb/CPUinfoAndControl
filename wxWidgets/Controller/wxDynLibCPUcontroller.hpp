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
  WORD GetVoltageID(float fVoltageInVolt ) ;
  BYTE 
    //Let voltage be the first element from name because the same in
    //"Dyn Voltage And Freq. Sclaing"
    SetVoltageAndFrequency(
      float fVolt    
      , WORD wFreqInMHz 
      , BYTE byCoreID 
      ) //{return 0 ; }
      ;
} ;
