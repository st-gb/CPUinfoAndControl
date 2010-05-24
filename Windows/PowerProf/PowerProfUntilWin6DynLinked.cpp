//If gobal.h is included after other include following error may occur:
//"C:/MinGW/bin/../lib/gcc/mingw32/3.4.5/../../../../include/c++/3.4.5/bits/
//locale_facets.h:1974: error: expected primary-expression before ',' token"
#include "global.h" //for LOGN(...)
#include "PowerProfUntilWin6DynLinked.hpp"
#include <tchar.h> //for "_T(...)"
#include <string> //std::string
#include <Windows/LocalLanguageMessageFromErrorCode.h>
//#include <Powrprof.h> //for static linking 

//struct PwrSchemesEnumProcStruct
//{
//  PwrSchemesEnumProcStruct() 
//    : bPowerSchemeFound ( false ) {}
//
//  bool bPowerSchemeFound ;
//  BYTE byDynamicThrottle ;
//  LPTSTR strPowerSchemeName ;
//  UINT uiPowerSchemeIndex ;
//};

//To continue until all power schemes have been enumerated, the 
//callback function must return TRUE. 
//To stop the enumeration, the callback function must return FALSE.
BOOLEAN CALLBACK PwrSchemesEnumProc (
  UINT uiPowerSchemeID ,      // power scheme index
  DWORD dwName,      // size of the sName string, in bytes
  //The sName and sDesc parameters are null-terminated strings; they are ANSI strings on Windows Me/98/95 and Unicode strings otherwise.
  //LPTSTR strPowerSchemeName,      // name of the power scheme
  LPWSTR strPowerSchemeName,      // name of the power scheme
  DWORD dwDesc,      // size of the sDesc string, in bytes
  //The sName and sDesc parameters are null-terminated strings; they are ANSI strings on Windows Me/98/95 and Unicode strings otherwise.
  //LPTSTR sDesc,      // description string
  LPWSTR sDesc,      // description string
  PPOWER_POLICY pp,  // receives the power policy
  LPARAM lParam      // user-defined value
  )
{
  PowerProfUntilWin6DynLinked * p_powerprofuntilwin6dynlinked = 
    (PowerProfUntilWin6DynLinked *) lParam ;
  if( p_powerprofuntilwin6dynlinked )
  {
    if( strPowerSchemeName == p_powerprofuntilwin6dynlinked->
      //m_strWantedPowerScheme 
      m_stdwstrWantedPowerScheme 
      )
    {
        p_powerprofuntilwin6dynlinked->m_uiPowerSchemeIndexWithP0ThrottleNone =
          uiPowerSchemeID ;
        ////stop the enumeration
        //return FALSE ;
    }
    //TODO powerscheme names seem to be wide chars. So only the first char is 
    //output because every 2nd char is '\0' which means a string end  .
    LOGN("powerscheme name: " << strPowerSchemeName << "(" << dwName << " chars)" )
    MACHINE_PROCESSOR_POWER_POLICY machine_processor_power_policy ;
    if( p_powerprofuntilwin6dynlinked->ReadProcessorPwrScheme(
        uiPowerSchemeID ,
        & machine_processor_power_policy
        ) 
      )
    {
      LOGN("machine_processor_power_policy.ProcessorPolicyAc: " <<
        //Covert to WORD, else output as (non-printable) CHARACTER.
        (WORD) machine_processor_power_policy.ProcessorPolicyAc.DynamicThrottle )
      if( machine_processor_power_policy.ProcessorPolicyAc.
        DynamicThrottle == PO_THROTTLE_NONE 
        )
      {
        p_powerprofuntilwin6dynlinked->m_bPowerSchemeWithP0ThrottleNoneFound = true ;
        p_powerprofuntilwin6dynlinked->m_uiPowerSchemeIndexWithP0ThrottleNone =
          uiPowerSchemeID ;
        ////stop the enumeration
        //return FALSE ;
      }
    }
  }
  return TRUE ;
}

//To continue until all power schemes have been enumerated, the 
//callback function must return TRUE. 
//To stop the enumeration, the callback function must return FALSE.
BOOLEAN CALLBACK PwrSchemesEnumProcSearchByDynamicThrottle (
  UINT uiPowerSchemeID ,      // power scheme index
  DWORD dwName,      // size of the sName string, in bytes
  //The sName and sDesc parameters are null-terminated strings; they are ANSI 
  //strings on Windows Me/98/95 and Unicode strings otherwise.
  //LPTSTR strPowerSchemeName,      // name of the power scheme
  LPWSTR strPowerSchemeName,      // name of the power scheme
  DWORD dwDesc,      // size of the sDesc string, in bytes
  //The sName and sDesc parameters are null-terminated strings; they are ANSI strings on Windows Me/98/95 and Unicode strings otherwise.
  //LPTSTR sDesc,      // description string
  LPWSTR  sDesc,      // description string
  PPOWER_POLICY pp,  // receives the power policy
  LPARAM lParam      // user-defined value
  )
{
  //PwrSchemesEnumProcStruct * p_pwrschemesenumprocstruct = 
  //  (PwrSchemesEnumProcStruct *) lParam ;
  //if( p_pwrschemesenumprocstruct )
  PowerProfUntilWin6DynLinked * p_powerprofuntilwin6dynlinked = 
    (PowerProfUntilWin6DynLinked *) lParam ;
  if( p_powerprofuntilwin6dynlinked )
  {
    //TODO powerscheme names seem to be wide chars. So only the first char is 
    //output because every 2nd char is '\0' which means a string end  .
    LOGN("powerscheme name: " << strPowerSchemeName << "(" << dwName << " chars)" )
    MACHINE_PROCESSOR_POWER_POLICY machine_processor_power_policy ;
    if( p_powerprofuntilwin6dynlinked->ReadProcessorPwrScheme(
        uiPowerSchemeID ,
        & machine_processor_power_policy
        ) 
      )
    {
      LOGN("machine_processor_power_policy.ProcessorPolicyAc: " <<
        //Covert to WORD, else output as (non-printable) CHARACTER.
        (WORD) machine_processor_power_policy.ProcessorPolicyAc.DynamicThrottle )

              SYSTEM_POWER_STATUS system_power_status ;
      if( //ACPower() 
        //If the function succeeds, the return value is nonzero.
        ::GetSystemPowerStatus(& system_power_status)
        )
      {
        //Initialize just to avoid (g++) compiler warning.
        BYTE byDynamicThrottle = 0 ;
        //DWORD dwACProcThrottleMaxValue ;
        if( system_power_status.ACLineStatus == 1 // Online
          )
        {
          byDynamicThrottle =
            machine_processor_power_policy.ProcessorPolicyAc.
            DynamicThrottle ;
        }
        else if( system_power_status.ACLineStatus == //Offline
          0 )
        {
          byDynamicThrottle =
            machine_processor_power_policy.ProcessorPolicyDc.
            DynamicThrottle ;
        }
        if( 
					( system_power_status.ACLineStatus == 1
          || system_power_status.ACLineStatus == 0 )
					&& byDynamicThrottle == 
					p_powerprofuntilwin6dynlinked->m_byDynamicThrottle )
        {
          //p_pwrschemesenumprocstruct->uiPowerSchemeIndex =
          //  uiPowerSchemeID ;
          //p_pwrschemesenumprocstruct->bPowerSchemeFound = true ;
          p_powerprofuntilwin6dynlinked->m_uiPowerSchemeIndex =
            uiPowerSchemeID ;
          p_powerprofuntilwin6dynlinked->m_bPowerSchemeFound = true ;
					return FALSE ;
        }
      }
    }
  }
  return TRUE ;
}

