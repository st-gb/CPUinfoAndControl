
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
typedef PDH_STATUS
  ( __stdcall * PdhLookupPerfNameByIndexW_type ) (
    __in_opt LPCWSTR szMachineName,
    __in     DWORD   dwNameIndex,
    __out_ecount_opt(* pcchNameBufferSize) LPWSTR  szNameBuffer,
    __inout  LPDWORD pcchNameBufferSize
    ) ;
#endif //#ifdef COMPILE_RUNTIME_DYN_LINKED

class PDH_CPUcoreUsageGetterPerCoreAtts
{
public:
  HCOUNTER hCounter;
  HQUERY hQuery;
} ;

class PDH_CPUcoreUsageGetter
{
  HCOUNTER hCounter;
  HINSTANCE m_hinstancePDH_DLL ;
  HQUERY hQuery;
  PDH_CPUcoreUsageGetterPerCoreAtts * m_ar_pdh_cpu_core_usage_getter_per_core_atts ;
#ifdef COMPILE_RUNTIME_DYN_LINKED
  PdhLookupPerfNameByIndexW_type m_pfnPdhLookupPerfNameByIndexW ;
#endif //#ifdef COMPILE_RUNTIME_DYN_LINKED
  LPWSTR m_lpwstrProcentProcessorTimeCounterName ;
  LPWSTR m_lpwstrCPUcoreNumber ;
  wchar_t m_ar_wch[20] ;
  WORD m_wNumLogicalCPUcores ;
public:
  void AssignDLLfunctionPointers() ;
  void GetProcentProcessorTimeName() ;
  float GetPercentalUsageForCore(
    BYTE byCoreID
    ) ;
  PDH_CPUcoreUsageGetter() ;
  ~PDH_CPUcoreUsageGetter() ;
#ifdef COMPILE_RUNTIME_DYN_LINKED
  PDH_FUNCTION
  PdhLookupPerfNameByIndexW(
    __in_opt LPCWSTR szMachineName,
    __in     DWORD   dwNameIndex,
    __out_ecount_opt(* pcchNameBufferSize) LPWSTR  szNameBuffer,
    __inout  LPDWORD pcchNameBufferSize
    ) ;
#endif //#ifdef COMPILE_RUNTIME_DYN_LINKED
  void SetNumberOfCPUcores( WORD wNumLogicalCPUcores ) ;
  void StartPerfCounting() ;
} ;
