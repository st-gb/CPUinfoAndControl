/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
#include "DynFreqScalingAccess.hpp"

//#define COMPILE_WITH_VISTA_POWERPROFILE_ACCESS
#ifdef COMPILE_WITH_VISTA_POWERPROFILE_ACCESS
//#include <windef.h> //for PowrProf.h
#include <WINDOWS.H> //for PowrProf.h: SYSTEM_POWER_STATE
//Needed to get the declarations inside the Windows 6.1 (Vista,...) powrprof.h.
#define NTDDI_VERSION NTDDI_VISTA

//http://msdn.microsoft.com/en-us/library/7f0aews7(VS.80).aspx:
//"The following pragma causes the linker to search for the EMAPI.LIB library 
//while linking. The linker searches first in the current working directory
//and then in the path specified in the LIB environment variable."
#pragma comment( lib, "PowrProf" )

//#ifdef LINK_TO_POWERPROF_STATICALLY
  //This must be the PowrProf.h from the Windows (platform) SDK for Vista, ie. SDK version "6.1" .
  #include <PowrProf.h> // for PowerWriteACValueIndex()
//#else
  
//#endif

//the follwong values can get viewed via "powercfg -aliases"
//The following global variables must NOT be defined in the header file,
//else the linker says: multiple definitions for these vars (because of 
//the includes of the header wheere they wre defined
//#define PROCTHROTTLEMAX bc5038f7-23e0-4960-96da-33abaf5935ec
GUID g_guidPROCTHROTTLEMAX = {0xbc5038f7,0x23e0,0x4960,
  {0x96,0xda,0x33,0xab,0xaf,0x59,0x35,0xec}};
  //{0xec,0x35,0x59,0xaf,0xab,0x33,0xda,0x96}};
//#define PROCTHROTTLEMIN 893dee8e-2bef-41e0-89c6-b55d0929964c
GUID g_guidPROCTHROTTLEMIN = {0x893dee8e,0x2bef,0x41e0,
  {0x89,0xc6,0xb5,0x5d,0x09,0x29,0x96,0x4c}};

#include <ios> //for "std::hex"
#include <strstream> //for std::ostrstream
#include "../global.h" //for DEBUG(...) etc.

  //std::string 
  char *
  //void 
  GetGUIDasString(GUID & r_guid
    //,std::ostrstream & r_ostrstream
    )
  {
    //std::string str ;
    std::ostrstream r_ostrstream ;
    r_ostrstream << std::hex << //"%lx-%x-%x-%x%x-%x%x%x%x%x%x", 
    r_guid.Data1 //, 
    << "-" << r_guid.Data2//, 
    << "-" << r_guid.Data3//, 
    << //For the char value to be interpreted as number
      (WORD)r_guid.Data4[0]
    <<//,
      //For the char value to be interpreted as number
      (WORD)r_guid.Data4[1]
    //(DWORD*)r_guid.Data4+2,
    //(DWORD)r_guid.Data4+4
    //,
    << "-" << 
      //For the char value to be interpreted as number
      (WORD)r_guid.Data4[2]
    //,
    << //For the char value to be interpreted as number
      (WORD)r_guid.Data4[3]
    //,
    << //For the char value to be interpreted as number
      (WORD)r_guid.Data4[4]
    //,
    << //For the char value to be interpreted as number
      (WORD)r_guid.Data4[5]
    //,
    << //For the char value to be interpreted as number
      (WORD)r_guid.Data4[6]
    //,
    << //For the char value to be interpreted as number
      (WORD)r_guid.Data4[7] ;
    //Must set this, else text may follow after the string we want.
    //I had program crashes with the following method:
    //pch[r_ostrstream.pcount()] = '\0' ;
    r_ostrstream.put('\0');
    return r_ostrstream.str() ;
  }
#endif //#ifdef COMPILE_WITH_VISTA_POWERPROFILE_ACCESS

