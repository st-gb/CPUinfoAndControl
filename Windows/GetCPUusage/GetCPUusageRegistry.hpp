/*
 * GetCPUusageRegistry.hpp
 *
 *  Created on: Mar 20, 2010
 *      Author: Stefan
 */

#ifndef GETCPUUSAGEREGISTRY_HPP_
#define GETCPUUSAGEREGISTRY_HPP_

#include <sstream>
#include <iosfwd> //wostringstream
//#include <Windows_compatible_typedefs.h> //DWORD etc.
//#include <winnt.h> //for LONGLONG
#include <windef.h>

// Contains the elements required to calculate a counter value.
typedef struct _rawdata
{
    DWORD CounterType;
    ULONGLONG Data;          // Raw counter data
    LONGLONG Time;           // Is a time value or a base value
    DWORD MultiCounterData;  // Second raw counter value for multi-valued counters
    LONGLONG Frequency;
}RAW_DATA, *PRAW_DATA;

class WinRegistryPerCoreAtts
{
public:
  RAW_DATA Sample1;
  RAW_DATA Sample2;
} ;

class WinRegistryCPUcoreUsageGetter
{
private:
  //Declare some frequently used vars here. So the execution is faster.
  DWORD m_dwAtMask2ndTimeCPUcoreMask ;
  LPWSTR m_lpwstrCPUcoreNumber ;
  //std::wostringstream<wchar_t> m_stdostringstream ;
  std::basic_ostringstream<wchar_t> m_stdowstringstream;
  wchar_t m_ar_wch[20] ;
  WinRegistryPerCoreAtts * m_ar_winregistrypercoreatts ;
public:
  WinRegistryCPUcoreUsageGetter() ;
  ~WinRegistryCPUcoreUsageGetter() ;
  float GetPercentalUsageForCore(
    BYTE byCoreID
    ) ;
  void SetNumberOfCPUcores( WORD wNumLogicalCPUcores ) ;
} ;

#endif /* GETCPUUSAGEREGISTRY_HPP_ */
