#pragma once 

#include <Controller/IDynFreqScalingAccess.hpp>
#include <Controller/stdtstr.hpp> //for std::tstring 
//#include <Windows/PowerProfDynLinked.hpp>
#include <Windows/PowerProf/I_PowerProfDynLinked.hpp>
#include <Windows.h> //for ULONG in Powerprof.h WINAPI, ::Sleep(...)
#include <Powrprof.h>

#define WSTR_PWRSCHEMESENUMPROC
#ifdef WSTR_PWRSCHEMESENUMPROC
  typedef BOOLEAN (CALLBACK* PWRSCHEMESENUMPROC_SG)(
    UINT, DWORD, LPWSTR, DWORD, LPWSTR, PPOWER_POLICY, LPARAM);
#else
  typedef BOOLEAN (CALLBACK* PWRSCHEMESENUMPROC_SG)(
    UINT, DWORD, LPTSTR, DWORD, LPTSTR, PPOWER_POLICY, LPARAM);
#endif
#include <string>

#define __in
#define __out 

typedef BOOLEAN (WINAPI * pfnCanUserWritePwrScheme) () ;

typedef BOOLEAN (WINAPI * pfnEnumPwrSchemes)(
  //PWRSCHEMESENUMPROC 
  PWRSCHEMESENUMPROC_SG lpfnPwrSchemesEnumProc,
  LPARAM lParam
);

//http://msdn.microsoft.com/en-us/library/aa372688(VS.85).aspx:
typedef BOOLEAN (WINAPI * pfnGetActivePwrScheme)(
  __out  PUINT puiID
  );

//http://msdn.microsoft.com/en-us/library/aa372689(VS.85).aspx
typedef BOOLEAN (WINAPI * pfnGetCurrentPowerPolicies) (
  __out  PGLOBAL_POWER_POLICY pGlobalPowerPolicy,
  __out  PPOWER_POLICY pPowerPolicy
  );

typedef BOOLEAN (WINAPI * pfnReadProcessorPwrScheme)(
  __in   UINT uiID,
  __out  PMACHINE_PROCESSOR_POWER_POLICY pMachineProcessorPowerPolicy
);

typedef BOOLEAN (WINAPI * pfnReadPwrScheme) (
  __in   UINT uiID,
  __out  PPOWER_POLICY pPowerPolicy
);

typedef BOOLEAN (WINAPI * pfnSetActivePwrScheme)(
  UINT uiID,
  PGLOBAL_POWER_POLICY lpGlobalPowerPolicy,
  PPOWER_POLICY lpPowerPolicy
);

typedef BOOLEAN (WINAPI * pfnWritePwrScheme) (
  PUINT puiID,
  //LPTSTR lpszName,
  //LPTSTR lpszDescription,
  //e.g. WinXP uses wide chars for service name (and for descr.?)
  LPWSTR lpszName ,
  LPWSTR lpszDescription ,
  PPOWER_POLICY pPowerPolicy
);

////ms-help://MS.VSCC.v80/MS.MSDN.v80/MS.WIN32COM.v10.en/power/base/enumpwrschemes.htm:
////The EnumPwrSchemes function enumerates all power schemes. 
////For each power scheme enumerated, the function calls a callback function 
////with information about the power scheme.
//BOOLEAN EnumPwrSchemes(
//  PWRSCHEMESENUMPROC lpfnPwrSchemesEnumProc,
//  LPARAM lParam
//);


//ms-help://MS.VSCC.v80/MS.MSDN.v80/MS.WIN32COM.v10.en/power/base/enumpwrschemes.htm:
//For each power scheme enumerated, the callback function is called with the following parameters:
//BOOLEAN CALLBACK PwrSchemesEnumProc(
//  UINT uiIndex,      // power scheme index
//  DWORD dwName,      // size of the sName string, in bytes
//  LPWSTR sName,      // name of the power scheme
//  DWORD dwDesc,      // size of the sDesc string, in bytes
//  LPWSTR sDesc,      // description string
//  PPOWER_POLICY pp,  // receives the power policy
//  LPARAM lParam      // user-defined value
//{
//  
//}