//void 
  bool Windows_API::DynFreqScalingAccess::DisableFrequencyScalingByOS()
{
    bool bReturn = false ;
    //"Vista and Windows Server 2008 use a new power policy infrastructure
    //and API.  This page details all the power APIs and indicates what is
    //supported by the various Windows versions:
    //http://msdn2.microsoft.com/en-us/library/aa373163(VS.85).aspx
    //While some of the older APIs are supported in Vista, it is recommended
    //that the new APIs be used on Vista and above.
    //To give some background, in Vista a power plan (sometimes called a 
    //power scheme) contains the power setting values that are actually in
    //effect on the system.
    //A power plan is uniquely identified by a GUID and contains an AC and DC
    //values for each power setting.
    //There can only be one power plan active at a time on the system.
    //The current plan can be obtained using the PowerGetActiveScheme.
    //Like power plans, power settings are also identified by unique GUIDs 
    //and include a friendly name, description, allowable values, and default
    //values for AC and DC.
    //Power setting values can be modified using the PowerWriteACValueIndex
    //and PowerWriteDCValueIndex routines.
    //Note that PowerSetActiveScheme must be called after modifying any of
    //the power settings in order to activate the new values.
    //Specifically to your question, there are two settings that allow you
    //to adjust the minimum and maximum state for processor power management.
    //These are expressed as a percentage of the maximum processor frequency.
    //Here is a summary of these two settings:

    //Minimum processor performance state. The performance state is specified
    //as a percentage of maximum processor frequency.
      //GUID: 893dee8e-2bef-41e0-89c6-b55d0929964c
      //PowerCfg Alias: PROCTHROTTLEMIN
      //Minimum Value: 0
      //Maximum Value: 100
      //Label: Percentage (%)
      //Hidden: No
    
    //Description: Maximum processor performance state. The performance state is specified as a percentage of maximum processor frequency.
      //GUID: bc5038f7-23e0-4960-96da-33abaf5935ec
      //PowerCfg Alias: PROCTHROTTLEMAX
      //Minimum Value: 0
      //Maximum Value: 100
      //Label: Percentage (%)
      //Hidden: No

    //Additional information on Vista power policy is described in detail in 
    //the following whitepaper: 
    //http://www.microsoft.com/whdc/system/pnppwr/powermgmt/PMpolicy_Vista.mspx

#ifdef COMPILE_WITH_VISTA_POWERPROFILE_ACCESS
    //GUID *pActivePolicyGuid;
    //DEBUG("Before getting active power scheme\n");
    
    if ( ::PowerGetActiveScheme(
      //This parameter is reserved for future use and must be set to NULL.
      NULL, 
      //&pActivePolicyGuid
      & mp_guidPowerSchemeBeforeDisabling ) == ERROR_SUCCESS )
    {
      //Returns ERROR_SUCCESS (zero) if the call was successful, and a non-zero value if the call failed.
      //GUID *pPwrGUID;
      //PowerGetActiveScheme(NULL, &pPwrGUID);
      //DEBUG("active power scheme:%s\n", ::GetGUIDasString(//*pActivePolicyGuid
      //  *mp_guidPowerSchemeBeforeDisabling)//.c_str()
      //  );
      //LOG( "Active power scheme:" << ::GetGUIDasString( //*pActivePolicyGuid
      //    *mp_guidPowerSchemeBeforeDisabling) //.c_str()
      //    << "\n" );
      //DEBUG("Before writing throttle max for AC at GUID %s\n",
      //  ::GetGUIDasString(g_guidPROCTHROTTLEMAX)//.c_str()
      //  );
      //LOG( "Before writing throttle max for AC at GUID " << 
      //    ::GetGUIDasString(g_guidPROCTHROTTLEMAX)//.c_str() 
      //    << "\n" );
      ::PowerReadACValueIndex (
        //__in_opt HKEY RootPowerKey: This parameter is reserved for future use and must be set to NULL.
        NULL,
        //__in_opt CONST GUID *SchemeGuid,
        //pActivePolicyGuid,
        mp_guidPowerSchemeBeforeDisabling,
        //__in_opt CONST GUID *SubGroupOfPowerSettingsGuid,
        //GUID_PROCESSOR_SETTINGS_SUBGROUP defined in Platform SDK 6.1's "include/WinNT.h"
        & GUID_PROCESSOR_SETTINGS_SUBGROUP,
        //__in_opt CONST GUID *PowerSettingGuid,
        //& g_guidPROCTHROTTLEMAX
        //defined in Platform SDK 6.1's "include/WinNT.h"
        & GUID_PROCESSOR_THROTTLE_MAXIMUM ,
        //__out LPDWORD AcValueIndex
        & m_dwOldACProcThrottleMaxValue
        );
      //DEBUG("AC max. throttle:%lu\n",m_dwOldACProcThrottleMaxValue);
      LOG("AC max. throttle:" << m_dwOldACProcThrottleMaxValue << "\n" );
      PowerWriteACValueIndex(
        //RootPowerKey: This parameter is reserved for future use and must be set to NULL.
        NULL,      
        //__in      const GUID *SchemeGuid,
        //pPwrGUID,
        //pActivePolicyGuid,
        mp_guidPowerSchemeBeforeDisabling,
        //__in_opt  const GUID *SubGroupOfPowerSettingsGuid,
        &GUID_PROCESSOR_SETTINGS_SUBGROUP,
        //__in_opt  const GUID *PowerSettingGuid,
        //PROCTHROTTLEMIN,
        &g_guidPROCTHROTTLEMAX,
        //__in      DWORD AcValueIndex
        //25
        50
      );
      DEBUG("Before writing throttle min for AC\n");
      PowerReadACValueIndex (
        //__in_opt HKEY RootPowerKey: This parameter is reserved for future use and must be set to NULL.
        NULL,
        //__in_opt CONST GUID *SchemeGuid,
        //pActivePolicyGuid,
        mp_guidPowerSchemeBeforeDisabling,
        //__in_opt CONST GUID *SubGroupOfPowerSettingsGuid,
        &GUID_PROCESSOR_SETTINGS_SUBGROUP,
        //__in_opt CONST GUID *PowerSettingGuid,
        &g_guidPROCTHROTTLEMIN,
        //__out LPDWORD AcValueIndex
        &m_dwOldACProcThrottleMinValue
        );
      //DEBUG("AC min. throttle:%lu\n",m_dwOldACProcThrottleMinValue);
      LOG("AC min. throttle:" << m_dwOldACProcThrottleMinValue << "\n" );
      PowerWriteACValueIndex(
        //RootPowerKey: This parameter is reserved for future use and must be set to NULL.
        NULL,
        //__in      const GUID *SchemeGuid,
        //pPwrGUID,
        //pActivePolicyGuid,
        mp_guidPowerSchemeBeforeDisabling,
        //__in_opt  const GUID *SubGroupOfPowerSettingsGuid,
        &GUID_PROCESSOR_SETTINGS_SUBGROUP,
        //__in_opt  const GUID *PowerSettingGuid,
        //PROCTHROTTLEMAX,
        &g_guidPROCTHROTTLEMIN,
        //__in      DWORD AcValueIndex
        //25
        //100
        50
      );
      PowerReadDCValueIndex (
        //__in_opt HKEY RootPowerKey: This parameter is reserved for future use and must be set to NULL.
        NULL,
        //__in_opt CONST GUID *SchemeGuid,
        //pActivePolicyGuid,
        mp_guidPowerSchemeBeforeDisabling,
        //__in_opt CONST GUID *SubGroupOfPowerSettingsGuid,
        &GUID_PROCESSOR_SETTINGS_SUBGROUP,
        //__in_opt CONST GUID *PowerSettingGuid,
        &g_guidPROCTHROTTLEMAX,
        //__out LPDWORD AcValueIndex
        &m_dwOldDCProcThrottleMaxValue
        );
      //DEBUG("DC max. throttle:%lu\n",m_dwOldDCProcThrottleMaxValue);
      LOG("DC max. throttle:" << m_dwOldDCProcThrottleMaxValue << "\n" );
      PowerWriteDCValueIndex(
        //RootPowerKey: This parameter is reserved for future use and must be set to NULL.
        NULL,      
        //__in      const GUID *SchemeGuid,
        //pPwrGUID,
        //pActivePolicyGuid,
        mp_guidPowerSchemeBeforeDisabling,
        //__in_opt  const GUID *SubGroupOfPowerSettingsGuid,
        &GUID_PROCESSOR_SETTINGS_SUBGROUP,
        //__in_opt  const GUID *PowerSettingGuid,
        //PROCTHROTTLEMIN,
        &g_guidPROCTHROTTLEMAX,
        //__in      DWORD AcValueIndex
        //25
        //100
        50
      );
      DEBUG("Before writing throttle min for AC\n");
      PowerReadDCValueIndex (
        //__in_opt HKEY RootPowerKey: This parameter is reserved for future use and must be set to NULL.
        NULL,
        //__in_opt CONST GUID *SchemeGuid,
        //pActivePolicyGuid,
        mp_guidPowerSchemeBeforeDisabling,
        //__in_opt CONST GUID *SubGroupOfPowerSettingsGuid,
        &GUID_PROCESSOR_SETTINGS_SUBGROUP,
        //__in_opt CONST GUID *PowerSettingGuid,
        &g_guidPROCTHROTTLEMIN,
        //__out LPDWORD AcValueIndex
        &m_dwOldDCProcThrottleMinValue
        );
      //DEBUG("DC min. throttle:%lu\n",m_dwOldDCProcThrottleMinValue);
      LOG("DC min. throttle:" << m_dwOldDCProcThrottleMinValue << "\n" );
      PowerWriteDCValueIndex(
        //RootPowerKey: This parameter is reserved for future use and must be set to NULL.
        NULL,
        //__in      const GUID *SchemeGuid,
        //pPwrGUID,
        //pActivePolicyGuid,
        mp_guidPowerSchemeBeforeDisabling,
        //__in_opt  const GUID *SubGroupOfPowerSettingsGuid,
        &GUID_PROCESSOR_SETTINGS_SUBGROUP,
        //__in_opt  const GUID *PowerSettingGuid,
        //PROCTHROTTLEMAX,
        &g_guidPROCTHROTTLEMIN,
        //__in      DWORD AcValueIndex
        //25
        50
      );

      //DWORD WINAPI PowerWriteACValueIndex(...):Changes to the settings for the active power scheme do not take effect until you call the PowerSetActiveScheme function.
      
      //Returns ERROR_SUCCESS (zero) if the call was successful, and a non-zero value if the call failed.
      ::PowerSetActiveScheme(
        //UserRootPowerKey [in, optional] 
        //This parameter is reserved for future use and must be set to NULL.
        NULL,
        //pActivePolicyGuid
        mp_guidPowerSchemeBeforeDisabling);
      //::LocalFree(pActivePolicyGuid);
      bReturn = true ;
    }
#endif //#ifdef COMPILE_WITH_VISTA_POWERPROFILE_ACCESS
    return bReturn ;
}

