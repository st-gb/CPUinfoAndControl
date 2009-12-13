#include <Windows.h> //for ::GetProcAddress(), ::GetSystemPowerStatus(), <PowrProf.h>
#include "global.h"
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
//#define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
//    EXTERN_C const GUID FAR name
//#endif // INITGUID
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


BYTE PowerProfFromWin6DynLinked::DeletePowerScheme( 
  const std::tstring & cr_stdtstrPowerSchemeName )
{
  return 0 ;
}

bool PowerProfFromWin6DynLinked::DisableFrequencyScalingByOS()
{
  bool bSuccess = false ;
  //bool bOtherDVFSisEnabled = true ;
  //GUID * p_guidActivePowerScheme ;
  GUID guidActivePowerScheme ;
  try
  {
    DWORD dwBufferSize , dwRes ;
    GUID guidPowerScheme ;
    GUID guidSubGroupOfPowerSettings ;
    std::string strPowerScheme, strWantedPowerScheme ; //= "CPUcontrol" ;
    UCHAR * Buffer ;
    POWER_DATA_ACCESSOR AccessFlags ;
    ULONG ulIndex = 0 ;
    strWantedPowerScheme = GetCharPointer( 
      m_stdwstrPowerSchemeName.c_str() ) ;
    //Walk through all power schemes.
    do
    {
      dwRes = PowerEnumerate(
        & guidPowerScheme ,
        & guidSubGroupOfPowerSettings ,//__in_opt   const GUID * ,
        AccessFlags ,//__in       POWER_DATA_ACCESSOR AccessFlags,
        ulIndex ,//__in       ULONG ulIndex,
        //A pointer to a variable to receive the elements. If this parameter
        //is NULL, the function retrieves the size of the buffer required.
        Buffer //__out_opt  UCHAR *Buffer,
        ) ;
      //Get the name of the power scheme with GUID "guidPowerScheme".
      if( GetPowerSchemeName(guidPowerScheme, strPowerScheme ) == ERROR_SUCCESS )
        if( strPowerScheme == strWantedPowerScheme )
          break ;
      ulIndex ++ ;
    }
    while( dwRes == ERROR_SUCCESS) ;
    GetThrottleSettings( 
      m_dwACProcThrottleMaxValue
      , m_dwACProcThrottleMinValue
      , m_dwDCProcThrottleMaxValue
      , m_dwDCProcThrottleMinValue 
      ) ;
    m_bThrottleSettingsAssigned = true ;
    if ( PowerGetActiveScheme(
      //This parameter is reserved for future use and must be set to NULL.
      NULL, 
      //&pActivePolicyGuid
      guidActivePowerScheme ) == ERROR_SUCCESS 
      )
    {
      m_bActivePowerSchemeAssigned = true ;
      m_guidPowerSchemeBeforeDisabling = guidActivePowerScheme ;
      DWORD dwProcThrottleValue = 25 ;
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
          if( PowerWriteACValueIndex(
            //RootPowerKey: This parameter is reserved for future use and must be set to NULL.
            NULL,
            //__in      const GUID *SchemeGuid,
            //pPwrGUID,
            //pActivePolicyGuid,
            & guidActivePowerScheme ,
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
            if( PowerWriteACValueIndex(
              //RootPowerKey: This parameter is reserved for future use and must be set to NULL.
              NULL,
              //__in      const GUID *SchemeGuid,
              //pPwrGUID,
              //pActivePolicyGuid,
              & guidActivePowerScheme ,
              //__in_opt  const GUID *SubGroupOfPowerSettingsGuid,
              & GUID_PROCESSOR_SETTINGS_SUBGROUP,
              //__in_opt  const GUID *PowerSettingGuid,
              //PROCTHROTTLEMAX,
              & GUID_PROCESSOR_THROTTLE_MINIMUM,
              //__in      DWORD AcValueIndex
              dwProcThrottleValue
              ) == ERROR_SUCCESS )
              bSuccess = true ;
          }
          //if( dwACProcThrottleMaxValue == dwACProcThrottleMinValue )
          //  bOtherDVFSisEnabled = false ;
        }
        else if( system_power_status.ACLineStatus == //Offline
          0 )
        {
          DWORD dwACProcThrottleMaxValue ;
          DWORD dwACProcThrottleMinValue ;
          if( PowerWriteDCValueIndex(
            //RootPowerKey: This parameter is reserved for future use and must be set to NULL.
            NULL,
            //__in      const GUID *SchemeGuid,
            //pPwrGUID,
            //pActivePolicyGuid,
            & guidActivePowerScheme ,
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
              & guidActivePowerScheme ,
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
          //if( dwACProcThrottleMaxValue == dwACProcThrottleMinValue )
          //  bOtherDVFSisEnabled = false ;
        }
      }
      //::LocalFree( p_guidActivePowerScheme);
    }
  }
  catch( DLLfunctionPointerNotAssignedException e )
  {

  }
  return //bOtherDVFSisEnabled ;
    bSuccess ;
}

unsigned char PowerProfFromWin6DynLinked::EnableFrequencyScalingByOS()
{
  BYTE bySuccess = 0 ;
  if( m_bThrottleSettingsAssigned  && m_bActivePowerSchemeAssigned )
  {
    bySuccess = 1 ;
    if( PowerWriteACValueIndex(
      //RootPowerKey: This parameter is reserved for future use and must be set to NULL.
      NULL,
      //__in      const GUID *SchemeGuid,
      //pPwrGUID,
      //pActivePolicyGuid,
      & m_guidPowerSchemeBeforeDisabling ,
      //__in_opt  const GUID *SubGroupOfPowerSettingsGuid,
      & GUID_PROCESSOR_SETTINGS_SUBGROUP,
      //__in_opt  const GUID *PowerSettingGuid,
      //PROCTHROTTLEMAX,
      & GUID_PROCESSOR_THROTTLE_MAXIMUM,
      //__in      DWORD AcValueIndex
      m_dwACProcThrottleMaxValue
      ) != ERROR_SUCCESS
      )
      bySuccess = 0 ;
              
    if( PowerWriteACValueIndex(
      //RootPowerKey: This parameter is reserved for future use and must be set to NULL.
      NULL,
      //__in      const GUID *SchemeGuid,
      //pPwrGUID,
      //pActivePolicyGuid,
      & m_guidPowerSchemeBeforeDisabling ,
      //__in_opt  const GUID *SubGroupOfPowerSettingsGuid,
      & GUID_PROCESSOR_SETTINGS_SUBGROUP,
      //__in_opt  const GUID *PowerSettingGuid,
      //PROCTHROTTLEMAX,
      & GUID_PROCESSOR_THROTTLE_MINIMUM,
      //__in      DWORD AcValueIndex
      m_dwACProcThrottleMinValue
      ) == ERROR_SUCCESS
      )
      bySuccess = 0 ;

    if( PowerWriteDCValueIndex(
      //RootPowerKey: This parameter is reserved for future use and must be set to NULL.
      NULL,
      //__in      const GUID *SchemeGuid,
      //pPwrGUID,
      //pActivePolicyGuid,
      & m_guidPowerSchemeBeforeDisabling ,
      //__in_opt  const GUID *SubGroupOfPowerSettingsGuid,
      & GUID_PROCESSOR_SETTINGS_SUBGROUP,
      //__in_opt  const GUID *PowerSettingGuid,
      //PROCTHROTTLEMAX,
      & GUID_PROCESSOR_THROTTLE_MAXIMUM,
      //__in      DWORD AcValueIndex
      m_dwDCProcThrottleMaxValue
      ) != ERROR_SUCCESS
      )
      bySuccess = 0 ;
              
    if( PowerWriteDCValueIndex(
      //RootPowerKey: This parameter is reserved for future use and must be set to NULL.
      NULL,
      //__in      const GUID *SchemeGuid,
      //pPwrGUID,
      //pActivePolicyGuid,
      & m_guidPowerSchemeBeforeDisabling ,
      //__in_opt  const GUID *SubGroupOfPowerSettingsGuid,
      & GUID_PROCESSOR_SETTINGS_SUBGROUP,
      //__in_opt  const GUID *PowerSettingGuid,
      //PROCTHROTTLEMAX,
      & GUID_PROCESSOR_THROTTLE_MINIMUM,
      //__in      DWORD AcValueIndex
      m_dwDCProcThrottleMinValue
      ) == ERROR_SUCCESS
      )
      bySuccess = 0 ;
  }
  return bySuccess ;
}

//Return: ERROR_SUCCESS if everything succeeded.
DWORD PowerProfFromWin6DynLinked::GetPowerSchemeName(
  GUID & guidPowerScheme , std::string & r_stdstr )
{
  DWORD dwBufferSize ;
  DWORD dwRes = 
    //double-call-approach: 
    //1st call: get the number of bytes needed for allocating
    //the buffer.
    PowerReadFriendlyName(
    NULL ,
    //http://msdn.microsoft.com/en-us/library/aa372740(VS.85).aspx:
    //If the SchemeGuid parameter is not NULL but both the 
    //SubGroupOfPowerSettingsGuid and PowerSettingGuid parameters are 
    //NULL, the friendly name of the power scheme will be returned.
    & guidPowerScheme ,
    NULL ,//__in_opt   const GUID *SubGroupOfPowerSettingsGuid,
    NULL ,//__in_opt   const GUID *PowerSettingGuid,
    NULL ,//__out_opt  PUCHAR Buffer,
    //If the Buffer parameter is NULL, the function returns ERROR_SUCCESS 
    //and the variable receives the required buffer size.
    & dwBufferSize //__inout    LPDWORD BufferSize
    ) ;
  if( dwRes == ERROR_SUCCESS 
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
      r_stdstr = std::string( (char *) ar_ucharBuffer ) ;
    //Release the allocated memory.
    delete [] ar_ucharBuffer ;
  }
  return dwRes ;
}

//http://msdn.microsoft.com/en-us/library/aa372730(VS.85).aspx
//This function is normally called in a loop incrementing the Index parameter to retrieve subkeys until they've all been enumerated.
//Returns ERROR_SUCCESS (zero) if the call was successful, and a non-zero value if the call failed. If the buffer size passed in the BufferSize parameter is too small, or if the Buffer parameter is NULL, ERROR_MORE_DATA will be returned and the DWORD pointed to by the BufferSize parameter will be filled in with the required buffer size.
DWORD PowerProfFromWin6DynLinked::PowerEnumerate(
  __in_opt   HKEY RootPowerKey,
  __in_opt   const GUID *SchemeGuid,
  __in_opt   const GUID *SubGroupOfPowerSettingsGuid,
  __in       POWER_DATA_ACCESSOR AccessFlags,
  __in       ULONG Index,
  //A pointer to a variable to receive the elements. If this parameter is NULL, the function retrieves the size of the buffer required.
  __out_opt  UCHAR *Buffer,
  //A pointer to a variable that on input contains the size of the buffer pointed to by the Buffer parameter. If the Buffer parameter is NULL or if the BufferSize is not large enough, the function will return ERROR_MORE_DATA and the variable receives the required buffer size.
  __inout    DWORD *BufferSize
  )
{
  DWORD dw ;
  if( m_pfnpowerenumerate )
  {
    dw = (*m_pfnpowerenumerate) (
      //This parameter is reserved for future use and must be set to NULL.
      NULL ,//RootPowerKey,
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

DWORD PowerProfFromWin6DynLinked::PowerEnumerate(
  __in_opt   const GUID *SchemeGuid,
  __in_opt   const GUID *SubGroupOfPowerSettingsGuid,
  __in       POWER_DATA_ACCESSOR AccessFlags,
  __in       ULONG ulIndex,
  //A pointer to a variable to receive the elements. If this parameter is NULL, the function retrieves the size of the buffer required.
  UCHAR * & Buffer
  )
{
  DWORD dwBufferSize ;
  DWORD dwRes = PowerEnumerate(
    NULL ,
    SchemeGuid,
    SubGroupOfPowerSettingsGuid,
    AccessFlags,
    ulIndex,
    //A pointer to a variable to receive the elements. If this parameter is NULL, the function retrieves the size of the buffer required.
    NULL ,//__out_opt  UCHAR *Buffer,
    //A pointer to a variable that on input contains the size of the buffer pointed to by the Buffer parameter. If the Buffer parameter is NULL or if the BufferSize is not large enough, the function will return ERROR_MORE_DATA and the variable receives the required buffer size.
    & dwBufferSize
    ) ;
  if ( dwRes == ERROR_SUCCESS )
  {
    Buffer = new UCHAR[dwBufferSize] ;
    dwRes = PowerEnumerate(
      NULL ,
      SchemeGuid,
      SubGroupOfPowerSettingsGuid,
      AccessFlags,
      ulIndex,
      //A pointer to a variable to receive the elements. If this parameter is NULL, the function retrieves the size of the buffer required.
      Buffer ,//__out_opt  UCHAR *Buffer,
      //A pointer to a variable that on input contains the size of the buffer pointed to by the Buffer parameter. If the Buffer parameter is NULL or if the BufferSize is not large enough, the function will return ERROR_MORE_DATA and the variable receives the required buffer size.
      & dwBufferSize
      ) ;
    delete [] Buffer ;
  }
  return dwRes ;
}

PowerProfFromWin6DynLinked::PowerProfFromWin6DynLinked(
  //std::wstring & r_stdwstrPowerSchemeName
  std::tstring & r_stdtstrPowerSchemeName
  )
  : mp_guidPowerSchemeBeforeDisabling ( NULL ) 
  , m_bActivePowerSchemeAssigned ( false )
  , m_bThrottleSettingsAssigned ( false )
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
  }
  else
    SetFunctionPointersToNULL();
}

void PowerProfFromWin6DynLinked::InitializeFunctionPointers()
{
  std::string strFuncName = "PowerGetActiveScheme" ;
  m_pfnpowergetactivescheme = (pfnPowerGetActiveScheme) ::GetProcAddress( 
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

  strFuncName = "PowerWriteACValueIndex" ;
  m_pfnpowerwriteacvalueindex = (pfnPowerWriteACValueIndex) ::GetProcAddress( 
    m_hinstancePowerProfDLL, strFuncName.c_str() );

  strFuncName = "PowerWriteDCValueIndex" ;
  m_pfnpowerwritedcvalueindex = (pfnPowerWriteACValueIndex) ::GetProcAddress( 
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

//http://msdn.microsoft.com/en-us/library/aa372740(VS.85).aspx:
//If the SchemeGuid parameter is not NULL but both the SubGroupOfPowerSettingsGuid and PowerSettingGuid parameters are NULL, the friendly name of the power scheme will be returned. If the SchemeGuid and SubGroupOfPowerSettingsGuid parameters are not NULLand the PowerSettingGuid parameter is NULL, the friendly name of the subgroup will be returned. If the SchemeGuid, SubGroupOfPowerSettingsGuid, and PowerSettingGuid parameters are not NULL, the friendly name of the power setting will be returned.
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
  DWORD & dwACProcThrottleMaxValue 
  , DWORD & dwACProcThrottleMinValue 
  , DWORD & dwDCProcThrottleMaxValue 
  , DWORD & dwDCProcThrottleMinValue 
  )
{
  try
  {
    GUID guidActivePowerScheme ;
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

void PowerProfFromWin6DynLinked::SetFunctionPointersToNULL()
{
  m_pfnpowergetactivescheme = NULL ;
  m_pfnpowerenumerate = NULL ;
  m_pfnpowerreadacvalueindex = NULL ;
  m_pfnpowerreaddcvalueindex = NULL ;
  m_pfnpowerwriteacvalueindex = NULL ;
  m_pfnpowerwritedcvalueindex = NULL ;
}