//To continue until all power schemes have been enumerated, the 
//callback function must return TRUE. 
//To stop the enumeration, the callback function must return FALSE.
BOOLEAN CALLBACK PwrSchemesEnumProcSearchPowerSchemeByName (
  UINT uiPowerSchemeID ,      // power scheme index
  DWORD dwName,      // size of the sName string, in bytes
  //The sName and sDesc parameters are null-terminated strings; they are ANSI strings on Windows Me/98/95 and Unicode strings otherwise.
  //LPTSTR strPowerSchemeName,      // name of the power scheme
  LPWSTR strPowerSchemeName,      // name of the power scheme
  DWORD dwDesc,      // size of the sDesc string, in bytes
  //The sName and sDesc parameters are null-terminated strings; they are ANSI strings on Windows Me/98/95 and Unicode strings otherwise.
  //LPTSTR sDesc,      // description string
  LPWSTR sDesc,      // description string
  PPOWER_POLICY pp,  // receives the power policy
  LPARAM lParam      // user-defined value
  )
{
  LOGWN( L"Power scheme index:" << uiPowerSchemeID << " name:" << strPowerSchemeName )
  PowerProfUntilWin6DynLinked * p_powerprofuntilwin6dynlinked = 
    (PowerProfUntilWin6DynLinked *) lParam ;
  if( p_powerprofuntilwin6dynlinked )
  {
    p_powerprofuntilwin6dynlinked->m_bPowerSchemeWithP0ThrottleNoneFound = false ;
    if( strPowerSchemeName == p_powerprofuntilwin6dynlinked->
      //m_strWantedPowerScheme 
      m_stdwstrWantedPowerScheme 
      //m_stdwstrPowerSchemeName
      )
    {
      //p_powerprofuntilwin6dynlinked->m_uiPowerSchemeIndexWithP0ThrottleNone =
      //p_powerprofuntilwin6dynlinked->m_uiPowerSchemeIndex = uiPowerSchemeID ;
      //p_powerprofuntilwin6dynlinked->m_uiPowerSchemeIndexOfFoundName = uiPowerSchemeID ;
      p_powerprofuntilwin6dynlinked->m_uiPowerSchemeIndexOfWantedName = uiPowerSchemeID ;
      p_powerprofuntilwin6dynlinked->m_bPowerSchemeFound = true ;
      //ok, we found the scheme. Returning "FALSE" stops further enumeration.
      return FALSE ;
    }
  }
  return TRUE ;
}

//To continue until all power schemes have been enumerated, the 
//callback function must return TRUE. 
//To stop the enumeration, the callback function must return FALSE.
BOOLEAN CALLBACK PwrSchemesEnumProcSearchPowerSchemeByID (
  UINT uiPowerSchemeID ,      // power scheme index
  DWORD dwName,      // size of the sName string, in bytes
  //The sName and sDesc parameters are null-terminated strings; they are ANSI strings on Windows Me/98/95 and Unicode strings otherwise.
  //LPTSTR strPowerSchemeName,      // name of the power scheme
  LPWSTR strPowerSchemeName,      // name of the power scheme
  DWORD dwDesc,      // size of the sDesc string, in bytes
  //The sName and sDesc parameters are null-terminated strings; they are ANSI strings on Windows Me/98/95 and Unicode strings otherwise.
  //LPTSTR sDesc,      // description string
  LPWSTR sDesc,      // description string
  PPOWER_POLICY pp,  // receives the power policy
  LPARAM lParam      // user-defined value
  )
{
  //LOGWN( L"Power scheme index:" << uiPowerSchemeID << " name:" << strPowerSchemeName )
  PowerProfUntilWin6DynLinked * p_powerprofuntilwin6dynlinked = 
    (PowerProfUntilWin6DynLinked *) lParam ;
  if( p_powerprofuntilwin6dynlinked )
  {
    if( uiPowerSchemeID == p_powerprofuntilwin6dynlinked->
      m_uiPowerSchemeIndexToSearch
      )
    {
      p_powerprofuntilwin6dynlinked->m_stdwstrPowerSchemeNameOfIDtoSearch = 
        std::wstring( strPowerSchemeName) ;
      p_powerprofuntilwin6dynlinked->m_bPowerSchemeFound = true ;
      //ok, we found the scheme. Returning "FALSE" stops further enumeration.
      return FALSE ;
    }
  }
  return TRUE ;
}

//To continue until all power schemes have been enumerated, the 
//callback function must return TRUE. 
//To stop the enumeration, the callback function must return FALSE.
BOOLEAN CALLBACK PwrSchemesEnumProcLogOutput (
  UINT uiPowerSchemeID ,      // power scheme index
  DWORD dwName,      // size of the sName string, in bytes
  //The sName and sDesc parameters are null-terminated strings; they are ANSI strings on Windows Me/98/95 and Unicode strings otherwise.
  //LPTSTR strPowerSchemeName,      // name of the power scheme
  LPWSTR strPowerSchemeName,      // name of the power scheme
  DWORD dwDesc,      // size of the sDesc string, in bytes
  //The sName and sDesc parameters are null-terminated strings; they are ANSI strings on Windows Me/98/95 and Unicode strings otherwise.
  //LPTSTR sDesc,      // description string
  LPWSTR sDesc,      // description string
  PPOWER_POLICY pp,  // receives the power policy
  LPARAM lParam      // user-defined value
  )
{
  LOGWN( L"Power scheme index:" << uiPowerSchemeID << L" name:" << 
    strPowerSchemeName )
  return TRUE ;
}

//To continue until all power schemes have been enumerated, the
//callback function must return TRUE.
//To stop the enumeration, the callback function must return FALSE.
BOOLEAN CALLBACK PwrSchemesEnumProcGetAllNames (
  UINT uiPowerSchemeID ,      // power scheme index
  DWORD dwName,      // size of the sName string, in bytes
  //The sName and sDesc parameters are null-terminated strings; they are ANSI strings on Windows Me/98/95 and Unicode strings otherwise.
  //LPTSTR strPowerSchemeName,      // name of the power scheme
  LPWSTR lpwstrPowerSchemeName,      // name of the power scheme
  DWORD dwDesc,      // size of the sDesc string, in bytes
  //The sName and sDesc parameters are null-terminated strings; they are ANSI strings on Windows Me/98/95 and Unicode strings otherwise.
  //LPTSTR sDesc,      // description string
  LPWSTR sDesc,      // description string
  PPOWER_POLICY pp,  // receives the power policy
  LPARAM lParam      // user-defined value
  )
{
  std::set<std::wstring> * p_stdset_stdwstr = (std::set<std::wstring> *)
      lParam ;
  if( p_stdset_stdwstr )
  {
    p_stdset_stdwstr->insert( std::wstring(lpwstrPowerSchemeName) ) ;
  }
  return TRUE ;
}