//#ifdef COMPILE_WITH_VISTA_POWERPROFILE_ACCESS

bool Windows_API::DynFreqScalingAccess::OSsDVFSisEnabled()
{
  #ifdef COMPILE_WITH_VISTA_POWERPROFILE_ACCESS
  GUID * p_guidActivePowerScheme ;
  if ( ::PowerGetActiveScheme(
      //This parameter is reserved for future use and must be set to NULL.
      NULL, 
      //&pActivePolicyGuid
      & p_guidActivePowerScheme ) == ERROR_SUCCESS )
    {
      DWORD dwACProcThrottleMaxValue ;
      //Returns ERROR_SUCCESS (zero) if the call was successful, and a non-zero value if the call failed.
      //GUID *pPwrGUID;
      //PowerGetActiveScheme(NULL, &pPwrGUID);
      ::PowerReadACValueIndex (
        //__in_opt HKEY RootPowerKey: This parameter is reserved for future use and must be set to NULL.
        NULL,
        //__in_opt CONST GUID *SchemeGuid,
        //pActivePolicyGuid,
        p_guidActivePowerScheme ,
        //__in_opt CONST GUID *SubGroupOfPowerSettingsGuid,
        & GUID_PROCESSOR_SETTINGS_SUBGROUP ,
        //__in_opt CONST GUID *PowerSettingGuid,
        & g_guidPROCTHROTTLEMAX ,
        //__out LPDWORD AcValueIndex
        & dwACProcThrottleMaxValue
        );
  }
  #endif //#ifdef COMPILE_WITH_VISTA_POWERPROFILE_ACCESS
  return false ;
}

