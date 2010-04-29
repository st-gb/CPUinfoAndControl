//#include <Windows.h> //for ::GetProcAddress(), ::GetSystemPowerStatus(), <PowrProf.h>
//Use "windows.h" because g++ (MinGW) is case sensitive.
#include <windows.h> //for ::GetProcAddress(), ::GetSystemPowerStatus(), <PowrProf.h>
#include "global.h"
#include <stdio.h> //wprintf()
#include <tchar.h> //for "_T(...)"
////This must be the PowrProf.h from the Windows (platform) SDK for Vista, 
////ie. SDK version "6.1" .
//#include <PowrProf.h> // for PowerWriteACValueIndex()
#include "PowerProfFromWin6DynLinked.hpp"
#include <Controller/tchar_conversion.h> //GetCharPointer()
//#include <Controller/stdtstr.hpp> //for std::tstring 
#include <exception>

  //HINSTANCE m_hinstancePowerProfDLL ;

// from Windows 6.1 Platform SDK's "winnt.h" :
//#ifdef INITGUID
#ifdef DEFINE_GUID
  #undef DEFINE_GUID
  #define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
        EXTERN_C const GUID DECLSPEC_SELECTANY name \
                = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }
#endif
//#else
//Use C comment, else compiler warning: multi-line comment because of "\" at
// line end.
/*#define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
//    EXTERN_C const GUID FAR name
//#endif // INITGUID */
//
// Specifies the subgroup which will contain all of the processor
// settings for a single policy.
//
DEFINE_GUID( GUID_PROCESSOR_SETTINGS_SUBGROUP, 0x54533251, 0x82BE, 0x4824, 0x96, 0xC1, 0x47, 0xB6, 0x0B, 0x74, 0x0D, 0x00 );
//
// Specifies a percentage (between 0 and 100) that the processor frequency
// should never go above.  For example, if this value is set to 80, then
// the processor frequency will never be throttled above 80 percent of its
// maximum frequency by the system.
//
DEFINE_GUID( GUID_PROCESSOR_THROTTLE_MAXIMUM, 0xBC5038F7, 0x23E0, 0x4960, 0x96, 0xDA, 0x33, 0xAB, 0xAF, 0x59, 0x35, 0xEC );

// Specifies a percentage (between 0 and 100) that the processor frequency
// should not drop below.  For example, if this value is set to 50, then the
// processor frequency will never be throttled below 50 percent of its
// maximum frequency by the system.
//
DEFINE_GUID( GUID_PROCESSOR_THROTTLE_MINIMUM, 0x893DEE8E, 0x2BEF, 0x41E0, 0x89, 0xC6, 0xB5, 0x5D, 0x09, 0x29, 0x96, 0x4C );

BYTE PowerProfFromWin6DynLinked::ActivatePowerSchemeToSet()
{
  PowerGetActiveScheme( NULL, m_guidPowerSchemeBeforeDisabling ) ;
  mp_guidPowerSchemeBeforeDisabling = & m_guidPowerSchemeBeforeDisabling ;
  //if( m_guidPowerSchemeBeforeDisabling == m_guidPowerSchemeToSet )
  PowerSetActiveScheme( NULL, & m_guidPowerSchemeToSet ) ;
  return 0 ;
}

//Return value: step at which the failure was.
BYTE PowerProfFromWin6DynLinked::CreatePowerScheme(
  const std::wstring & cr_stdwstrPowerSchemeName
  //GetLastError() error code.
  , DWORD & r_dwRetValue
  , GUID & r_guidNewPowerScheme
  )
{
  //return 0 ;
  ULONG ulIndex = 0 ;
  DWORD dwPowerEnumerateRet ;
  UCHAR * p_uchBuffer ;
  GUID GUIDscheme ;
//  GUID guidNewPowerScheme ;
  GUID * p_guidNewPowerScheme = NULL ;
  std::wstring stdwstr ;
//http://msdn.microsoft.com/en-us/library/aa372730(VS.85).aspx:
//"This function is normally called in a loop incrementing the Index parameter
//to retrieve subkeys until they've all been enumerated."
  do
  {
    dwPowerEnumerateRet = PowerEnumerate(
      & GUIDscheme //__in_opt   const GUID *SchemeGuid,
      //NULL
      , NULL //__in_opt   const GUID *SubGroupOfPowerSettingsGuid,
      //ACCESS_SCHEME: Enumerate power schemes. The
      //SchemeGuid and
      //SubgroupOfPowerSettingsGuid
      //parameters will be ignored."
      , ACCESS_SCHEME //__in POWER_DATA_ACCESSOR AccessFlags,
      , ulIndex //__in       ULONG ulIndex,
      //A pointer to a variable to receive the elements. If this parameter is NULL,
      //the function retrieves the size of the buffer required.
      , p_uchBuffer //UCHAR * & Buffer
      ) ;
    ++ ulIndex ;
  }while( dwPowerEnumerateRet != ERROR_SUCCESS ) ;
  #ifdef _DEBUG
  ulIndex = ulIndex ; //instr. is only for breakpoint
  #endif

  r_dwRetValue =
    //http://msdn.microsoft.com/en-us/library/aa372729%28VS.85%29.aspx:
    //"Returns ERROR_SUCCESS (zero) if the call was successful, and a non-zero
    //value if the call failed."
    PowerDuplicateScheme(
    //http://msdn.microsoft.com/en-us/library/aa372729%28VS.85%29.aspx:
    //"This parameter is reserved for future use and must be set to NULL."
    NULL , //__in_opt  HKEY RootPowerKey,
    & GUIDscheme //__in      const GUID *SourceSchemeGuid,
    , & p_guidNewPowerScheme //__in      GUID **DestinationSchemeGuid
  );
  r_guidNewPowerScheme = * p_guidNewPowerScheme ;
  //ERROR_ALREADY_EXISTS  183 (0xB7)
  if( r_dwRetValue == ERROR_SUCCESS )
  {
    DWORD dwBufferSize = ( 
      //"(DWORD" to avoid "warning C4267: 'initializing' : conversion from 'size_t' to 
      //'DWORD', possible loss of data"
      (DWORD) cr_stdwstrPowerSchemeName.length() 
    ////MSVC: wstring::length(): size in bytes
    ////gcc: wstring::length(): size in characters
    //#ifndef _MSC_VER
      //terminating NULL character
      + 1 )
      * 2 
    //#endif
      ;
//    const wchar_t * p_wch = cr_stdwstrPowerSchemeName.c_str() ;
    //dwBufferSize = wcslen( p_wch ) ;
    r_dwRetValue =
      //"Returns ERROR_SUCCESS (zero) if the call was successful, and a non-zero
      //value if the call failed."
      PowerWriteFriendlyName(
      NULL , //__in_opt  HKEY RootPowerKey,
      //http://msdn.microsoft.com/en-us/library/aa372773%28VS.85%29.aspx:
      //"If the SchemeGuid parameter is not NULL but both the
      //SubGroupOfPowerSettingsGuid and PowerSettingGuid parameters are NULL, the
      //friendly name of the power scheme will be set."
      p_guidNewPowerScheme //__in      const GUID *SchemeGuid,
      , NULL //__in_opt  const GUID *SubGroupOfPowerSettingsGuid,
      , NULL  //__in_opt  const GUID *PowerSettingGuid,
      , (UCHAR *) cr_stdwstrPowerSchemeName.c_str() //__in      UCHAR *Buffer,
      //The size in bytes.
      , 
      //cr_stdwstrPowerSchemeName.length()
      ////1 wide char has 2 bytes-> "*2"
      //* 2 //__in      DWORD BufferSize
      //http://msdn.microsoft.com/en-us/library/aa372773%28VS.85%29.aspx:
      //"The size of the friendly name specified by the Buffer parameter, 
      //including the terminating NULL character."
      dwBufferSize
    );
    //"If the pointer contains NULL, the function allocates memory for a new
    //GUID and puts the address of this memory in the pointer. The caller can
    //free this memory using LocalFree."
    LocalFree( p_guidNewPowerScheme ) ;
    if( r_dwRetValue == ERROR_SUCCESS )
    {
      //guidNewPowerScheme = * p_guidNewPowerScheme ;
      return CREATE_POWER_SCHEME_SUCCEEDED ;
    }
  }
  else
    return 2 ;
  return 1 ;
}