PowerProfUntilWin6DynLinked::PowerProfUntilWin6DynLinked(
  std::wstring & r_stdwstrPowerSchemeName
//  std::tstring & r_stdtstrPowerSchemeName
  )
  //: m_bGotPowerSchemeBeforeDisabling ( false)
{
  //mp_stdwstrPowerSchemeName = & stdwstrPowerSchemeName ;
//#ifdef  UNICODE                     // r_winnt
//  m_stdwstrPowerSchemeName = r_stdtstrPowerSchemeName ;
//#else
//  m_stdwstrPowerSchemeName = //r_stdwstrPowerSchemeName ;
//    //http://www.wer-weiss-was.de/theme158/article3047390.html:
//    std::wstring ( r_stdtstrPowerSchemeName.begin(),
//      r_stdtstrPowerSchemeName.end() );
//#endif
  m_stdwstrPowerSchemeName = r_stdwstrPowerSchemeName ;
  Initialize() ;
}

//0: power scheme does not exist
//1: power scheme exists
//2: error (enumerating) power scheme(s)
BYTE PowerProfUntilWin6DynLinked:: PowerSchemeToSetExists()
{
  //May be set to true inside "PwrSchemesEnumProcSearchPowerSchemeByName"
  m_bPowerSchemeFound = false ;
  //Set the string to search
  m_stdwstrWantedPowerScheme = m_stdwstrPowerSchemeName ;
  //If the function succeeds, the return value is nonzero.
  //I experienced that it fails if either not power prfs for the user 
  //the process runs for or that maybe the user has no rights to enumerate the schemes.
  if( EnumPwrSchemes(
      //PwrSchemesEnumProc,
      PwrSchemesEnumProcSearchPowerSchemeByName ,
      (LPARAM) this
      )
    )
  {
    if( //m_bPowerSchemeWithP0ThrottleNoneFound 
      m_bPowerSchemeFound 
      )
    {
      //LOGN("Power scheme \"" << m_strWantedPowerScheme << "\" exists." )
      //LOGN("Power scheme \"" << m_strWantedPowerScheme.c_str() << "\" exists." )
      //LOGWN(L"Power scheme \"" << m_stdwstrWantedPowerScheme << L"\" exists." )
      //wcstombs
      std::string str( m_stdwstrPowerSchemeName.begin(), m_stdwstrPowerSchemeName.end() );
      LOGN("Power scheme \"" << str.c_str() << "\" exists" )
      LOGWN(L"Power scheme \"" << m_stdwstrPowerSchemeName.c_str() << L"\" exists" )
      //uiPowerSchemeIndex = m_uiPowerSchemeIndex ;
      //bDesiredPowerSchemeExists = true ;
    }
    else
    {
      //LOGN("Power scheme " << m_strWantedPowerScheme << " does not exist." )
      //LOGN("Power scheme " << m_strWantedPowerScheme.c_str() << " does not exist." )
      //LOGWN(L"Power scheme " << m_stdwstrWantedPowerScheme << L" does not exist." )
      std::string str( m_stdwstrPowerSchemeName.begin(), m_stdwstrPowerSchemeName.end() );
      LOGWN(L"Power scheme \"" << m_stdwstrPowerSchemeName.c_str() << L"\" does NOT exist" )
      LOGN("Power scheme \"" << str.c_str() << "\" does NOT exist" )
			//bCreatePowerScheme = true ;
    }
  }
	else //no power scheme for the user THIS process runs for or no
    //access rights.
	//	bCreatePowerScheme = true ;
    return 2 ;
  return m_bPowerSchemeFound ;
}

BYTE PowerProfUntilWin6DynLinked::ActivatePowerSchemeToSet()
{
  //return SetActivePwrScheme( //m_stdwstrPowerSchemeName.c_str() 
  //  m_stdwstrPowerSchemeName ) ;
  BOOLEAN booleanRet = SetActivePwrScheme( //m_uiPowerSchemeIndexWithP0ThrottleNone 
    //m_uiPowerSchemeIndexOfFoundName
    m_uiPowerSchemeIndexOfWantedName
  //http://msdn.microsoft.com/en-us/library/aa373200(VS.85).aspx:
  //If lpGlobalPowerPolicy is NULL, the function uses the current 
  //global power policy settings set by WriteGlobalPwrPolicy. 
  //Otherwise, the settings in the specified structure replace the 
  //current global power policy settings.
  , NULL 
  , NULL ) ;
  LOGN("Setting power scheme " << m_uiPowerSchemeIndexOfWantedName 
    << " as active scheme " << ( booleanRet ? "succeeded" : "failed" ) )
  return booleanRet ;
}

BOOLEAN WINAPI PowerProfUntilWin6DynLinked::CanUserWritePwrScheme(void)
{
  if( m_pfncanuserwritepwrscheme )
    return (*m_pfncanuserwritepwrscheme)() ;
  return FALSE ;
}

bool PowerProfUntilWin6DynLinked::ChangeOtherDVFSaccessPossible ()
{
  return CanUserWritePwrScheme() ;
}

