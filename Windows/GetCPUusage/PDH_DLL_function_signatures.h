/*
 * PDH_DLL_function_signatures.h
 *
 *  Created on: Apr 30, 2010
 *      Author: Stefan
 */

#ifndef PDH_DLL_FUNCTION_SIGNATURES_H_
#define PDH_DLL_FUNCTION_SIGNATURES_H_

#include <Pdh.h> //HCOUNTER, HQUERY

//from pdh.h:
//PDH_FUNCTION -> "PDH_STATUS ( __stdcall * ... )"

typedef PDH_STATUS ( __stdcall * PdhAddCounterW_type) (
    __in  PDH_HQUERY     hQuery,
    __in  LPCWSTR        szFullCounterPath,
    __in  DWORD_PTR      dwUserData,
    __out PDH_HCOUNTER * phCounter
);
typedef PDH_STATUS ( __stdcall * PdhCloseQuery_type) (
    __inout PDH_HQUERY hQuery
);
typedef PDH_STATUS ( __stdcall * PdhCollectQueryData_type) (
    __inout PDH_HQUERY hQuery
);
typedef PDH_STATUS (  __stdcall * PdhEnumObjectsW_type) (
    __in_opt LPCWSTR szDataSource,
    __in_opt LPCWSTR szMachineName,
    __out_ecount_opt(* pcchBufferSize) PZZWSTR mszObjectList,
    __inout  LPDWORD pcchBufferSize,
    __in     DWORD   dwDetailLevel,
    __in     BOOL    bRefresh
);
typedef PDH_STATUS
  ( __stdcall * PdhGetFormattedCounterValue_type) (
    __in      PDH_HCOUNTER          hCounter,
    __in      DWORD                 dwFormat,
    __out_opt LPDWORD               lpdwType,
    __out     PPDH_FMT_COUNTERVALUE pValue
);
typedef PDH_STATUS
  ( __stdcall * PdhOpenQueryW_type) (
    __in_opt LPCWSTR      szDataSource,
    __in     DWORD_PTR    dwUserData,
    __out    PDH_HQUERY * phQuery
);
typedef PDH_STATUS
  ( __stdcall * PdhLookupPerfNameByIndexW_type ) (
    __in_opt LPCWSTR szMachineName,
    __in     DWORD   dwNameIndex,
    __out_ecount_opt(* pcchNameBufferSize) LPWSTR  szNameBuffer,
    __inout  LPDWORD pcchNameBufferSize
    ) ;

#endif /* PDH_DLL_FUNCTION_SIGNATURES_H_ */