//Return: 0 = success
unsigned char PowerProfFromWin6DynLinked::CreatePowerSchemeWithWantedName()
{
  BYTE byRet = 1 ;
  DWORD dwRetValue ;
  GUID guidNewPowerScheme ;
  byRet = CreatePowerScheme(
    m_stdwstrPowerSchemeName //const std::wstring & cr_stdwstrPowerSchemeName
    , dwRetValue //DWORD & r_dwRetValue
    , guidNewPowerScheme
    ) ;
  if( byRet == CREATE_POWER_SCHEME_SUCCEEDED )
  {
    m_guidPowerSchemeToSet = guidNewPowerScheme ;
  }
  return byRet ;
}

BYTE PowerProfFromWin6DynLinked::DeletePowerScheme( 
  const std::tstring & cr_stdtstrPowerSchemeName )
{
  return 0 ;
//  //http://msdn.microsoft.com/en-us/library/aa372727%28VS.85%29.aspx:
//  //"Returns ERROR_SUCCESS (zero) if the call was successful, and a non-zero
//  //value if the call failed."
//  PowerDeleteScheme(
//    //http://msdn.microsoft.com/en-us/library/aa372727%28VS.85%29.aspx:
//    //"This parameter is reserved for future use and must be set to NULL."
//    NULL //__in_opt  HKEY RootPowerKey,
//    , __in      const GUID *SchemeGuid
//    );
}

//Inherited from class PowerProfDynLinked.
//bool PowerProfFromWin6DynLinked::DisableFrequencyScalingByOS()
//{
//  bool bSuccess = false ;
//  //bool bOtherDVFSisEnabled = true ;
//  //GUID * p_guidActivePowerScheme ;
//  GUID guidActivePowerScheme ;
//  try
//  {
//    DWORD dwBufferSize , dwRes ;
//    GUID guidPowerScheme ;
//    GUID guidSubGroupOfPowerSettings ;
//    std::wstring stdwstrPowerScheme, stdwstrWantedPowerScheme ; //= "CPUcontrol" ;
//    UCHAR * Buffer ;
//    POWER_DATA_ACCESSOR AccessFlags ;
//    ULONG ulIndex = 0 ;
//    stdwstrWantedPowerScheme = //GetCharPointer(
//      //GetStdString(
//      //m_stdwstrPowerSchemeName.c_str() ) ;
//      m_stdwstrPowerSchemeName ;
////    //Walk through all power schemes.
////    do
////    {
////      dwRes = PowerEnumerate(
////        & guidPowerScheme ,
////        & guidSubGroupOfPowerSettings ,//__in_opt   const GUID * ,
////        AccessFlags ,//__in       POWER_DATA_ACCESSOR AccessFlags,
////        ulIndex ,//__in       ULONG ulIndex,
////        //A pointer to a variable to receive the elements. If this parameter
////        //is NULL, the function retrieves the size of the buffer required.
////        Buffer //__out_opt  UCHAR *Buffer,
////        ) ;
////      //Get the name of the power scheme with GUID "guidPowerScheme".
////      if( GetPowerSchemeName(guidPowerScheme, stdwstrPowerScheme ) ==
////        ERROR_SUCCESS )
////        if( stdwstrPowerScheme == stdwstrWantedPowerScheme )
////          break ;
////      ulIndex ++ ;
////    }
////    while( dwRes == ERROR_SUCCESS) ;
//    if( PowerSchemeToSetExists() )
//    {
//    GetThrottleSettings(
//      m_dwACProcThrottleMaxValue
//      , m_dwACProcThrottleMinValue
//      , m_dwDCProcThrottleMaxValue
//      , m_dwDCProcThrottleMinValue
//      ) ;
//    }
//    m_bThrottleSettingsAssigned = true ;
//    if ( PowerGetActiveScheme(
//      //This parameter is reserved for future use and must be set to NULL.
//      NULL,
//      //&pActivePolicyGuid
//      guidActivePowerScheme ) == ERROR_SUCCESS
//      )
//    {
//      m_bActivePowerSchemeAssigned = true ;
//      m_guidPowerSchemeBeforeDisabling = guidActivePowerScheme ;
//      DWORD dwProcThrottleValue = 25 ;
//      SYSTEM_POWER_STATUS system_power_status ;
//      if( //ACPower()
//        //If the function succeeds, the return value is nonzero.
//        ::GetSystemPowerStatus(& system_power_status)
//        )
//      {
//        //DWORD dwACProcThrottleMaxValue ;
//        if( system_power_status.ACLineStatus == 1 // Online
//          )
//        {
//          DWORD dwACProcThrottleMaxValue ;
//          DWORD dwACProcThrottleMinValue ;
//          DisableDVFSforAlternateCurrent( & guidActivePowerScheme ,
//            dwProcThrottleValue ) ;
//          //if( dwACProcThrottleMaxValue == dwACProcThrottleMinValue )
//          //  bOtherDVFSisEnabled = false ;
//        }
//        else if( system_power_status.ACLineStatus == //Offline
//          0 )
//        {
//          DWORD dwACProcThrottleMaxValue ;
//          DWORD dwACProcThrottleMinValue ;
//          DisableDVFSforDirectCurrent( & guidActivePowerScheme ,
//            dwProcThrottleValue ) ;
//          //if( dwACProcThrottleMaxValue == dwACProcThrottleMinValue )
//          //  bOtherDVFSisEnabled = false ;
//        }
//      }
//      //::LocalFree( p_guidActivePowerScheme);
//    }
//  }
//  catch( DLLfunctionPointerNotAssignedException e )
//  {
//
//  }
//  return //bOtherDVFSisEnabled ;
//    bSuccess ;
//}

BYTE PowerProfFromWin6DynLinked::DisableDVFSforAlternateCurrent(
  const GUID * cp_guidPowerScheme
  , DWORD dwProcThrottleValue )
{
  BYTE bSuccess = 0 ;
  //Returns ERROR_SUCCESS (zero) if the call was successful, and a non-zero value if the call failed.
  //GUID *pPwrGUID;
  //PowerGetActiveScheme(NULL, &pPwrGUID);
  if( PowerWriteACValueIndex(
    //RootPowerKey: This parameter is reserved for future use and must be set to NULL.
    NULL,
    //__in      const GUID *SchemeGuid,
    //pPwrGUID,
    //pActivePolicyGuid,
    cp_guidPowerScheme ,
    //__in_opt  const GUID *SubGroupOfPowerSettingsGuid,
    & GUID_PROCESSOR_SETTINGS_SUBGROUP,
    //__in_opt  const GUID *PowerSettingGuid,
    //PROCTHROTTLEMAX,
    & GUID_PROCESSOR_THROTTLE_MAXIMUM,
    //__in      DWORD AcValueIndex
    dwProcThrottleValue
    ) == ERROR_SUCCESS
    )
  {
    if(
      PowerWriteACValueIndex(
      //RootPowerKey: This parameter is reserved for future use and must be set to NULL.
      NULL,
      //__in      const GUID *SchemeGuid,
      //pPwrGUID,
      //pActivePolicyGuid,
      cp_guidPowerScheme ,
      //__in_opt  const GUID *SubGroupOfPowerSettingsGuid,
      & GUID_PROCESSOR_SETTINGS_SUBGROUP,
      //__in_opt  const GUID *PowerSettingGuid,
      //PROCTHROTTLEMAX,
      & GUID_PROCESSOR_THROTTLE_MINIMUM,
      //__in      DWORD AcValueIndex
      dwProcThrottleValue
      ) == ERROR_SUCCESS
      )
      bSuccess = true ;
  }
  return bSuccess ;
}