BYTE PowerProfUntilWin6DynLinked::CreatePowerScheme(
  //LPTSTR lpszName ,
  //LPTSTR lpszDescription ,
  //e.g. WinXP uses wide chars for service name (and for descr.?)
  LPWSTR lpszName ,
  LPWSTR lpszDescription ,
  //std::wstring & r_stdwstrName ,
  //std::wstring & r_stdwstrDescription ,
  UINT & uiCurrentPowerSchemeID
  )
{
  BOOLEAN booleanRet ;
  BYTE byRet = 0 ;
  POWER_POLICY power_policy ;
  LOGWN( L"trying to create power scheme \"" << lpszName << "\" " )
  std::wstring stdwstrName(lpszName );
  std::wstring stdwstrDescription (lpszDescription) ;
  //UINT uiCurrentPowerSchemeID = 0 ;
  //Begin with 0
  uiCurrentPowerSchemeID = 0 ;
  //http://msdn.microsoft.com/en-us/library/aa372711(VS.85).aspx:
  //To create a power scheme, call the WritePwrScheme function, 
  //specifying an index number that is not currently in use. 
  //Note that creating a power scheme does not automatically update the 
  //active power scheme. You must always call SetActivePwrScheme to 
  //update the active power scheme.
  do
  {
    LOGN( "power scheme " << uiCurrentPowerSchemeID )
    booleanRet = ReadPwrScheme(
        uiCurrentPowerSchemeID ,
        & power_policy
      ) ;
    //ms-help://MS.VSCC.v80/MS.MSDN.v80/MS.WIN32COM.v10.en/power/base/readpwrscheme.htm:
    //If the power scheme specified does not exist, the function returns FALSE.
    if( ! booleanRet )
      break ;
    //BOOLEAN 
    //If the function succeeds, the return value is nonzero.
    ++ uiCurrentPowerSchemeID ;
  }
  while( booleanRet ) ;
  //if the power scheme specified does not exist or error
  //if( ! booleanRet )
  //{
  //  //The function failed
  //GetLastError() returned "5" ("access denied") when ReadPwrScheme() ret. "false"
  // but the power scheme just did not exist, so it's not an appropriate error code.
  // resp. "access denied" does not refer to a callto ReadPwrScheme() but to a call
  //  from before.
  //  DWORD dwLastError = ::GetLastError() ;
  //  LOGN( "ReadPwrScheme error code: " << dwLastError << 
  //    " error message: " << 
  //    LocalLanguageMessageFromErrorCode(dwLastError) 
  //    )
  //}
  GLOBAL_POWER_POLICY globalpowerpolicy ;
  POWER_POLICY powerpolicy ;
  if( GetCurrentPowerPolicies(
    & globalpowerpolicy  ,//PGLOBAL_POWER_POLICY pGlobalPowerPolicy,
    & powerpolicy //PPOWER_POLICY pPowerPolicy
    )
    )
  {
    LOGWN( L"Trying to create power scheme: index: " << uiCurrentPowerSchemeID 
      << L", name: \"" << stdwstrName.c_str() << "\""
      << L",descripton: \"" << stdwstrDescription.c_str() << "\"" )
    if( WritePwrScheme(
        & uiCurrentPowerSchemeID ,
        //lpszName,
        //lpszDescription,
        //e.g. WinXP uses wide chars for service name (and for descr.?)
        lpszName ,
        lpszDescription ,
        //NULL //PPOWER_POLICY pPowerPolicy
        & powerpolicy
        )
      )
    {
		  LOGWN( L"writing power scheme " << lpszName << L"succeeded" )
		  //LOGN("writing power scheme " << stdwstrName.c_str()  << "succeeded")
      byRet = 1 ;
    }
    else
    {
      //Wrong g++ compiler warning "unused variable" here:
      DWORD dwLastError = ::GetLastError() ;
      LOGWN( L"writing power scheme " << stdwstrName.c_str()  << 
        L"failed. Error code: " << dwLastError //)
        << L" error message: " << 
        ::GetStdWstring( ::LocalLanguageMessageFromErrorCode(dwLastError) )
        )
    }
  }
  else
  {
    DWORD dwLastError = ::GetLastError() ;
    LOGN("getting current power policies failed. Error code: " 
      << dwLastError << " error message: " << 
      LocalLanguageMessageFromErrorCode(dwLastError) 
      )
  }
  return byRet ;
}

unsigned char PowerProfUntilWin6DynLinked::CreatePowerSchemeWithWantedName() 
{
  //std::wstring::pointer pStr2 = m_stdwstrWantedPowerScheme;
  //const wchar_t * text_ptr = m_stdwstrWantedPowerScheme.c_str();
  //std::vector<wchar_t> buffer(text_ptr, text_ptr + text.size() + 1);
  //wchar_t* buffer_ptr = &buffer[0];
  return CreatePowerScheme(
    //LPTSTR lpszName ,
    //LPTSTR lpszDescription ,
    //e.g. WinXP uses wide chars for service name (and for descr.?)
    (LPWSTR) m_stdwstrWantedPowerScheme.c_str() ,
    //buffer_ptr
    //LPWSTR lpszDescription ,
    L"" ,
    //std::wstring & r_stdwstrName ,
    //std::wstring & r_stdwstrDescription ,
    //setzt voraus, dass vorher die ID von m_stdwstrWantedPowerScheme
    //erfahren wurde
    m_uiPowerSchemeIndexOfWantedName
    );
}

void PowerProfUntilWin6DynLinked::DeactivateCPUscaling(
  PROCESSOR_POWER_POLICY & r_processor_power_policy )
{
#ifdef _DEBUG
  LOGN("DynamicThrottle: " << (WORD) r_processor_power_policy.DynamicThrottle)
#endif
  //LOGN("machine_processor_power_policy.ProcessorPolicyAc: " <<
  //  //Covert to WORD, else output as (non-printable) CHARACTER.
  //  (WORD) machine_processor_power_policy.ProcessorPolicyAc.DynamicThrottle )
  if( //machine_processor_power_policy.ProcessorPolicyAc.
    r_processor_power_policy.
    DynamicThrottle != PO_THROTTLE_NONE 
    )
  {
    //machine_processor_power_policy.ProcessorPolicyAc.DynamicThrottle 
    r_processor_power_policy.DynamicThrottle 
      = PO_THROTTLE_NONE ;
  }
  //if( machine_processor_power_policy.ProcessorPolicyDc.
  //  DynamicThrottle != PO_THROTTLE_NONE 
  //  )
  //{
  //  machine_processor_power_policy.ProcessorPolicyDc.DynamicThrottle 
  //    = PO_THROTTLE_NONE ;
  //}
  //LOGN("writing setting for index " << uiPowerSchemeIndex <<
  //  "AC:" << 
  //    machine_processor_power_policy.ProcessorPolicyAc.DynamicThrottle 
  //  << "DC:" << 
  //  machine_processor_power_policy.ProcessorPolicyDc.DynamicThrottle
  //  )
  for( BYTE byPolicyIndex = 0 ; byPolicyIndex < 
    r_processor_power_policy.PolicyCount ; ++ byPolicyIndex )
  {
#ifdef _DEBUG
    LOGN("Policy " << (WORD) byPolicyIndex << " AllowDemotion: "
      << r_processor_power_policy.Policy[byPolicyIndex].AllowDemotion )
    LOGN("Policy " << (WORD) byPolicyIndex << " AllowPromotion: "
      << r_processor_power_policy.Policy[byPolicyIndex].AllowPromotion )
#endif
    //ms-help://MS.VSCC.v80/MS.MSDN.v80/MS.WIN32COM.v10.en/power/base/processor_power_policy_info_str.htm:
    //When set, allows the kernel power policy manager to demote from the current state. 
    r_processor_power_policy.Policy[byPolicyIndex].AllowDemotion = 0 ;
    //ms-help://MS.VSCC.v80/MS.MSDN.v80/MS.WIN32COM.v10.en/power/base/processor_power_policy_info_str.htm:
    //When set, allows the kernel power policy manager to promote from the current state.
    r_processor_power_policy.Policy[byPolicyIndex].AllowPromotion = 0 ;
  }
  //machine_processor_power_policy.ProcessorPolicyDc.Policy[3];
}

BOOLEAN PowerProfUntilWin6DynLinked::DeletePwrScheme(
  UINT uiIndex
  )
{
  if( m_pfndeletepwrscheme )
    return 
    //ms-help://MS.VSCC.v80/MS.MSDN.v80/MS.WIN32COM.v10.en/power/base/deletepwrscheme.htm:
    //"If the function succeeds, the return value is nonzero."
    (*m_pfndeletepwrscheme)( uiIndex ) ;
  else
    //throw 
    return FALSE ;
}

