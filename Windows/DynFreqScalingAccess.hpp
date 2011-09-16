/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
#pragma once //incl. guard

//Powercfg.exe
//from
// http://www.heise.de/ct/Energie-Einstellungen-nur-als-Admin--/hotline/131373 :
//The settings are stored in the registry:
//"HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Controls Folder
// \PowerCfg"

#include <Controller/IDynFreqScalingAccess.hpp>
#include <global.h> //for BYTE, DWORD
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