BYTE PowerProfFromWin6DynLinked::DisableDVFSforDirectCurrent(
  const GUID * cp_guidPowerScheme
  , DWORD dwProcThrottleValue )
{
  BYTE bSuccess = 0 ;
  if( PowerWriteDCValueIndex(
    //RootPowerKey: This parameter is reserved for future use and must be set to NULL.
    NULL,
    //__in      const GUID *SchemeGuid,
    //pPwrGUID,
    //pActivePolicyGuid,
    cp_guidPowerScheme ,
    //__in_opt  const GUID *SubGroupOfPowerSettingsGuid,
    & GUID_PROCESSOR_SETTINGS_SUBGROUP,
    //__in_opt  const GUID *PowerSettingGuid,
    //PROCTHROTTLEMAX,
    & GUID_PROCESSOR_THROTTLE_MAXIMUM,
    //__in      DWORD AcValueIndex
    dwProcThrottleValue
    ) ==  ERROR_SUCCESS
    )
  {
    if( PowerWriteDCValueIndex(
      //RootPowerKey: This parameter is reserved for future use and must be set to NULL.
      NULL,
      //__in      const GUID *SchemeGuid,
      //pPwrGUID,
      //pActivePolicyGuid,
      cp_guidPowerScheme ,
      //__in_opt  const GUID *SubGroupOfPowerSettingsGuid,
      & GUID_PROCESSOR_SETTINGS_SUBGROUP,
      //__in_opt  const GUID *PowerSettingGuid,
      //PROCTHROTTLEMAX,
      & GUID_PROCESSOR_THROTTLE_MINIMUM,
      //__in      DWORD AcValueIndex
      dwProcThrottleValue
      ) ==  ERROR_SUCCESS
      )
      bSuccess = true ;
  }
  return bSuccess ;
}

BYTE PowerProfFromWin6DynLinked::DisableDVFSforPowerSchemeToSet()
{
  GUID guidActivePowerScheme ;
//  if ( PowerSetActiveScheme(
//    //This parameter is reserved for future use and must be set to NULL.
//    NULL,
//    //&pActivePolicyGuid
//    & m_guidPowerSchemeToSet ) == ERROR_SUCCESS
//    )
  {
    if ( PowerGetActiveScheme(
      //This parameter is reserved for future use and must be set to NULL.
      NULL,
      //&pActivePolicyGuid
      guidActivePowerScheme ) == ERROR_SUCCESS
      //&& guidActivePowerScheme == m_guidPowerSchemeToSet
      )
    {
      bool bWriteSetting = false ;
      GetThrottleSettings(
        guidActivePowerScheme
        , m_dwACProcThrottleMaxValue
        , m_dwACProcThrottleMinValue
        , m_dwDCProcThrottleMaxValue
        , m_dwDCProcThrottleMinValue
        ) ;
      //The current setting may be the "max. perf" scheme e.g.
      if( m_dwACProcThrottleMaxValue != m_dwACProcThrottleMinValue )
        if( guidActivePowerScheme == m_guidPowerSchemeToSet )
        {
          //GUID guidActivePowerScheme ;
          //PowerGetActiveScheme( NULL , guidActivePowerScheme ) ;
          //if( mp_guidPowerSchemeToSet )
          {
      //      PowerSetActiveScheme( NULL ,
      //        //If the scheme for the wanted name was retrieved once, use its GUID for
      //        //setting the scheme. The name of the scheme may be altered, but its GUID
      //        //stays the same.
      //        //mp_guidPowerSchemeToSet
      //        & m_guidPowerSchemeToSet ) ;
            //DisableDVFSforAlternateCurrent( //mp_guidPowerSchemeToSet ,
            //  & m_guidPowerSchemeToSet ,
            //  50 ) ;
            bWriteSetting = true ;
          }
        }
        else
        {
          //The settings for the power scheme for THIS program.
          GetThrottleSettings(
            m_guidPowerSchemeToSet
            , m_dwACProcThrottleMaxValue
            , m_dwACProcThrottleMinValue
            , m_dwDCProcThrottleMaxValue
            , m_dwDCProcThrottleMinValue
            ) ;
          if( m_dwACProcThrottleMaxValue != m_dwACProcThrottleMinValue )
            bWriteSetting = true ;
        }
      if( bWriteSetting )
        DisableDVFSforAlternateCurrent( //mp_guidPowerSchemeToSet ,
          & m_guidPowerSchemeToSet ,
          50 ) ;
      bWriteSetting = false ;
      if( m_dwDCProcThrottleMaxValue != m_dwDCProcThrottleMinValue )
      {
        if( guidActivePowerScheme == m_guidPowerSchemeToSet )
          bWriteSetting = true ;
      }
      else
      {
        //The settings for the power scheme for THIS program.
        GetThrottleSettings(
          m_guidPowerSchemeToSet
          , m_dwACProcThrottleMaxValue
          , m_dwACProcThrottleMinValue
          , m_dwDCProcThrottleMaxValue
          , m_dwDCProcThrottleMinValue
          ) ;
       if( m_dwDCProcThrottleMaxValue != m_dwDCProcThrottleMinValue )
         bWriteSetting = false ;
     }
      if( bWriteSetting )
        DisableDVFSforDirectCurrent( //& guidActivePowerScheme ,
          & m_guidPowerSchemeToSet ,
          50 ) ;
    }
  }
  return 1 ;
}

unsigned char PowerProfFromWin6DynLinked::EnableFrequencyScalingByOS()
{
  //BYTE bySuccess = 0 ;
  BYTE byRet = IDynFreqScalingAccess::enable_failure ;
  DWORD dwRet ;
//  if( m_bThrottleSettingsAssigned  && m_bActivePowerSchemeAssigned )
//  {
//    bySuccess = 1 ;
//    if( PowerWriteACValueIndex(
//      //RootPowerKey: This parameter is reserved for future use and must be set to NULL.
//      NULL,
//      //__in      const GUID *SchemeGuid,
//      //pPwrGUID,
//      //pActivePolicyGuid,
//      & m_guidPowerSchemeBeforeDisabling ,
//      //__in_opt  const GUID *SubGroupOfPowerSettingsGuid,
//      & GUID_PROCESSOR_SETTINGS_SUBGROUP,
//      //__in_opt  const GUID *PowerSettingGuid,
//      //PROCTHROTTLEMAX,
//      & GUID_PROCESSOR_THROTTLE_MAXIMUM,
//      //__in      DWORD AcValueIndex
//      m_dwACProcThrottleMaxValue
//      ) != ERROR_SUCCESS
//      )
//      bySuccess = 0 ;
//
//    if( PowerWriteACValueIndex(
//      //RootPowerKey: This parameter is reserved for future use and must be set to NULL.
//      NULL,
//      //__in      const GUID *SchemeGuid,
//      //pPwrGUID,
//      //pActivePolicyGuid,
//      & m_guidPowerSchemeBeforeDisabling ,
//      //__in_opt  const GUID *SubGroupOfPowerSettingsGuid,
//      & GUID_PROCESSOR_SETTINGS_SUBGROUP,
//      //__in_opt  const GUID *PowerSettingGuid,
//      //PROCTHROTTLEMAX,
//      & GUID_PROCESSOR_THROTTLE_MINIMUM,
//      //__in      DWORD AcValueIndex
//      m_dwACProcThrottleMinValue
//      ) == ERROR_SUCCESS
//      )
//      bySuccess = 0 ;
//
//    if( PowerWriteDCValueIndex(
//      //RootPowerKey: This parameter is reserved for future use and must be set to NULL.
//      NULL,
//      //__in      const GUID *SchemeGuid,
//      //pPwrGUID,
//      //pActivePolicyGuid,
//      & m_guidPowerSchemeBeforeDisabling ,
//      //__in_opt  const GUID *SubGroupOfPowerSettingsGuid,
//      & GUID_PROCESSOR_SETTINGS_SUBGROUP,
//      //__in_opt  const GUID *PowerSettingGuid,
//      //PROCTHROTTLEMAX,
//      & GUID_PROCESSOR_THROTTLE_MAXIMUM,
//      //__in      DWORD AcValueIndex
//      m_dwDCProcThrottleMaxValue
//      ) != ERROR_SUCCESS
//      )
//      bySuccess = 0 ;
//
//    if( PowerWriteDCValueIndex(
//      //RootPowerKey: This parameter is reserved for future use and must be set to NULL.
//      NULL,
//      //__in      const GUID *SchemeGuid,
//      //pPwrGUID,
//      //pActivePolicyGuid,
//      & m_guidPowerSchemeBeforeDisabling ,
//      //__in_opt  const GUID *SubGroupOfPowerSettingsGuid,
//      & GUID_PROCESSOR_SETTINGS_SUBGROUP,
//      //__in_opt  const GUID *PowerSettingGuid,
//      //PROCTHROTTLEMAX,
//      & GUID_PROCESSOR_THROTTLE_MINIMUM,
//      //__in      DWORD AcValueIndex
//      m_dwDCProcThrottleMinValue
//      ) == ERROR_SUCCESS
//      )
//      bySuccess = 0 ;
//  }
  dwRet = PowerSetActiveScheme( NULL, & 
    //TODO: if this program was started and the DVFS was disabled yet
    //then this programs knows no valid guid for enabling the DVFS.
    m_guidPowerSchemeBeforeDisabling ) ;
  GUID guidActiveScheme ;
  if( PowerGetActiveScheme( NULL , guidActiveScheme ) == ERROR_SUCCESS )
  {
    if( guidActiveScheme == m_guidPowerSchemeBeforeDisabling )
      byRet = IDynFreqScalingAccess::already_enabled ;
  }
  if( dwRet == ERROR_SUCCESS )
    //bySuccess = 1 ;
    byRet = IDynFreqScalingAccess::enable_success ;
  return //bySuccess ;
    byRet ;
}