BYTE PowerProfUntilWin6DynLinked::DeletePowerScheme( 
    const std::tstring & cr_stdtstrPowerSchemeName )
{
  UINT uiPowerSchemeIndex ;
  LOGN( "should delete power scheme \"" << 
    GetStdString( cr_stdtstrPowerSchemeName ) << "\"" ) ;
  //1st: get ID for power scheme named "cr_stdtstrPowerSchemeName".
  if( GetPowerSchemeIndex( 
    GetStdWstring( cr_stdtstrPowerSchemeName )
    , uiPowerSchemeIndex )
    )
  {
    BOOL bool_ = DeletePwrScheme( uiPowerSchemeIndex ) ;
    LOGN( "Deleting power scheme \"" << 
      //Getstdtstring( cr_stdtstrPowerSchemeName ) << "\"" << 
      GetStdString( cr_stdtstrPowerSchemeName ) << "\"" << 
      ( bool_ ? "succeeded" : "failed" )
      ) ;
    return  bool_ ;
  }
  return 0 ;
}

BYTE PowerProfUntilWin6DynLinked::DisableDVFSforPowerSchemeToSet()
{
  return DisableCPUscaling(m_uiPowerSchemeIndexOfWantedName ) ;
}

BYTE PowerProfUntilWin6DynLinked::DisableCPUscaling(UINT uiPowerSchemeIndex)
{
  BYTE byRet = 0 ;
  MACHINE_PROCESSOR_POWER_POLICY machine_processor_power_policy ;
  if( ReadProcessorPwrScheme(
      //m_uiPowerSchemeIndexWithP0ThrottleNone ,
      uiPowerSchemeIndex ,
      & machine_processor_power_policy
      ) 
    )
  {
    PROCESSOR_POWER_POLICY * p_processor_power_policy = 
      & machine_processor_power_policy.ProcessorPolicyAc ;
    DeactivateCPUscaling(*p_processor_power_policy) ;
    p_processor_power_policy =
      & machine_processor_power_policy.ProcessorPolicyDc ;
    DeactivateCPUscaling(*p_processor_power_policy) ;
    LOGN("writing setting for index " << uiPowerSchemeIndex )
    //BOOLEAN 
    if( WriteProcessorPwrScheme(
        uiPowerSchemeIndex ,
        & machine_processor_power_policy
        )
      )
    {
      LOGN("Writing CPU settings succeeded" )
      byRet = 1 ;
    }
    else
      LOGN("Writing CPU settings failed:" << ::GetLastError() )
  }
  else
    LOGN("error getting processor info" << uiPowerSchemeIndex << " as active scheme.")
  return byRet ;
}

bool PowerProfUntilWin6DynLinked::DisableFrequencyScalingByOS()
{
	bool bCreatePowerScheme = false ;
  //UINT uiPowerSchemeID ;
  m_bGotPowerSchemeBeforeDisabling = GetActivePwrScheme( 
    & m_uiPowerSchemeIDBeforeDisabling ) ;
  //LOGN("Disabling OS's Dynamic Frequency scaling")
  LOGN("Disabling OS's Dynamic Frequency scaling")
  //if( GetActivePwrScheme( & uiPowerSchemeID)  )
  //{
  //  MACHINE_PROCESSOR_POWER_POLICY machine_processor_power_policy ;
  //  //If the function succeeds, the return value is nonzero.
  //  if( ReadProcessorPwrScheme(
  //      uiPowerSchemeID ,
  //      & machine_processor_power_policy
  //      ) 
  //    )
  //  {
  //    machine_processor_power_policy.ProcessorPolicyAc.
  //      DynamicThrottle = PO_THROTTLE_NONE ;
  //    if( WriteProcessorPwrScheme(
  //        uiPowerSchemeID,
  //        & machine_processor_power_policy
  //        )
  //      )
  //      return true ;
  //  }
  //}
  m_bPowerSchemeWithP0ThrottleNoneFound = false ;
  //m_strWantedPowerScheme = "CPUcontrol" ;
  //m_stdwstrWantedPowerScheme = L"CPUcontrol" ;
  bool bDesiredPowerSchemeExists = false ;
  UINT uiPowerSchemeIndex ;
  //If the function succeeds, the return value is nonzero.
  //I experienced that it fails if either not power prfs for the user 
  //the process runs for or that maybe the user has no rights to enumerate the schemes.
  if( EnumPwrSchemes(
      //PwrSchemesEnumProc,
      PwrSchemesEnumProcSearchPowerSchemeByName ,
      (LPARAM) this
      )
    )
  {
    if( //m_bPowerSchemeWithP0ThrottleNoneFound 
      m_bPowerSchemeFound 
      )
    {
      //LOGN("Power scheme \"" << m_strWantedPowerScheme << "\" exists." )
      LOGN("Power scheme \"" << m_strWantedPowerScheme.c_str() << "\" exists." )
      //LOGWN(L"Power scheme \"" << m_stdwstrWantedPowerScheme << L"\" exists." )
      uiPowerSchemeIndex = m_uiPowerSchemeIndex ;
      bDesiredPowerSchemeExists = true ;
    }
    else
    {
      //LOGN("Power scheme " << m_strWantedPowerScheme << " does not exist." )
      LOGN("Power scheme " << m_strWantedPowerScheme.c_str() << " does not exist." )
      //LOGWN(L"Power scheme " << m_stdwstrWantedPowerScheme << L" does not exist." )
			bCreatePowerScheme = true ;
    }
  }
	else //no power scheme for the user THIS process runs for or no
    //access rights.
		bCreatePowerScheme = true ;
	if( bCreatePowerScheme )
  {
    //std::wstring wstr = m_strWantedPowerScheme ;
    //LPTSTR lptstr = _T("CPUcontrol") ;
    //LPWSTR lpwstr = L"CPUcontrol" ;
    LPWSTR lpwstrDescription = L"for an own Dynamic Voltage & Frequency Scaling" ;
    if( CreatePowerScheme( //m_strWantedPowerScheme.c_str() , 
        //wstr.c_str() ,
        //lptstr ,
        //lpwstr ,
        (wchar_t *) m_stdwstrPowerSchemeName.c_str() ,
        //_T("for own Dyn.Volt. & Freq. Scal.") ,
        //L"for own Dyn.Volt. & Freq. Scal." ,
        lpwstrDescription ,
        uiPowerSchemeIndex
        ) 
      )
    {
      LOGN("creating power scheme succeeded")
      bDesiredPowerSchemeExists = true ;
    }
    else
    {
      LOGN("creating power scheme failed")
    }
  }
  if( bDesiredPowerSchemeExists )
  {
    LOGN("Wanted scheme exists")
    DisableCPUscaling(uiPowerSchemeIndex) ;
    LOGN("Setting power scheme index " << uiPowerSchemeIndex << " as active scheme.")
    SetActivePwrScheme( //m_uiPowerSchemeIndexWithP0ThrottleNone 
      uiPowerSchemeIndex
      //http://msdn.microsoft.com/en-us/library/aa373200(VS.85).aspx:
      //If lpGlobalPowerPolicy is NULL, the function uses the current global power policy settings set by WriteGlobalPwrPolicy. Otherwise, the settings in the specified structure replace the current global power policy settings.
      , NULL 
      , NULL 
      ) ;
  }
  else
  {
    LOGN("Wanted scheme does NOT exist")
  }
  return false ; 
}

