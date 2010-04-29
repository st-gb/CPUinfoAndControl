/*
 * PowerProfUntilWin6_DLL_functions_definitions.h
 *
 *  Created on: Apr 28, 2010
 *      Author: Stefan
 */

#ifndef POWERPROFUNTILWIN6_DLL_FUNCTIONS_DEFINITIONS_H_
#define POWERPROFUNTILWIN6_DLL_FUNCTIONS_DEFINITIONS_H_

//g++ has neither "__in" nor "__out"
#ifndef __in
  #define __in
#endif
#ifndef __out
  #define __out
#endif

typedef BOOLEAN (WINAPI * pfnCanUserWritePwrScheme) () ;

typedef BOOLEAN (WINAPI * pfnDeletePwrScheme) (
  UINT uiIndex
  );

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

#endif /* POWERPROFUNTILWIN6_DLL_FUNCTIONS_DEFINITIONS_H_ */