bool PowerProfFromWin6DynLinked::EnablingIsPossible()
{
  bool bEnablingIsPossible = false ;
  if( mp_guidPowerSchemeBeforeDisabling )
  {
    std::wstring stdwstrPowerSchemeName ;
    GetPowerSchemeName( * mp_guidPowerSchemeBeforeDisabling 
      , stdwstrPowerSchemeName ) ;
    if( stdwstrPowerSchemeName != m_stdwstrPowerSchemeName )
      bEnablingIsPossible = true ;
  }
  return bEnablingIsPossible ;
}

//std::tstring 
IDynFreqScalingAccess::string_type PowerProfFromWin6DynLinked::GetEnableDescription()
{
  std::tstring stdtstrReturn(_T("no other power scheme available") ) ;
  if( mp_guidPowerSchemeBeforeDisabling )
  {
    std::wstring stdwstrPowerSchemeName ;
    GetPowerSchemeName( * mp_guidPowerSchemeBeforeDisabling 
      , stdwstrPowerSchemeName ) ;
    if( stdwstrPowerSchemeName == m_stdwstrPowerSchemeName )
      ;
    else
      stdtstrReturn = _T("set power scheme \"") 
        + Getstdtstring( stdwstrPowerSchemeName )
        + _T("\"") ;
  }
  return stdtstrReturn ;
}

//Return: ERROR_SUCCESS if everything succeeded.
DWORD PowerProfFromWin6DynLinked::GetPowerSchemeName(
  GUID & guidPowerScheme , std::wstring & r_stdwstr )
{
  DWORD dwBufferSize ;
  DWORD dwRes = 
    //double-call-approach: 
    //1st call: get the number of bytes needed for allocating
    //the buffer.
    PowerReadFriendlyName(
    //http://msdn.microsoft.com/en-us/library/aa372740%28VS.85%29.aspx:
    //"This parameter is reserved for future use and must be set to NULL."
    NULL , //__in_opt   HKEY RootPowerKey
    //http://msdn.microsoft.com/en-us/library/aa372740(VS.85).aspx:
    //If the SchemeGuid parameter is not NULL but both the 
    //SubGroupOfPowerSettingsGuid and PowerSettingGuid parameters are 
    //NULL, the friendly name of the power scheme will be returned.
    & guidPowerScheme ,
    NULL ,//__in_opt   const GUID *SubGroupOfPowerSettingsGuid,
    NULL ,//__in_opt   const GUID *PowerSettingGuid,
    //http://msdn.microsoft.com/en-us/library/aa372740%28VS.85%29.aspx:
    //"If this parameter is NULL, the BufferSize parameter receives the required
    //buffer size."
    NULL ,//__out_opt  PUCHAR Buffer,
    //If the Buffer parameter is NULL, the function returns ERROR_SUCCESS 
    //and the variable receives the required buffer size.
    & dwBufferSize //__inout    LPDWORD BufferSize
    ) ;
  if( dwRes == ERROR_SUCCESS
    //ERROR_MORE_DATA
    )
  {
    PUCHAR ar_ucharBuffer = new UCHAR[dwBufferSize] ;
    dwRes = PowerReadFriendlyName(
      NULL ,
      //http://msdn.microsoft.com/en-us/library/aa372740(VS.85).aspx:
      //If the SchemeGuid parameter is not NULL but both the 
      //SubGroupOfPowerSettingsGuid and PowerSettingGuid parameters are 
      //NULL, the friendly name of the power scheme will be returned.
      & guidPowerScheme ,
      NULL ,//__in_opt   const GUID *SubGroupOfPowerSettingsGuid,
      NULL ,//__in_opt   const GUID *PowerSettingGuid,
      ar_ucharBuffer ,//__out_opt  PUCHAR Buffer,
      //If the Buffer parameter is NULL, the function returns ERROR_SUCCESS and the variable receives the required buffer size. 
      & dwBufferSize //__inout    LPDWORD BufferSize
      ) ;
    if( dwRes == ERROR_SUCCESS )
      //Power Profile names are wide strings (2 bytes per character).
      //so often a "\0" (that would terminate if interpreted as single byte
      //string) follows the first byte.
      r_stdwstr = std::wstring( //(char *)
        (wchar_t *) ar_ucharBuffer ) ;
    //Release the allocated memory.
    delete [] ar_ucharBuffer ;
  }
  return dwRes ;
}

void PowerProfFromWin6DynLinked:://OutputAllPowerSchemes()
  GetAllPowerSchemeNames(std::set<std::wstring> & stdset_stdwstrPowerSchemeName )
{
  ULONG ulIndex = 0 ;
  DWORD dwPowerEnumerateRet ;
  UCHAR * p_uchBuffer ;
  GUID GUIDscheme ;
  std::wstring stdwstr ;
//http://msdn.microsoft.com/en-us/library/aa372730(VS.85).aspx:
//"This function is normally called in a loop incrementing the Index parameter
//to retrieve subkeys until they've all been enumerated."
  do
  {
    dwPowerEnumerateRet = PowerEnumerate(
      & GUIDscheme //__in_opt   const GUID *SchemeGuid,
      //NULL
      , NULL //__in_opt   const GUID *SubGroupOfPowerSettingsGuid,
      //ACCESS_SCHEME: Enumerate power schemes. The
      //SchemeGuid and
      //SubgroupOfPowerSettingsGuid
      //parameters will be ignored."
      , ACCESS_SCHEME //__in POWER_DATA_ACCESSOR AccessFlags,
      , ulIndex //__in       ULONG ulIndex,
      //A pointer to a variable to receive the elements. If this parameter is NULL,
      //the function retrieves the size of the buffer required.
      , p_uchBuffer //UCHAR * & Buffer
      ) ;
    if( dwPowerEnumerateRet == ERROR_SUCCESS )
    {
  //    PowerReadFriendlyName(
  //      NULL
  //      , NULL
  //      , p_uchBuffer
  //      ) ;
  //    //Release memory.
  //    delete [] p_uchBuffer ;
      GetPowerSchemeName(GUIDscheme, stdwstr ) ;
      #if defined(_DEBUG)
        #if defined(__MINGW32__) //MinGW does not know "wcout"
//          wprintf(L"%s\n", stdwstr.c_str() ) ;
        #else
          std::wcout << stdwstr << L'\n' ;
        #endif
      #endif
      stdset_stdwstrPowerSchemeName.insert(stdwstr) ;
      ++ ulIndex ;
    }
  }while( dwPowerEnumerateRet == ERROR_SUCCESS ) ;
  DEBUGN(  "Numer of power schemes:" << ( ulIndex + 1 ) )
  #ifdef _DEBUG
  ulIndex = ulIndex ; //instr. is only for breakpoint
  #endif
}

//Implements the access via the runtime-dynamic loaded powerprof.DLL.
//Has the same function signature as here:
//http://msdn.microsoft.com/en-us/library/aa372730(VS.85).aspx:
//"This function is normally called in a loop incrementing the Index parameter
//to retrieve subkeys until they've all been enumerated."
//"Returns ERROR_SUCCESS (zero) if the call was successful, and a non-zero value
//if the call failed. If the buffer size passed in the BufferSize parameter is
//too small, or if the Buffer parameter is NULL, ERROR_MORE_DATA will be
//returned and the DWORD pointed to by the BufferSize parameter will be filled
//in with the required buffer size."
DWORD PowerProfFromWin6DynLinked::PowerEnumerate(
  __in_opt   HKEY RootPowerKey,
  __in_opt   const GUID *SchemeGuid,
  __in_opt   const GUID *SubGroupOfPowerSettingsGuid,
  __in       POWER_DATA_ACCESSOR AccessFlags,
  __in       ULONG Index,
  //A pointer to a variable to receive the elements. If this parameter is NULL,
  //the function retrieves the size of the buffer required.
  __out_opt  UCHAR *Buffer,
  //A pointer to a variable that on input contains the size of the buffer
  //pointed to by the Buffer parameter. If the Buffer parameter is NULL or if
  //the BufferSize is not large enough, the function will return
  //ERROR_MORE_DATA and the variable receives the required buffer size.
  __inout    DWORD *BufferSize
  )
{
  DWORD dw ;
  if( m_pfnpowerenumerate )
  {
    dw = (*m_pfnpowerenumerate) (
      //This parameter is reserved for future use and must be set to NULL.
      NULL ,//RootPowerKey,
      //http://msdn.microsoft.com/en-us/library/aa372730%28VS.85%29.aspx:
      //"The identifier of the power scheme. If this parameter is NULL, an
      //enumeration of the power policies is returned."
      SchemeGuid,
      SubGroupOfPowerSettingsGuid,
      AccessFlags,
      Index,
      Buffer,
      BufferSize );
  }
  else
    throw //std::exception ;
      DLLfunctionPointerNotAssignedException() ;
  return dw ;
}

