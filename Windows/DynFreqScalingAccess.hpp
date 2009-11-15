#pragma once //incl. guard

//Powercfg.exe
//http://www.heise.de/ct/Energie-Einstellungen-nur-als-Admin--/hotline/131373
//The settings are stored in the registry:
//HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Controls Folder\PowerCfg

#include "../Controller/IDynFreqScalingAccess.hpp"
#include "global.h" //for BYTE, DWORD
#ifdef COMPILE_WITH_VISTA_POWERPROFILE_ACCESS
  #include <guiddef.h> //for GUID
#endif //#ifdef COMPILE_WITH_VISTA_POWERPROFILE_ACCESS

//#ifndef LINK_TO_POWERPROF_STATICALLY
//
//  //PowerGetActiveScheme * g_pfnPowerGetActiveScheme ;
//  typedef DWORD (WINAPI * pfnPowerGetActiveScheme )(
//    __in_opt HKEY UserRootPowerKey,
//    __deref_out GUID **ActivePolicyGuid
//    );
//#endif

namespace Windows_API
{
  class DynFreqScalingAccess
    :public IDynFreqScalingAccess
{
#ifdef COMPILE_WITH_VISTA_POWERPROFILE_ACCESS
private:
  DWORD m_dwOldACProcThrottleMaxValue;
  DWORD m_dwOldACProcThrottleMinValue;
  DWORD m_dwOldDCProcThrottleMaxValue;
  DWORD m_dwOldDCProcThrottleMinValue;
  //storage is allocated by ::PowerGetActiveScheme(...)
  GUID * mp_guidPowerSchemeBeforeDisabling;
#endif //#ifdef COMPILE_WITH_VISTA_POWERPROFILE_ACCESS
public:
  //void 
    bool DisableFrequencyScalingByOS() ;
  BYTE EnableFrequencyScalingByOS() ;
  bool OSsDVFSisEnabled() ;
  //void SetMaxCPUcoreFreqAsPercentOfMaxFreqForAC(BYTE byPercent) ;
  //void SetMaxCPUcoreFreqAsPercentOfMaxFreqForDC(BYTE byPercent) ;
  //void SetMinCPUcoreFreqAsPercentOfMaxFreqForAC(BYTE byPercent) ;
  //void SetMinCPUcoreFreqAsPercentOfMaxFreqForDC(BYTE byPercent) ;
  ~DynFreqScalingAccess();
};
}