//bool 
BYTE Windows_API::DynFreqScalingAccess::EnableFrequencyScalingByOS()
{
  bool bReturn = false ;
#ifdef COMPILE_WITH_VISTA_POWERPROFILE_ACCESS
  //GUID *pActivePolicyGuid;
  DEBUG("Before getting active power scheme\n");
	if ( //PowerGetActiveScheme(
    ::PowerSetActiveScheme(
    //This parameter is reserved for future use and must be set to NULL.
    NULL,
    mp_guidPowerSchemeBeforeDisabling) == ERROR_SUCCESS 
    )
  {
    DEBUG("Setting back the power scheme config\n");
    //DEBUG("active power scheme:%s\n", ::GetGUIDasString(//*pActivePolicyGuid
    //  *mp_guidPowerSchemeBeforeDisabling)//.c_str()
    //  );
    DEBUG("active power scheme:%s\n" << ::GetGUIDasString(//*pActivePolicyGuid
      *mp_guidPowerSchemeBeforeDisabling)//.c_str()
      );
    //DEBUG("Before writing throttle max \"%lu\"for AC at GUID %s\n",
    //  m_dwOldACProcThrottleMaxValue,
    //  ::GetGUIDasString(g_guidPROCTHROTTLEMAX)//.c_str()
    //  );
    DEBUG("Before writing throttle max \"%lu\"for AC at GUID %s\n" <<
      m_dwOldACProcThrottleMaxValue <<
      ::GetGUIDasString(g_guidPROCTHROTTLEMAX)//.c_str()
      );
    //Restore the original power plan/scheme values.
    if(PowerWriteACValueIndex(
      //RootPowerKey: This parameter is reserved for future use and must be set to NULL.
      NULL,
      //__in      const GUID *SchemeGuid,
      //pPwrGUID,
      //pActivePolicyGuid,
      mp_guidPowerSchemeBeforeDisabling,
      //__in_opt  const GUID *SubGroupOfPowerSettingsGuid,
      &GUID_PROCESSOR_SETTINGS_SUBGROUP,
      //__in_opt  const GUID *PowerSettingGuid,
      //PROCTHROTTLEMIN,
      &g_guidPROCTHROTTLEMAX,
      //__in      DWORD AcValueIndex
      m_dwOldACProcThrottleMaxValue
      ) == ERROR_SUCCESS
      )
    {
      DEBUG("Successfully called PowerWriteACValueIndex\n");
    }
    else
    {
      DEBUG("Error calling PowerWriteACValueIndex\n");
    }
    PowerWriteACValueIndex(
      //RootPowerKey: This parameter is reserved for future use and must be set to NULL.
      NULL,
      //__in      const GUID *SchemeGuid,
      //pPwrGUID,
      //pActivePolicyGuid,
      mp_guidPowerSchemeBeforeDisabling,
      //__in_opt  const GUID *SubGroupOfPowerSettingsGuid,
      &GUID_PROCESSOR_SETTINGS_SUBGROUP,
      //__in_opt  const GUID *PowerSettingGuid,
      //PROCTHROTTLEMIN,
      &g_guidPROCTHROTTLEMIN,
      //__in      DWORD AcValueIndex
      m_dwOldACProcThrottleMinValue
    );
    PowerWriteDCValueIndex(
      //RootPowerKey: This parameter is reserved for future use and must be set to NULL.
      NULL,
      //__in      const GUID *SchemeGuid,
      //pPwrGUID,
      //pActivePolicyGuid,
      mp_guidPowerSchemeBeforeDisabling,
      //__in_opt  const GUID *SubGroupOfPowerSettingsGuid,
      &GUID_PROCESSOR_SETTINGS_SUBGROUP,
      //__in_opt  const GUID *PowerSettingGuid,
      //PROCTHROTTLEMIN,
      &g_guidPROCTHROTTLEMAX,
      //__in      DWORD AcValueIndex
      m_dwOldDCProcThrottleMaxValue
    );
    //"Returns ERROR_SUCCESS (zero) if the call was successful, and a 
    //non-zero value if the call failed."
    PowerWriteDCValueIndex(
      //RootPowerKey: This parameter is reserved for future use and must be set to NULL.
      NULL,
      //__in      const GUID *SchemeGuid,
      //pPwrGUID,
      //pActivePolicyGuid,
      mp_guidPowerSchemeBeforeDisabling,
      //__in_opt  const GUID *SubGroupOfPowerSettingsGuid,
      &GUID_PROCESSOR_SETTINGS_SUBGROUP,
      //__in_opt  const GUID *PowerSettingGuid,
      //PROCTHROTTLEMIN,
      &g_guidPROCTHROTTLEMIN,
      //__in      DWORD AcValueIndex
      m_dwOldDCProcThrottleMinValue
    );
    //DWORD WINAPI PowerWriteACValueIndex(...):Changes to the settings for the active power scheme do not take effect until you call the PowerSetActiveScheme function.

    //Returns ERROR_SUCCESS (zero) if the call was successful, and a 
    //non-zero value if the call failed.
    if( ::PowerSetActiveScheme(
      //UserRootPowerKey [in, optional]
      //This parameter is reserved for future use and must be set to NULL.
      NULL,
      //pActivePolicyGuid
      mp_guidPowerSchemeBeforeDisabling ) == ERROR_SUCCESS 
      )
      bReturn = true ;
    //::LocalFree(pActivePolicyGuid);
    ::LocalFree(mp_guidPowerSchemeBeforeDisabling);
    DEBUG("Setting back the power scheme config--end\n");
  }
#endif //#ifdef COMPILE_WITH_VISTA_POWERPROFILE_ACCESS
  return bReturn ;
}