DWORD PowerProfFromWin6DynLinked::PowerDeleteScheme(
  __in_opt  HKEY RootPowerKey,
  __in      const GUID *SchemeGuid
  )
{
  return 0 ;
}

DWORD WINAPI PowerProfFromWin6DynLinked::PowerDuplicateScheme(
  __in_opt  HKEY RootPowerKey ,
  __in      const GUID * SourceSchemeGuid ,
  __in      GUID ** DestinationSchemeGuid
  )
{
  //if( )
  return (*m_pfnpowerduplicatescheme) (
    RootPowerKey ,
    SourceSchemeGuid ,
    DestinationSchemeGuid
    ) ;
}

//Helper function for PowerEnumerate to handle the buffer allocation.
//Calling this function is easier to call "PowrProf.DLL"'s PowerEnumerate
//function. (indirectly)
DWORD PowerProfFromWin6DynLinked::PowerEnumerate(
  __in_opt   GUID * SchemeGuid,
  __in_opt   const GUID *SubGroupOfPowerSettingsGuid,
  __in       POWER_DATA_ACCESSOR AccessFlags,
  __in       ULONG ulIndex,
  //A pointer to a variable to receive the elements. If this parameter is NULL,
  //the function retrieves the size of the buffer required.
  UCHAR * & Buffer
  )
{
  DWORD dwBufferSize ;
  DWORD dwRes = PowerEnumerate(
    NULL ,
    //SchemeGuid,
    NULL ,
    SubGroupOfPowerSettingsGuid,
    AccessFlags,
    ulIndex,
    //A pointer to a variable to receive the elements. If this parameter is
    //NULL, the function retrieves the size of the buffer required.
    NULL ,//__out_opt  UCHAR *Buffer,
    //A pointer to a variable that on input contains the size of the buffer
    //pointed to by the Buffer parameter. If the Buffer parameter is NULL or if
    //the BufferSize is not large enough, the function will return
    //ERROR_MORE_DATA and the variable receives the required buffer size.
    & dwBufferSize
    ) ;
  if ( dwRes == //ERROR_SUCCESS
    ERROR_MORE_DATA )
  {
    Buffer = new UCHAR[dwBufferSize] ;
    //dwBufferSize usually gets 16. That is the same size as GUIDs.
    dwRes = PowerEnumerate(
      NULL ,
      //SchemeGuid,
      NULL ,
      SubGroupOfPowerSettingsGuid,
      AccessFlags,
      ulIndex,
      //A pointer to a variable to receive the elements. If this parameter is
      //NULL, the function retrieves the size of the buffer required.
      Buffer ,//__out_opt  UCHAR *Buffer,
      //A pointer to a variable that on input contains the size of the buffer
      //pointed to by the Buffer parameter. If the Buffer parameter is NULL or
      //if the BufferSize is not large enough, the function will return
      //ERROR_MORE_DATA and the variable receives the required buffer size.
      & dwBufferSize
      ) ;
    * SchemeGuid = * ( (GUID *) Buffer ) ;
    delete [] Buffer ;
  }
  return dwRes ;
}

PowerProfFromWin6DynLinked::PowerProfFromWin6DynLinked(
  //std::wstring & r_stdwstrPowerSchemeName
  std::tstring & r_stdtstrPowerSchemeName
  )
  :
  //Initialize in the same order as textual in the declaration?
  //(to avoid g++ warnings)
  m_bActivePowerSchemeAssigned ( false )
  , m_bThrottleSettingsAssigned ( false )
  , mp_guidPowerSchemeBeforeDisabling ( NULL )
{
  //m_stdwstrPowerSchemeName = r_stdwstrPowerSchemeName ;
#ifdef  UNICODE                     // r_winnt
  m_stdwstrPowerSchemeName = //r_stdwstrPowerSchemeName ;
    r_stdtstrPowerSchemeName ;
#else
  m_stdwstrPowerSchemeName = //r_stdwstrPowerSchemeName ;
    //http://www.wer-weiss-was.de/theme158/article3047390.html:
    std::wstring ( r_stdtstrPowerSchemeName.begin(), 
      r_stdtstrPowerSchemeName.end() );
#endif
  m_hinstancePowerProfDLL = 
    //If the function fails, the return value is NULL.
    ::LoadLibraryA( "PowrProf.dll" //LPCSTR 
      );
  if( m_hinstancePowerProfDLL )
  {
    InitializeFunctionPointers();
    if( PowerGetActiveScheme( NULL, m_guidPowerSchemeBeforeDisabling ) == 
      ERROR_SUCCESS )
      mp_guidPowerSchemeBeforeDisabling = & m_guidPowerSchemeBeforeDisabling ;
  }
  else
    SetFunctionPointersToNULL();
}

void PowerProfFromWin6DynLinked::InitializeFunctionPointers()
{
  std::string strFuncName = "PowerGetActiveScheme" ;
  m_pfnpowergetactivescheme = (pfnPowerGetActiveScheme) ::GetProcAddress( 
    m_hinstancePowerProfDLL, strFuncName.c_str() );

  strFuncName = "PowerDeleteScheme" ;
  m_pfnpowerdeletescheme = (pfnPowerDeleteScheme) ::GetProcAddress(
    m_hinstancePowerProfDLL, strFuncName.c_str() );

  strFuncName = "PowerDuplicateScheme" ;
  m_pfnpowerduplicatescheme = (pfnPowerDuplicateScheme) ::GetProcAddress(
    m_hinstancePowerProfDLL, strFuncName.c_str() );

  strFuncName = "PowerEnumerate" ;
  m_pfnpowerenumerate = ( pfnPowerEnumerate) ::GetProcAddress( 
    m_hinstancePowerProfDLL, strFuncName.c_str() );

  strFuncName = "PowerReadACValueIndex" ;
  m_pfnpowerreadacvalueindex = (pfnPowerReadACValueIndex) ::GetProcAddress( 
    m_hinstancePowerProfDLL, strFuncName.c_str() );

  strFuncName = "PowerReadDCValueIndex" ;
  m_pfnpowerreaddcvalueindex = (
    //PowerReadDCValueIndex(...) has the same signature as 
    //PowerReadACValueIndex(...)
    pfnPowerReadACValueIndex) ::GetProcAddress( 
    m_hinstancePowerProfDLL, strFuncName.c_str() );

  strFuncName = "PowerReadFriendlyName" ;
  m_pfnpowerreadfriendlyname = (
    pfnPowerReadFriendlyName) ::GetProcAddress(
    m_hinstancePowerProfDLL, strFuncName.c_str() );

  strFuncName = "PowerSetActiveScheme" ;
  m_pfnpowersetactivescheme = (
    pfnPowerSetActiveScheme) ::GetProcAddress(
    m_hinstancePowerProfDLL, strFuncName.c_str() );

  strFuncName = "PowerSettingAccessCheck " ;
  m_pfnpowersettingaccesscheck = (pfnPowerSettingAccessCheck) ::GetProcAddress( 
    m_hinstancePowerProfDLL, strFuncName.c_str() );

  strFuncName = "PowerWriteACValueIndex" ;
  m_pfnpowerwriteacvalueindex = (pfnPowerWriteACValueIndex) ::GetProcAddress( 
    m_hinstancePowerProfDLL, strFuncName.c_str() );

  strFuncName = "PowerWriteDCValueIndex" ;
  m_pfnpowerwritedcvalueindex = (pfnPowerWriteACValueIndex) ::GetProcAddress( 
    m_hinstancePowerProfDLL, strFuncName.c_str() );

  strFuncName = "PowerWriteFriendlyName" ;
  m_pfnpowerwritefriendlyname = (pfnPowerWriteFriendlyName) ::GetProcAddress(
    m_hinstancePowerProfDLL, strFuncName.c_str() );
}

