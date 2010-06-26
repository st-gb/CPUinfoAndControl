
#include <windef.h> //BYTE
#include <specstrings.h> //for __out_ecount_opt etc. in Pdh.h

#ifndef _MSC_VER
  //for __in_opt , __success(expr) in Pdh.h
  #include <Controller/MSVC_adaption/specstrings_strict.h>
  #include <specstrings.h> //for __out_ecount_opt etc. in Pdh.h
  #define PZZSTR char * //needed for pdh.h
  #define PZZWSTR wchar_t * //needed for pdh.h
#endif //#ifndef _MSC_VER
//#define UNICODE
#include <Pdh.h> //HCOUNTER, HQUERY
//#include <global.h> //for DEBUGN(...)

#ifndef _MSC_VER //because .lib files (->load-time dyn linking)
  //avail. only for MSVC
  #define COMPILE_RUNTIME_DYN_LINKED
#endif

#ifdef COMPILE_RUNTIME_DYN_LINKED
  #include "PDH_DLL_function_signatures.h"
#endif //#ifdef COMPILE_RUNTIME_DYN_LINKED

#ifndef _MSC_VER //MinGW does not have a PDH_MORE_DATA #define
  //from http://msdn.microsoft.com/en-us/library/aa373046%28v=VS.85%29.aspx:
  #define PDH_MORE_DATA 0x800007D2
  #define PDH_INSUFFICIENT_BUFFER 0xC0000BC2

  //from Windows Platform SDK version 6.1's "pdh.h":
  #define PDH_MAX_COUNTER_NAME    1024  // Maximum counter name length.
#endif
void OutputMultiWStringValue(LPWSTR mszObjectList) ;

class PDH_CPUcoreUsageGetterPerCoreAtts
{
public:
  HCOUNTER hCounter;
  HQUERY hQuery;
} ;

class ExceptionWithStdString
{
public:
  std::string m_stdstr ;
  ExceptionWithStdString(const std::string & r_stdstr )
  {
    m_stdstr = r_stdstr ;
  }
} ;

class PDH_CPUcoreUsageGetter
{
//  HCOUNTER hCounter;
  HINSTANCE m_hinstancePDH_DLL ;
//  HQUERY hQuery;
  PDH_CPUcoreUsageGetterPerCoreAtts * m_ar_pdh_cpu_core_usage_getter_per_core_atts ;
#ifdef COMPILE_RUNTIME_DYN_LINKED
  PdhAddCounterW_type m_pfnPdhAddCounterW ;
  PdhCloseQuery_type m_pfnPdhCloseQuery ;
  PdhCollectQueryData_type m_pfnPdhCollectQueryData ;
  PdhEnumObjectsW_type m_pfnPdhEnumObjectsW ;
  PdhGetFormattedCounterValue_type m_pfnPdhGetFormattedCounterValue ;
  PdhLookupPerfNameByIndexW_type m_pfnPdhLookupPerfNameByIndexW ;
  PdhOpenQueryW_type m_pfnPdhOpenQueryW ;
#endif //#ifdef COMPILE_RUNTIME_DYN_LINKED
  LPWSTR m_lpwstrProcentProcessorTimeCounterName ;
  LPWSTR m_lpwstrProcessorPerfObjectName ;
  LPWSTR m_lpwstrCPUcoreNumber ;
  wchar_t m_ar_wch[20] ;
  WORD m_wNumLogicalCPUcores ;
public:
  void AssignDLLfunctionPointers() ;
  void GetCounterObjects() ;
  LPWSTR GetPerfnameByIndex(WORD) ;
  void GetPercentProcessorTimeName() ;
  float GetPercentalUsageForCore(
    BYTE byCoreID
    ) ;
  BYTE GetPDHerrorCodeString(
    DWORD dwErrorCode,std::wstring & wstr ) ;
  PDH_CPUcoreUsageGetter() ;
  ~PDH_CPUcoreUsageGetter() ;
#ifdef COMPILE_RUNTIME_DYN_LINKED
  PDH_FUNCTION
  PdhAddCounterW(
      __in  PDH_HQUERY     hQuery,
      __in  LPCWSTR        szFullCounterPath,
      __in  DWORD_PTR      dwUserData,
      __out PDH_HCOUNTER * phCounter
  );
  PDH_FUNCTION
  PdhCloseQuery(
      __inout PDH_HQUERY hQuery
  );
  PDH_FUNCTION
  PdhCollectQueryData(
      __inout PDH_HQUERY hQuery
  );
  PDH_FUNCTION PdhEnumObjectsW(
    __in_opt LPCWSTR szDataSource,
    __in_opt LPCWSTR szMachineName,
    __out_ecount_opt(* pcchBufferSize) PZZWSTR mszObjectList,
    __inout  LPDWORD pcchBufferSize,
    __in     DWORD   dwDetailLevel,
    __in     BOOL    bRefresh
    ) ;

  PDH_FUNCTION
  PdhGetFormattedCounterValue(
      __in      PDH_HCOUNTER          hCounter,
      __in      DWORD                 dwFormat,
      __out_opt LPDWORD               lpdwType,
      __out     PPDH_FMT_COUNTERVALUE pValue
  );
  PDH_FUNCTION
  PdhLookupPerfNameByIndexW(
    __in_opt LPCWSTR szMachineName,
    __in     DWORD   dwNameIndex,
    __out_ecount_opt(* pcchNameBufferSize) LPWSTR  szNameBuffer,
    __inout  LPDWORD pcchNameBufferSize
    ) ;
  PDH_FUNCTION
  PdhOpenQueryW(
      __in_opt LPCWSTR      szDataSource,
      __in     DWORD_PTR    dwUserData,
      __out    PDH_HQUERY * phQuery
  );
#endif //#ifdef COMPILE_RUNTIME_DYN_LINKED
  BYTE InitPDH_DLLaccess() ;
  void InitPDH_Names() ;
  void SetNumberOfCPUcores( WORD wNumLogicalCPUcores ) ;
  void StartPerfCounting() ;
} ;