Windows_API::DynFreqScalingAccess::~DynFreqScalingAccess()
{
  EnableFrequencyScalingByOS() ;
}

//Power setting values can be modified using the PowerWriteACValueIndex and PowerWriteDCValueIndex routines.  Note that PowerSetActiveScheme must be called after modifying any of the power settings in order to activate the new values.
//Description
// Minimum processor performance state. The performance state is specified as a percentage of maximum processor frequency.
// 
//GUID
// 893dee8e-2bef-41e0-89c6-b55d0929964c
// 
//PowerCfg Alias
// PROCTHROTTLEMIN
// 
//Minimum Value
// 0
// 
//Maximum Value
// 100
// 
//Label
// Percentage (%)
// 
//Hidden
// No
// 

//http://msdn.microsoft.com/en-us/library/aa373183(VS.85).aspx:
//to avoid dynamic freq scaling: 
//PO_THROTTLE_CONSTANT (=1)
//PO_THROTTLE_NONE (= 0)

//PowerWriteACValueIndex(

//  GUID *pPwrGUID;
//GUID subGUID = //GUID_VIDEO_SUBGROUP;
//  GUID_PROCESSOR_SETTINGS_SUBGROUP ;
//GUID BriGUID = GUID_VIDEO_BRIGHTNESS;

//PowerGetActiveScheme(NULL, &pPwrGUID);

//// ...Get new LCD brightness level by user input and save it to dwLevel;

//if (PowerSource())
//{ // AC Power
//PowerWriteACValueIndex(NULL, pPwrGUID, &subGUID, &BriGUID, dwLevel);
//}
//PowerSetActiveScheme(NULL, pPwrGUID); // It seems not working.