DWORD
  WINAPI
  PowerProfFromWin6DynLinked::PowerGetActiveScheme (
      __in_opt HKEY UserRootPowerKey,
      __deref_out GUID ** pp_guid_ActivePolicy
      )
{
  DWORD dw ;
  //std::string strFuncName = _T("PowerGetActiveScheme") ;
  //m_pfnpowergetactivescheme = ( ) ::GetProcAddress( m_hinstancePowerProfDLL,
  //  strFuncName.c_str() );
  if( m_pfnpowergetactivescheme )
  {
    dw = (*m_pfnpowergetactivescheme) (
      UserRootPowerKey 
      , pp_guid_ActivePolicy 
      ) ;
  }
  else
    throw //std::exception ;
      DLLfunctionPointerNotAssignedException() ;
  return dw ;
}

DWORD WINAPI PowerProfFromWin6DynLinked::PowerGetActiveScheme (
  __in_opt HKEY UserRootPowerKey,
  __deref_out GUID & r_guid_ActivePolicy
  )
{
  DWORD dw ;
  //std::string strFuncName = _T("PowerGetActiveScheme") ;
  //m_pfnpowergetactivescheme = ( ) ::GetProcAddress( m_hinstancePowerProfDLL,
  //  strFuncName.c_str() );
  if( m_pfnpowergetactivescheme )
  {
    GUID * p_guid_ActivePolicy ;
    dw = (*m_pfnpowergetactivescheme) (
      UserRootPowerKey 
      , & p_guid_ActivePolicy 
      ) ;
    if ( dw == ERROR_SUCCESS )
    {
      r_guid_ActivePolicy = * p_guid_ActivePolicy ;
      LocalFree(p_guid_ActivePolicy) ;
    }
  }
  else
    throw //std::exception ;
      DLLfunctionPointerNotAssignedException() ;
  return dw ;
}

DWORD WINAPI
  PowerProfFromWin6DynLinked::PowerReadACValueIndex (
    __in_opt HKEY RootPowerKey,
    __in_opt CONST GUID *SchemeGuid,
    __in_opt CONST GUID *SubGroupOfPowerSettingsGuid,
    __in_opt CONST GUID *PowerSettingGuid,
    __out LPDWORD AcValueIndex
    )
{
  DWORD dw ;
  if( m_pfnpowerreadacvalueindex )
    return (*m_pfnpowerreadacvalueindex) 
      (
      RootPowerKey,
      SchemeGuid,
      SubGroupOfPowerSettingsGuid,
      PowerSettingGuid ,
      AcValueIndex
      ) ;
  else
    throw //exception ;
      DLLfunctionPointerNotAssignedException() ;
  return dw ;
}

DWORD WINAPI
  PowerProfFromWin6DynLinked::PowerReadDCValueIndex (
    __in_opt HKEY RootPowerKey,
    __in_opt CONST GUID *SchemeGuid,
    __in_opt CONST GUID *SubGroupOfPowerSettingsGuid,
    __in_opt CONST GUID *PowerSettingGuid,
    __out LPDWORD AcValueIndex
    )
{
  if( m_pfnpowerreadacvalueindex )
    return (*m_pfnpowerreadacvalueindex) 
      (
      RootPowerKey,
      SchemeGuid,
      SubGroupOfPowerSettingsGuid,
      PowerSettingGuid ,
      AcValueIndex
      ) ;
  else
    throw //exception ;
      DLLfunctionPointerNotAssignedException() ;
}

//Overwrites bool IDynFreqScalingAccess::OtherDVFSisEnabled()
bool PowerProfFromWin6DynLinked::OtherDVFSisEnabled()
{
  bool bOtherDVFSisEnabled = true ;
  //GUID * p_guidActivePowerScheme ;
  GUID guidActivePowerScheme ;
  try
  {
    if ( PowerGetActiveScheme(
      //This parameter is reserved for future use and must be set to NULL.
      NULL, 
      //&pActivePolicyGuid
      guidActivePowerScheme ) == ERROR_SUCCESS 
      )
    {
      SYSTEM_POWER_STATUS system_power_status ;
      if( //ACPower() 
        //If the function succeeds, the return value is nonzero.
        ::GetSystemPowerStatus(& system_power_status)
        )
      {
        //DWORD dwACProcThrottleMaxValue ;
        if( system_power_status.ACLineStatus == 1 // Online
          )
        {
          DWORD dwACProcThrottleMaxValue ;
          DWORD dwACProcThrottleMinValue ;
          //Returns ERROR_SUCCESS (zero) if the call was successful, and a non-zero value if the call failed.
          //GUID *pPwrGUID;
          //PowerGetActiveScheme(NULL, &pPwrGUID);
          PowerReadACValueIndex (
            //__in_opt HKEY RootPowerKey: This parameter is reserved for 
            //future use and must be set to NULL.
            NULL,
            //__in_opt CONST GUID *SchemeGuid,
            //pActivePolicyGuid,
            //p_guidActivePowerScheme ,
            & guidActivePowerScheme ,
            //__in_opt CONST GUID *SubGroupOfPowerSettingsGuid,
            & GUID_PROCESSOR_SETTINGS_SUBGROUP ,
            //__in_opt CONST GUID *PowerSettingGuid,
            & GUID_PROCESSOR_THROTTLE_MAXIMUM ,
            //__out LPDWORD AcValueIndex
            & dwACProcThrottleMaxValue
            );
          PowerReadACValueIndex (
            //__in_opt HKEY RootPowerKey: This parameter is reserved for 
            //future use and must be set to NULL.
            NULL,
            //__in_opt CONST GUID *SchemeGuid,
            //pActivePolicyGuid,
            //p_guidActivePowerScheme ,
            & guidActivePowerScheme ,
            //__in_opt CONST GUID *SubGroupOfPowerSettingsGuid,
            & GUID_PROCESSOR_SETTINGS_SUBGROUP ,
            //__in_opt CONST GUID *PowerSettingGuid,
            & GUID_PROCESSOR_THROTTLE_MINIMUM ,
            //__out LPDWORD AcValueIndex
            & dwACProcThrottleMinValue
            );
          if( dwACProcThrottleMaxValue == dwACProcThrottleMinValue )
            bOtherDVFSisEnabled = false ;
        }
        else if( system_power_status.ACLineStatus == //Offline
          0 )
        {
          DWORD dwACProcThrottleMaxValue ;
          DWORD dwACProcThrottleMinValue ;
          PowerReadDCValueIndex (
            //__in_opt HKEY RootPowerKey: This parameter is reserved for future use and must be set to NULL.
            NULL,
            //__in_opt CONST GUID *SchemeGuid,
            //pActivePolicyGuid,
            //p_guidActivePowerScheme ,
            & guidActivePowerScheme ,
            //__in_opt CONST GUID *SubGroupOfPowerSettingsGuid,
            & GUID_PROCESSOR_SETTINGS_SUBGROUP ,
            //__in_opt CONST GUID *PowerSettingGuid,
            & GUID_PROCESSOR_THROTTLE_MAXIMUM ,
            //__out LPDWORD AcValueIndex
            & dwACProcThrottleMaxValue
            );
          PowerReadDCValueIndex (
            //__in_opt HKEY RootPowerKey: This parameter is reserved for future use and must be set to NULL.
            NULL,
            //__in_opt CONST GUID *SchemeGuid,
            //pActivePolicyGuid,
            //p_guidActivePowerScheme ,
            & guidActivePowerScheme ,
            //__in_opt CONST GUID *SubGroupOfPowerSettingsGuid,
            & GUID_PROCESSOR_SETTINGS_SUBGROUP ,
            //__in_opt CONST GUID *PowerSettingGuid,
            & GUID_PROCESSOR_THROTTLE_MINIMUM ,
            //__out LPDWORD AcValueIndex
            & dwACProcThrottleMinValue
            );
          if( dwACProcThrottleMaxValue == dwACProcThrottleMinValue )
            bOtherDVFSisEnabled = false ;
        }
      }
      //::LocalFree( p_guidActivePowerScheme);
    }
  }
  catch( DLLfunctionPointerNotAssignedException e )
  {

  }
  return bOtherDVFSisEnabled ;
}