unsigned char PowerProfUntilWin6DynLinked::EnableFrequencyScalingByOS()
{
  if( m_bGotPowerSchemeBeforeDisabling )
  {
    if( SetActivePwrScheme( m_uiPowerSchemeIDBeforeDisabling
      //http://msdn.microsoft.com/en-us/library/aa373200(VS.85).aspx:
      //If lpGlobalPowerPolicy is NULL, the function uses the current global power policy settings set by WriteGlobalPwrPolicy. Otherwise, the settings in the specified structure replace the current global power policy settings.
      , NULL 
      , NULL ) 
      )
    {
      LOGN("successfully enabled Windows' DVFS")
      return 1 ;
    }
  }
  else
  {
    //PwrSchemesEnumProcStruct pwrschemesenumprocstruct ;
    m_bPowerSchemeFound = false ;
		m_byDynamicThrottle = PO_THROTTLE_ADAPTIVE ;
    if( EnumPwrSchemes( 
      PwrSchemesEnumProcSearchByDynamicThrottle, 
      //PwrSchemesEnumProc ,
      //pwrschemesenumprocstruct 
      (LPARAM) this
      ) 
      //&& pwrschemesenumprocstruct.bPowerSchemeFound
      && m_bPowerSchemeFound
      )
    {
      if( SetActivePwrScheme( //pwrschemesenumprocstruct.uiPowerSchemeIndex
        m_uiPowerSchemeIndex
        //http://msdn.microsoft.com/en-us/library/aa373200(VS.85).aspx:
        //If lpGlobalPowerPolicy is NULL, the function uses the current global power policy settings set by WriteGlobalPwrPolicy. Otherwise, the settings in the specified structure replace the current global power policy settings.
        , NULL 
        , NULL ) 
        )
      {
        LOGN("successfully enabled Windows' DVFS")
        return 1 ;
      }      
    }
  }
  return 0 ;
} ;


bool PowerProfUntilWin6DynLinked::EnablingIsPossible()
{
  bool bEnablingIsPossible = false ;
  if( mp_uiPowerSchemeIDBeforeDisabling && CanUserWritePwrScheme() )
  {
    std::wstring stdwstrPowerSchemeName ;
    GetPowerSchemeName( * mp_uiPowerSchemeIDBeforeDisabling 
      , stdwstrPowerSchemeName ) ;
    if( stdwstrPowerSchemeName != m_stdwstrPowerSchemeName )
      bEnablingIsPossible = true ;
  }
  return bEnablingIsPossible ;
}

//Returns the return value from the call to the pwrprof.DLL's "EnumPwrSchemes"
//function.
//"If the function fails, the return value is zero."
//I experienced that it fails if either not power prfs for the user 
//the process runs for or that maybe the user has no rights to enumerate the schemes.
BOOLEAN PowerProfUntilWin6DynLinked::EnumPwrSchemes(
  //PWRSCHEMESENUMPROC 
  PWRSCHEMESENUMPROC_SG lpfnPwrSchemesEnumProc,
  LPARAM lParam
  )
{
  if( m_pfnenumpwrschemes )
  {
    BOOLEAN booleanRes = 
			//Call EnumPwrSchemes in pwrprof.dll 
			(*m_pfnenumpwrschemes) (lpfnPwrSchemesEnumProc, lParam) ;		
		//ms-help://MS.VSCC.v80/MS.MSDN.v80/MS.WIN32COM.v10.en/power/base/enumpwrschemes.htm:
    // / http://msdn.microsoft.com/en-us/library/aa372687(VS.85).aspx:
		//"If the function fails, the return value is zero. To get extended 
    //error information, call GetLastError."
		//I experienced that it fails if either not power prfs for the user 
		//the process runs for or that maybe the user has no rights to enumerate the schemes.
		if( ! booleanRes )
    {
      DWORD dw = ::GetLastError() ;
//#ifdef _DEBUG
//      booleanRes = ::EnumPwrSchemes(PwrSchemesEnumProc, 0) ;
//      dw = ::GetLastError() ;
//#endif //#ifdef _DEBUG
      //std::string str = LocalLanguageMessageFromErrorCode(dw) ;
      //str += " " ;
      //PPOWER_POLICY pp ;
      //PwrSchemesEnumProc( 
      //  0,      // power scheme index
      //    0,      // size of the sName string, in bytes
      //  //The sName and sDesc parameters are null-terminated strings; they are ANSI strings on Windows Me/98/95 and Unicode strings otherwise.
      //  "",      // name of the power scheme
      //  0,      // size of the sDesc string, in bytes
      //  //The sName and sDesc parameters are null-terminated strings; they are ANSI strings on Windows Me/98/95 and Unicode strings otherwise.
      //  "",      // description string
      //  pp,  // receives the power policy
      //  lParam      // user-defined value
      //  );
      std::string str = LocalLanguageMessageFromErrorCode(dw) ;
      LOGN( "enumerating power schemes failed: " << str ) ;
    }
    //If the function succeeds, the return value is nonzero.
    return booleanRes ;
  }
  return 0 ;
}

BOOLEAN //WINAPI
  PowerProfUntilWin6DynLinked::GetActivePwrScheme(
  //__out
    PUINT puiID )
{
  if( m_pfngetactivepwrscheme )
  {
    //If the function succeeds, the return value is nonzero.
    return (*m_pfngetactivepwrscheme) (puiID) ;
  }
  return FALSE ;
}

void PowerProfUntilWin6DynLinked::GetActivePowerSchemeName(
  std::wstring & r_stdwstrActivePowerSchemeName )
{
  BOOL boolRet ;
  UINT uiPowerSchemeID ;
  boolRet = GetActivePwrScheme( & uiPowerSchemeID ) ;
  if( boolRet )
  {
    boolRet = GetPowerSchemeName( uiPowerSchemeID ,
      r_stdwstrActivePowerSchemeName ) ;
    if( boolRet )
      r_stdwstrActivePowerSchemeName =
        L"Getting power scheme name for active scheme GUID failed" ;
  }
  else
  {
    r_stdwstrActivePowerSchemeName = L"Getting active scheme's GUID failed" ;
  }
}

BOOLEAN PowerProfUntilWin6DynLinked::GetCurrentPowerPolicies(
  PGLOBAL_POWER_POLICY pGlobalPowerPolicy,
  PPOWER_POLICY pPowerPolicy
  )
{
  if( m_pfngetcurrentpowerpolicies )
    return ( * m_pfngetcurrentpowerpolicies ) (
      pGlobalPowerPolicy,
      pPowerPolicy
      ) ;
  return FALSE ;
}