class PowerProfUntilWin6DynLinked
  : //public IDynFreqScalingAccess
  //: public PowerProfDynLinked
  //, 
  public I_PowerProfDynLinked
{
  bool m_bGotPowerSchemeBeforeDisabling ;
  HINSTANCE m_hinstancePowerProfDLL ;
  pfnCanUserWritePwrScheme m_pfncanuserwritepwrscheme ;
  pfnEnumPwrSchemes m_pfnenumpwrschemes ;
  pfnGetActivePwrScheme m_pfngetactivepwrscheme ;
  pfnGetCurrentPowerPolicies m_pfngetcurrentpowerpolicies ;
  pfnReadProcessorPwrScheme m_pfnreadprocessorpwrscheme ;
  //WriteProcessorPwrScheme(...) has the same function sig as 
  //ReadProcessorPwrScheme(...)
  pfnReadProcessorPwrScheme m_pfnwriteprocessorpwrscheme ;
  pfnReadPwrScheme m_pfnreadpwrscheme ;
  pfnSetActivePwrScheme m_pfnsetactivepwrscheme ;
	pfnWritePwrScheme m_pfnwritepwrscheme ;
  UINT m_uiPowerSchemeIDBeforeDisabling ;
public:
  bool m_bPowerSchemeWithP0ThrottleNoneFound ;
  bool m_bPowerSchemeFound ;
  BYTE m_byDynamicThrottle ;
  UINT m_uiPowerSchemeIndex ;
  UINT m_uiPowerSchemeIndexWithP0ThrottleNone ;
  UINT m_uiPowerSchemeIndexOfWantedName ;
  std::string m_strWantedPowerScheme ;
  std::wstring m_stdwstrWantedPowerScheme ;

  BYTE ActivatePowerSchemeToSet() ;
  BOOLEAN WINAPI CanUserWritePwrScheme(void);

  BYTE CreatePowerScheme(
		//LPTSTR lpszName ,
		//LPTSTR lpszDescription ,
    //e.g. WinXP uses wide chars for service name (and for descr.?)
    LPWSTR lpszName ,
    //LPCWSTR lpszName ,
    LPWSTR lpszDescription ,
    UINT & uiPowerSchemeID
		) ;
  BYTE CreatePowerSchemeWithWantedName() ;
  void DeactivateCPUscaling(
    PROCESSOR_POWER_POLICY & r_processor_power_policy ) ;
  BYTE DisableDVFSforPowerSchemeToSet() ;
  BYTE DisableCPUscaling(UINT uiPowerSchemeIndex) ;
  bool DisableFrequencyScalingByOS() //{ return true ; } 
    ;
  unsigned char EnableFrequencyScalingByOS() ;
  BOOLEAN EnumPwrSchemes(
    //PWRSCHEMESENUMPROC 
    PWRSCHEMESENUMPROC_SG lpfnPwrSchemesEnumProc,
    LPARAM lParam
    );
  BOOLEAN GetCurrentPowerPolicies(
    PGLOBAL_POWER_POLICY pGlobalPowerPolicy,
    PPOWER_POLICY pPowerPolicy
  );

  BOOLEAN WINAPI GetActivePwrScheme(
    __out  PUINT puiID
    );
  void Initialize() ;
  void InitializeFunctionPointers() ;
  BYTE PowerSchemeToSetExists() ;
  BOOLEAN WINAPI ReadPwrScheme (
    __in   UINT uiID,
    __out  PPOWER_POLICY pPowerPolicy
  );
  bool OtherDVFSisEnabled() ;

  PowerProfUntilWin6DynLinked(
    //std::wstring & r_stdwstrPowerSchemeName
    std::tstring & r_stdtstrPowerSchemeName
    ) ;
  BOOLEAN WINAPI ReadProcessorPwrScheme(
    __in   UINT uiPowerSchemeID,
    __out  PMACHINE_PROCESSOR_POWER_POLICY pMachineProcessorPowerPolicy
    );
  BOOLEAN SetActivePwrScheme(
    UINT uiID,
    PGLOBAL_POWER_POLICY lpGlobalPowerPolicy,
    PPOWER_POLICY lpPowerPolicy
    ) ;
  BOOLEAN SetActivePwrScheme(
    //LPTSTR strPowerSchemeName     // name of the power scheme
    std::wstring & r_stdwstrPowerSchemeName     // name of the power scheme
    ) ;
  void SetFunctionPointersToNULL() ;
  BOOLEAN WINAPI WriteProcessorPwrScheme(
    UINT uiPowerSchemeID, 
    PMACHINE_PROCESSOR_POWER_POLICY);
	BOOLEAN WritePwrScheme(
		PUINT puiID,
		//LPTSTR lpszName,
		//LPTSTR lpszDescription,
    //e.g. WinXP uses wide chars for service name (and for descr.?)
    LPWSTR lpszName ,
    LPWSTR lpszDescription ,
		PPOWER_POLICY pPowerPolicy
	);
};