DWORD WINAPI PowerProfFromWin6DynLinked::PowerReadFriendlyName(
  __in_opt   const GUID * SchemeGuid,
  __in_opt   const GUID * SubGroupOfPowerSettingsGuid,
  __in_opt   const GUID * PowerSettingGuid,
  //A pointer to a variable to receive the elements. If this parameter is NULL,
  //the function retrieves the size of the buffer required.
  UCHAR * & Buffer
  )
{
  DWORD dwRes , dwBufferSize ;
  dwRes = PowerReadFriendlyName(
    NULL ,
    SchemeGuid ,
    SubGroupOfPowerSettingsGuid ,
    PowerSettingGuid ,
    //http://msdn.microsoft.com/en-us/library/aa372740%28VS.85%29.aspx:
    //"If this parameter is NULL, the BufferSize parameter receives the
    //required buffer size."
    NULL ,
    & dwBufferSize
    ) ;
  if ( dwRes == //ERROR_SUCCESS
    ERROR_MORE_DATA )
  {
    Buffer = new UCHAR[dwBufferSize] ;
    dwRes = PowerReadFriendlyName(
      NULL ,
      SchemeGuid ,
      SubGroupOfPowerSettingsGuid ,
      PowerSettingGuid ,
      //http://msdn.microsoft.com/en-us/library/aa372740%28VS.85%29.aspx:
      //"A pointer to a buffer that receives the friendly name."
      Buffer ,
      & dwBufferSize
      ) ;
  }
  return dwRes ;
  }

//http://msdn.microsoft.com/en-us/library/aa372740(VS.85).aspx:
//"If the SchemeGuid parameter is not NULL but both the
//SubGroupOfPowerSettingsGuid and 
//PowerSettingGuid parameters
//are NULL, the
//friendly name of the power scheme
//will be returned. If the SchemeGuid and
//SubGroupOfPowerSettingsGuid parameters are not NULLand the PowerSettingGuid
//parameter is NULL, the friendly name of the subgroup will be returned. If the
//SchemeGuid, SubGroupOfPowerSettingsGuid, and PowerSettingGuid parameters are
//not NULL, the friendly name of the power setting will be returned."
DWORD WINAPI PowerProfFromWin6DynLinked::PowerReadFriendlyName(
  __in_opt   HKEY RootPowerKey,
  __in_opt   const GUID *SchemeGuid,
  __in_opt   const GUID *SubGroupOfPowerSettingsGuid,
  __in_opt   const GUID *PowerSettingGuid,
  __out_opt  PUCHAR Buffer,
  __inout    LPDWORD BufferSize
  )
{
  //If the function pointer is assigned.
  if( m_pfnpowerreadfriendlyname )
    return //Call the DLL function.
    ( * m_pfnpowerreadfriendlyname ) 
    (
      //http://msdn.microsoft.com/en-us/library/aa372740%28VS.85%29.aspx:
      //"This parameter is reserved for future use and must be set to NULL."
      RootPowerKey,
      SchemeGuid,
      SubGroupOfPowerSettingsGuid,
      PowerSettingGuid,
      Buffer,
      BufferSize
    ) ;
  else
    throw DLLfunctionPointerNotAssignedException() ;
}

//0: power scheme does not exist
//1: power scheme exists
//2: error (enumerating) power scheme(s)
BYTE PowerProfFromWin6DynLinked:: PowerSchemeToSetExists()
{
  ULONG ulIndex = 0 ;
  DWORD dwPowerEnumerateRet ;
  UCHAR * p_uchBuffer ;
  GUID GUIDscheme ;
  std::wstring stdwstr ;
//http://msdn.microsoft.com/en-us/library/aa372730(VS.85).aspx:
//"This function is normally called in a loop incrementing the Index parameter
//to retrieve subkeys until they've all been enumerated."
  do
  {
    dwPowerEnumerateRet = PowerEnumerate(
      & GUIDscheme //__in_opt   const GUID *SchemeGuid,
      //NULL
      , NULL //__in_opt   const GUID *SubGroupOfPowerSettingsGuid,
      //ACCESS_SCHEME: Enumerate power schemes. The
      //SchemeGuid and
      //SubgroupOfPowerSettingsGuid
      //parameters will be ignored."
      , ACCESS_SCHEME //__in POWER_DATA_ACCESSOR AccessFlags,
      , ulIndex //__in       ULONG ulIndex,
      //A pointer to a variable to receive the elements. If this parameter is NULL,
      //the function retrieves the size of the buffer required.
      , p_uchBuffer //UCHAR * & Buffer
      ) ;
    if( dwPowerEnumerateRet == ERROR_SUCCESS )
    {
      GetPowerSchemeName(GUIDscheme, stdwstr ) ;
      if( stdwstr == m_stdwstrPowerSchemeName )
      {
        m_guidPowerSchemeToSet = GUIDscheme ;
        return 1 ;
      }
      #ifdef _DEBUG
        #if !defined(__MINGW32__) //MinGW does not know "wcout"
          std::wcout << stdwstr << L'\n' ;
        #endif
      #endif
      ++ ulIndex ;
    }
  }while( dwPowerEnumerateRet == ERROR_SUCCESS ) ;
  #ifdef _DEBUG
  ulIndex = ulIndex ; //instr. is only for breakpoint
  #endif
  return 0 ;
}

DWORD WINAPI PowerProfFromWin6DynLinked::PowerSetActiveScheme(
    __in_opt  HKEY UserRootPowerKey,
    __in      const GUID *SchemeGuid
  )
{
  //http://msdn.microsoft.com/en-us/library/aa372758%28VS.85%29.aspx:
  //"Returns ERROR_SUCCESS (zero) if the call was successful, and a non-zero
  //value if the call failed."
  return (*m_pfnpowersetactivescheme) (
    UserRootPowerKey,
    SchemeGuid //__in      const GUID *SchemeGuid
    ) ;
}

DWORD WINAPI PowerProfFromWin6DynLinked::PowerSettingAccessCheck(
  __in      POWER_DATA_ACCESSOR AccessFlags,
  __in_opt  const GUID *PowerGuid
  )
{
  return (*m_pfnpowersettingaccesscheck) (
    AccessFlags,
    PowerGuid
    ) ;
}

DWORD WINAPI PowerProfFromWin6DynLinked::PowerWriteACValueIndex(
  __in_opt  HKEY RootPowerKey,
  __in      const GUID *SchemeGuid,
  __in_opt  const GUID *SubGroupOfPowerSettingsGuid,
  __in_opt  const GUID *PowerSettingGuid,
  __in      DWORD DcValueIndex
  )
{
  //DWORD dw ;
  
  if( m_pfnpowerwriteacvalueindex )
    return (*m_pfnpowerwriteacvalueindex) 
      (
      RootPowerKey,
      SchemeGuid,
      SubGroupOfPowerSettingsGuid,
      PowerSettingGuid ,
      DcValueIndex
      ) ;
  else
    throw //exception ;
      DLLfunctionPointerNotAssignedException() ;
}

DWORD WINAPI PowerProfFromWin6DynLinked::PowerWriteDCValueIndex(
  __in_opt  HKEY RootPowerKey,
  __in      const GUID *SchemeGuid,
  __in_opt  const GUID *SubGroupOfPowerSettingsGuid,
  __in_opt  const GUID *PowerSettingGuid,
  __in      DWORD DcValueIndex
)
{
  if( m_pfnpowerwritedcvalueindex )
    return (*m_pfnpowerwritedcvalueindex) 
      (
      RootPowerKey,
      SchemeGuid,
      SubGroupOfPowerSettingsGuid,
      PowerSettingGuid ,
      DcValueIndex
      ) ;
  else
    throw //exception ;
      DLLfunctionPointerNotAssignedException() ;
}