IDynFreqScalingAccess::string_type PowerProfUntilWin6DynLinked::
  GetEnableDescription()
{
  //std::tstring stdtstrReturn(_T("no other power scheme available") ) ;
  std::wstring stdwstrReturn( L"no other power scheme available" ) ;
  if( mp_uiPowerSchemeIDBeforeDisabling )
  {
    std::wstring stdwstrPowerSchemeName ;
    if( GetPowerSchemeName( * mp_uiPowerSchemeIDBeforeDisabling 
      , stdwstrPowerSchemeName ) 
      )
//      stdtstrReturn = _T("set power scheme \"")
//        + Getstdtstring( stdwstrPowerSchemeName )
//        + _T("\"") ;
      stdwstrReturn = L"set power scheme \""
            + stdwstrPowerSchemeName
            + L"\"" ;
  }
  return //stdtstrReturn ;
    stdwstrReturn ;
}

bool PowerProfUntilWin6DynLinked::GetPowerSchemeName( 
  UINT ui , std::wstring & r_stdwstrPowerSchemeName )
{
  m_bPowerSchemeFound = false ;
  m_uiPowerSchemeIndexToSearch = ui ;
  EnumPwrSchemes( 
    PwrSchemesEnumProcSearchPowerSchemeByID
    , (LPARAM) this ) ;
  if( m_bPowerSchemeFound )
  {
    r_stdwstrPowerSchemeName = m_stdwstrPowerSchemeNameOfIDtoSearch ;
  }
  return m_bPowerSchemeFound ;
}

void PowerProfUntilWin6DynLinked::Initialize()
{
  mp_uiPowerSchemeIDBeforeDisabling = NULL ;
  m_bGotPowerSchemeBeforeDisabling = false ;
  m_hinstancePowerProfDLL = 
    //If the function fails, the return value is NULL.
    ::LoadLibraryA( "PowrProf.dll" //LPCSTR / char * 
      );
  if( m_hinstancePowerProfDLL )
  {
    InitializeFunctionPointers();
    if( GetActivePwrScheme( & m_uiPowerSchemeIDBeforeDisabling ) )
      mp_uiPowerSchemeIDBeforeDisabling = & m_uiPowerSchemeIDBeforeDisabling ;
  }
  else
  {
    SetFunctionPointersToNULL();
  }
}

void PowerProfUntilWin6DynLinked::InitializeFunctionPointers()
{
  std::string strFuncName ;

  strFuncName = "CanUserWritePwrScheme" ;
  m_pfncanuserwritepwrscheme = (pfnCanUserWritePwrScheme) ::GetProcAddress( 
    m_hinstancePowerProfDLL, strFuncName.c_str() );

  strFuncName = "DeletePwrScheme" ;
  m_pfndeletepwrscheme = (pfnDeletePwrScheme) ::GetProcAddress( 
    m_hinstancePowerProfDLL, strFuncName.c_str() );

  strFuncName = "EnumPwrSchemes" ;
  m_pfnenumpwrschemes = (pfnEnumPwrSchemes) ::GetProcAddress( 
    m_hinstancePowerProfDLL, strFuncName.c_str() );

  strFuncName = "GetActivePwrScheme" ;
  m_pfngetactivepwrscheme = (pfnGetActivePwrScheme) ::GetProcAddress( 
    m_hinstancePowerProfDLL, strFuncName.c_str() );

  strFuncName = "GetCurrentPowerPolicies" ;
  m_pfngetcurrentpowerpolicies = (pfnGetCurrentPowerPolicies) 
    ::GetProcAddress( m_hinstancePowerProfDLL, strFuncName.c_str() );

  strFuncName = "ReadProcessorPwrScheme" ;
  m_pfnreadprocessorpwrscheme = (pfnReadProcessorPwrScheme) 
    ::GetProcAddress( m_hinstancePowerProfDLL, strFuncName.c_str() );

  strFuncName = "ReadPwrScheme" ;
  m_pfnreadpwrscheme = (pfnReadPwrScheme) 
    ::GetProcAddress( m_hinstancePowerProfDLL, strFuncName.c_str() );

  strFuncName = "WriteProcessorPwrScheme" ;
  m_pfnwriteprocessorpwrscheme = 
    //WriteProcessorPwrScheme(...) has the same function sig as 
    //ReadProcessorPwrScheme(...)
    (pfnReadProcessorPwrScheme) 
    ::GetProcAddress( m_hinstancePowerProfDLL, strFuncName.c_str() );

  strFuncName = "SetActivePwrScheme" ;
  m_pfnsetactivepwrscheme = (pfnSetActivePwrScheme) 
    ::GetProcAddress( m_hinstancePowerProfDLL, strFuncName.c_str() );

  strFuncName = "WritePwrScheme" ;
	m_pfnwritepwrscheme = (pfnWritePwrScheme)
    ::GetProcAddress( m_hinstancePowerProfDLL, strFuncName.c_str() );
}

//If the function succeeds, the return value is nonzero.
BOOLEAN WINAPI PowerProfUntilWin6DynLinked::ReadProcessorPwrScheme(
  __in   UINT uiPowerSchemeID,
  __out  PMACHINE_PROCESSOR_POWER_POLICY pMachineProcessorPowerPolicy
  )
{
  BOOLEAN booleanRet ;
  if( m_pfnreadprocessorpwrscheme )
  {
    booleanRet = (*m_pfnreadprocessorpwrscheme) (
      uiPowerSchemeID
      , pMachineProcessorPowerPolicy ) ;
    if( ! booleanRet )
    {
      DWORD dwErrorCode = ::GetLastError() ;
      std::string str = LocalLanguageMessageFromErrorCode(dwErrorCode) ;
      LOGN("error getting CPU power info: error code: " << dwErrorCode << str )
    }
    //If the function succeeds, the return value is nonzero.
    return booleanRet ;
  }
  return FALSE ;
}

BOOLEAN WINAPI PowerProfUntilWin6DynLinked::ReadPwrScheme (
  __in   UINT uiID,
  __out  PPOWER_POLICY pPowerPolicy
  )
{
  if( m_pfnreadpwrscheme )
  {
    LOGN( "power scheme " << uiID )
    return (* m_pfnreadpwrscheme) (
      uiID,
      pPowerPolicy
      ) ;
  }
  return FALSE ;
}

BOOLEAN PowerProfUntilWin6DynLinked::SetActivePwrScheme(
  UINT uiID,
  PGLOBAL_POWER_POLICY lpGlobalPowerPolicy,
  PPOWER_POLICY lpPowerPolicy
  )
{
  if( m_pfnsetactivepwrscheme )
  {
    //Call function in "powrprof.dll" .
    //http://msdn.microsoft.com/en-us/library/aa373200%28VS.85%29.aspx:
    //"If the function succeeds, the return value is nonzero."
    return (*m_pfnsetactivepwrscheme) (  
      uiID,
      lpGlobalPowerPolicy,
      lpPowerPolicy
      ) ;
  }
  return FALSE ;
}