void PowerProfFromWin6DynLinked::GetThrottleSettings(
  const GUID & guidActivePowerScheme
  , DWORD & dwACProcThrottleMaxValue
  , DWORD & dwACProcThrottleMinValue 
  , DWORD & dwDCProcThrottleMaxValue 
  , DWORD & dwDCProcThrottleMinValue 
  )
{
  try
  {
//    GUID guidActivePowerScheme ;
//    if ( PowerGetActiveScheme(
//      //This parameter is reserved for future use and must be set to NULL.
//      NULL,
//      //&pActivePolicyGuid
//      guidActivePowerScheme ) == ERROR_SUCCESS
//      )
    {
      SYSTEM_POWER_STATUS system_power_status ;
      if( //ACPower() 
        //If the function succeeds, the return value is nonzero.
        ::GetSystemPowerStatus(& system_power_status)
        )
      {
        ////DWORD dwACProcThrottleMaxValue ;
        //if( system_power_status.ACLineStatus == 1 // Online -> on AC
        //  )
        //{
          //DWORD dwACProcThrottleMaxValue ;
          //DWORD dwACProcThrottleMinValue ;
          //Returns ERROR_SUCCESS (zero) if the call was successful, and a non-zero value if the call failed.
          //GUID *pPwrGUID;
          //PowerGetActiveScheme(NULL, &pPwrGUID);
          PowerReadACValueIndex (
            //__in_opt HKEY RootPowerKey: This parameter is reserved for 
            //future use and must be set to NULL.
            NULL,
            //__in_opt CONST GUID *SchemeGuid,
            //pActivePolicyGuid,
            //p_guidActivePowerScheme ,
            & guidActivePowerScheme ,
            //__in_opt CONST GUID *SubGroupOfPowerSettingsGuid,
            & GUID_PROCESSOR_SETTINGS_SUBGROUP ,
            //__in_opt CONST GUID *PowerSettingGuid,
            & GUID_PROCESSOR_THROTTLE_MAXIMUM ,
            //__out LPDWORD AcValueIndex
            & dwACProcThrottleMaxValue
            );
          PowerReadACValueIndex (
            //__in_opt HKEY RootPowerKey: This parameter is reserved for 
            //future use and must be set to NULL.
            NULL,
            //__in_opt CONST GUID *SchemeGuid,
            //pActivePolicyGuid,
            //p_guidActivePowerScheme ,
            & guidActivePowerScheme ,
            //__in_opt CONST GUID *SubGroupOfPowerSettingsGuid,
            & GUID_PROCESSOR_SETTINGS_SUBGROUP ,
            //__in_opt CONST GUID *PowerSettingGuid,
            & GUID_PROCESSOR_THROTTLE_MINIMUM ,
            //__out LPDWORD AcValueIndex
            & dwACProcThrottleMinValue
            );
          //if( dwACProcThrottleMaxValue == dwACProcThrottleMinValue )
          //  bOtherDVFSisEnabled = false ;
        //}
        //else if( system_power_status.ACLineStatus == //Offline -> on Direct Current (battery)
        //  0 )
        //{
          //DWORD dwACProcThrottleMaxValue ;
          //DWORD dwACProcThrottleMinValue ;
          PowerReadDCValueIndex (
            //__in_opt HKEY RootPowerKey: This parameter is reserved for future use and must be set to NULL.
            NULL,
            //__in_opt CONST GUID *SchemeGuid,
            //pActivePolicyGuid,
            //p_guidActivePowerScheme ,
            & guidActivePowerScheme ,
            //__in_opt CONST GUID *SubGroupOfPowerSettingsGuid,
            & GUID_PROCESSOR_SETTINGS_SUBGROUP ,
            //__in_opt CONST GUID *PowerSettingGuid,
            & GUID_PROCESSOR_THROTTLE_MAXIMUM ,
            //__out LPDWORD AcValueIndex
            & dwDCProcThrottleMaxValue
            );
          PowerReadDCValueIndex (
            //__in_opt HKEY RootPowerKey: This parameter is reserved for future use and must be set to NULL.
            NULL,
            //__in_opt CONST GUID *SchemeGuid,
            //pActivePolicyGuid,
            //p_guidActivePowerScheme ,
            & guidActivePowerScheme ,
            //__in_opt CONST GUID *SubGroupOfPowerSettingsGuid,
            & GUID_PROCESSOR_SETTINGS_SUBGROUP ,
            //__in_opt CONST GUID *PowerSettingGuid,
            & GUID_PROCESSOR_THROTTLE_MINIMUM ,
            //__out LPDWORD AcValueIndex
            & dwDCProcThrottleMinValue
            );
          //if( dwACProcThrottleMaxValue == dwACProcThrottleMinValue )
          //  bOtherDVFSisEnabled = false ;
        //}
      }
      //::LocalFree( p_guidActivePowerScheme);
    }
  }
  catch( DLLfunctionPointerNotAssignedException e )
  {

  }
  //return bOtherDVFSisEnabled ;
}

DWORD PowerProfFromWin6DynLinked::PowerWriteFriendlyName(
  __in_opt  HKEY RootPowerKey,
  __in      const GUID *SchemeGuid,
  __in_opt  const GUID *SubGroupOfPowerSettingsGuid,
  __in_opt  const GUID *PowerSettingGuid,
  __in      UCHAR *Buffer,
  __in      DWORD BufferSize
  )
{
  #ifdef _DEBUG
    #if !defined(__MINGW32__) //MinGW does not know "wcout"
      std::wcout << L"changing name to " << (wchar_t*)Buffer << BufferSize ;
    #endif
  #endif
  return (*m_pfnpowerwritefriendlyname) (
    RootPowerKey,
    SchemeGuid,
    SubGroupOfPowerSettingsGuid,
    PowerSettingGuid,
    Buffer,
    BufferSize
    ) ;
}

//0: power scheme does not exist
//1: power scheme exists
//2: error (enumerating) power scheme(s)
BYTE PowerProfFromWin6DynLinked:: SetActivePowerScheme(
  const std::wstring & r_stdwstrPowerSchemeName )
{
  ULONG ulIndex = 0 ;
  DWORD dwPowerEnumerateRet ;
  UCHAR * p_uchBuffer ;
  GUID GUIDscheme ;
  std::wstring stdwstr ;
//http://msdn.microsoft.com/en-us/library/aa372730(VS.85).aspx:
//"This function is normally called in a loop incrementing the Index parameter
//to retrieve subkeys until they've all been enumerated."
  do
  {
    dwPowerEnumerateRet = PowerEnumerate(
      & GUIDscheme //__in_opt   const GUID *SchemeGuid,
      //NULL
      , NULL //__in_opt   const GUID *SubGroupOfPowerSettingsGuid,
      //ACCESS_SCHEME: Enumerate power schemes. The
      //SchemeGuid and
      //SubgroupOfPowerSettingsGuid
      //parameters will be ignored."
      , ACCESS_SCHEME //__in POWER_DATA_ACCESSOR AccessFlags,
      , ulIndex //__in       ULONG ulIndex,
      //A pointer to a variable to receive the elements. If this parameter is NULL,
      //the function retrieves the size of the buffer required.
      , p_uchBuffer //UCHAR * & Buffer
      ) ;
    if( dwPowerEnumerateRet == ERROR_SUCCESS )
    {
      GetPowerSchemeName(GUIDscheme, stdwstr ) ;
      DEBUGN("enumerating schemes. current power scheme name:" <<
          GetStdString( stdwstr) )
      if( stdwstr == r_stdwstrPowerSchemeName )
      {
        DWORD dwRet = PowerSetActiveScheme( NULL, & GUIDscheme ) ;
        if( dwRet == ERROR_SUCCESS )
          return 1 ;
        else
          return 2 ;
      }
      #ifdef _DEBUG
        #if !defined(__MINGW32__) //MinGW does not know "wcout"
          std::wcout << stdwstr << L'\n' ;
        #endif
      #endif
      ++ ulIndex ;
    }
  }while( dwPowerEnumerateRet == ERROR_SUCCESS ) ;
  #ifdef _DEBUG
  ulIndex = ulIndex ; //instr. is only for breakpoint
  #endif
  return 0 ;
}

void PowerProfFromWin6DynLinked::SetFunctionPointersToNULL()
{
  m_pfnpowerdeletescheme = NULL ;
  m_pfnpowerduplicatescheme = NULL ;
  m_pfnpowergetactivescheme = NULL ;
  m_pfnpowerenumerate = NULL ;
  m_pfnpowerreadacvalueindex = NULL ;
  m_pfnpowerreaddcvalueindex = NULL ;
  m_pfnpowersetactivescheme = NULL ;
  m_pfnpowersettingaccesscheck = NULL ;
  m_pfnpowerwriteacvalueindex = NULL ;
  m_pfnpowerwritedcvalueindex = NULL ;
  m_pfnpowerwritefriendlyname = NULL ;
}