BYTE PowerProfUntilWin6DynLinked::GetPowerSchemeIndex(
  const std::wstring & cr_stdwstrPowerSchemeName 
  , UINT & r_uiPowerSchemeIndexOfWantedName
  )
{
  BYTE byRet = 0 ;
  //UINT uiPowerSchemeIndex ;
  //m_bPowerSchemeWithP0ThrottleNoneFound = false ;
  m_bPowerSchemeFound = false ;
  m_stdwstrWantedPowerScheme = cr_stdwstrPowerSchemeName ;
  //If the function succeeds, the return value is nonzero.
  if( EnumPwrSchemes(
      PwrSchemesEnumProcSearchPowerSchemeByName,
      (LPARAM) //& uiPowerSchemeIndex
        this
      )
    )
  {
    //if( m_bPowerSchemeWithP0ThrottleNoneFound )
    if( m_bPowerSchemeFound )
    {
      r_uiPowerSchemeIndexOfWantedName = m_uiPowerSchemeIndexOfWantedName ;
      byRet = 1 ;
    }
  }
  return byRet ;
}

BOOLEAN PowerProfUntilWin6DynLinked::SetActivePowerScheme(
  //LPTSTR strPowerSchemeName     // name of the power scheme
  const std::wstring & r_stdwstrPowerSchemeName     // name of the power scheme
  )
{
  if( m_pfnsetactivepwrscheme )
  {
    //UINT uiPowerSchemeIndex ;
    //m_bPowerSchemeWithP0ThrottleNoneFound = false ;
    m_bPowerSchemeFound = false ;
    m_stdwstrWantedPowerScheme = r_stdwstrPowerSchemeName ;
    //If the function succeeds, the return value is nonzero.
    if( EnumPwrSchemes(
        PwrSchemesEnumProcSearchPowerSchemeByName,
        (LPARAM) //& uiPowerSchemeIndex
          this
        )
      )
    {
      //if( m_bPowerSchemeWithP0ThrottleNoneFound )
      if( m_bPowerSchemeFound )
        return SetActivePwrScheme( //m_uiPowerSchemeIndexWithP0ThrottleNone 
          //m_uiPowerSchemeIndexOfFoundName
          m_uiPowerSchemeIndexOfWantedName
        //http://msdn.microsoft.com/en-us/library/aa373200(VS.85).aspx:
        //If lpGlobalPowerPolicy is NULL, the function uses the current 
        //global power policy settings set by WriteGlobalPwrPolicy. 
        //Otherwise, the settings in the specified structure replace the 
        //current global power policy settings.
        , NULL 
        , NULL ) ;
      //else

    }
    //return (*m_pfnsetactivepwrscheme) (  
    //  uiID,
    //  lpGlobalPowerPolicy,
    //  lpPowerPolicy
    //  ) ;
  }
  return FALSE ;
}

void PowerProfUntilWin6DynLinked::SetFunctionPointersToNULL()
{
  m_pfncanuserwritepwrscheme = NULL ;
  m_pfnenumpwrschemes = NULL ;
  m_pfngetactivepwrscheme = NULL ;
  m_pfngetcurrentpowerpolicies = NULL ;
  m_pfnreadprocessorpwrscheme = NULL ;
  m_pfnreadpwrscheme = NULL ;
  m_pfnwriteprocessorpwrscheme = NULL ;
  m_pfnsetactivepwrscheme = NULL ;
	m_pfnwritepwrscheme = NULL ;
}

BOOLEAN WINAPI PowerProfUntilWin6DynLinked::WriteProcessorPwrScheme(
  __in   UINT uiPowerSchemeID,
  __in  PMACHINE_PROCESSOR_POWER_POLICY pMachineProcessorPowerPolicy
  )
{
  if( m_pfnwriteprocessorpwrscheme )
  {
    BOOLEAN boolean = CanUserWritePwrScheme() ;
    LOGN("User may write to power scheme: " << (WORD) boolean )
    if( ! boolean )
      LOGN("error code:" << ::GetLastError() )
    //DeactivateCPUscaling(pMachineProcessorPowerPolicy->ProcessorPolicyAc) ;
    //DeactivateCPUscaling(pMachineProcessorPowerPolicy->ProcessorPolicyDc) ;
    //If the function succeeds, the return value is nonzero.
    return ( * m_pfnwriteprocessorpwrscheme) (
      uiPowerSchemeID
      , pMachineProcessorPowerPolicy ) ;
  }
  return 0 ;
}

//This method does NOT return the correct value if ran as normal user
//(i.e. not as admin)
bool PowerProfUntilWin6DynLinked::OtherDVFSisEnabled()
{
  bool bReturn = true ;
  UINT uiPowerSchemeID ;
#ifdef _DEBUG
  OutputAllPowerSchemes() ;
#endif
  if( GetActivePwrScheme( & uiPowerSchemeID)  )
  {
    MACHINE_PROCESSOR_POWER_POLICY machine_processor_power_policy ;
    //If the function succeeds, the return value is nonzero.
    if( ReadProcessorPwrScheme(
        uiPowerSchemeID ,
        & machine_processor_power_policy
        ) 
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
          switch( machine_processor_power_policy.ProcessorPolicyAc.
            DynamicThrottle )
          {
          case PO_THROTTLE_CONSTANT :
          case PO_THROTTLE_NONE :
            bReturn = false ; 
          }
        }
        else if( system_power_status.ACLineStatus == //Offline
          0 )
        {
          switch( machine_processor_power_policy.ProcessorPolicyDc.
            DynamicThrottle )
          {
          case PO_THROTTLE_CONSTANT :
          case PO_THROTTLE_NONE :
            bReturn = false ; 
          }
        }
      }
    }
  }
  return bReturn ;
}

void PowerProfUntilWin6DynLinked::GetAllPowerSchemeNames(
  std::set<std::wstring> & r_stdset_stdwstrPowerSchemeName )
{
  EnumPwrSchemes(PwrSchemesEnumProcGetAllNames,
    (LPARAM) & r_stdset_stdwstrPowerSchemeName ) ;
}

void PowerProfUntilWin6DynLinked::OutputAllPowerSchemes()
{
  EnumPwrSchemes(PwrSchemesEnumProcLogOutput, //NULL
    0 ) ;
}

BOOLEAN PowerProfUntilWin6DynLinked::WritePwrScheme(
	PUINT puiID,
	//LPTSTR lpszName,
	//LPTSTR lpszDescription,
  //e.g. WinXP uses wide chars for service name (and for descr.?)
  LPWSTR lpszName ,
  LPWSTR lpszDescription ,
	PPOWER_POLICY pPowerPolicy
	)
{
	BOOLEAN boolean = FALSE ;
	if( m_pfnwritepwrscheme )
	{
		boolean = ( * m_pfnwritepwrscheme ) (
			puiID,
			lpszName,
			lpszDescription,
			pPowerPolicy
			) ;
	}
	return boolean ;
}
