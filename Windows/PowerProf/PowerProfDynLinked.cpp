//compile errors (indirect (sub-)includes of "sstream" or other) when it wasn't the
//1st include.
#include <global.h> //for LOGN(...)
#include "PowerProfDynLinked.hpp"
#include <Windows/GetWindowsVersion.h>
#include <Windows/PowerProf/PowerProfFromWin6DynLinked.hpp>
#include <Windows/PowerProf/PowerProfUntilWin6DynLinked.hpp>
#include <Controller/stdtstr.hpp> //class tstring

PowerProfDynLinked::PowerProfDynLinked(
  //std::wstring & r_stdwstrProgramName 
  std::tstring & r_stdtstrProgramName 
  )
  : mp_dynfreqscalingaccess ( NULL)
{
  DWORD dwMajor = 0, dwMinor ;
  std::string strWindowsVersion = ::GetWindowsVersion(dwMajor, dwMinor ) ;
  if( dwMajor >= 6 //&& dwMinor >= 1 
    ) 
  {
    mp_dynfreqscalingaccess = new PowerProfFromWin6DynLinked( 
      //r_stdwstrProgramName 
      r_stdtstrProgramName 
      ) ;
    //mp_cpucontroller->SetOtherDVFSaccess( mp_dynfreqscalingaccess ) ;
  }
  else
  {
    mp_dynfreqscalingaccess = new PowerProfUntilWin6DynLinked(
      //r_stdwstrProgramName 
      r_stdtstrProgramName ) ;
  }
}

PowerProfDynLinked::~PowerProfDynLinked()
{
  if( mp_dynfreqscalingaccess )
    delete mp_dynfreqscalingaccess ;
}

bool PowerProfDynLinked::ChangeOtherDVFSaccessPossible()
{
  if( mp_dynfreqscalingaccess )
    return mp_dynfreqscalingaccess->ChangeOtherDVFSaccessPossible() ;
  return false ;
}

BYTE PowerProfDynLinked::DeletePowerScheme( 
  const std::tstring & cr_stdtstrPowerSchemeName )
{
  if( mp_dynfreqscalingaccess )
    return mp_dynfreqscalingaccess->DeletePowerScheme(
      cr_stdtstrPowerSchemeName) ;
  return 0 ;
}

    //return: true=success
bool PowerProfDynLinked::DisableFrequencyScalingByOS()
{
  //return mp_dynfreqscalingaccess->DisableFrequencyScalingByOS() ; 
  bool bDesiredPowerSchemeExists = false ;
  //many gcc compile errors with LOGN _here_:
  // "/usr/lib/gcc/i686-pc-cygwin/4.3.4/include/c++/bits/istream.tcc:944:
  //error: expected primary-expression" etc.
  #ifdef _MSC_VER //if MS-compiler
  LOGN("Should disable Windows' Dynamic Voltage And Frequency Scaling.\n"
    "All available power schemes:")
  #endif
  mp_dynfreqscalingaccess->OutputAllPowerSchemes() ;
  //Even if the access to the power scheme differs between Windows Vista 
  //and XP the logic for setting the power scheme is the same.
  //So to ensure the same implementation, implement the logic here
  //in the superordinate access.
  if( mp_dynfreqscalingaccess->PowerSchemeToSetExists() == 1 )
  {
    //mp_dynfreqscalingaccess->ActivatePowerSchemeToSet() ;
    bDesiredPowerSchemeExists = true ;
  }
  else
  {
    if( mp_dynfreqscalingaccess->CreatePowerSchemeWithWantedName() )
    {
  //many gcc compile errors with LOGN _here_:
  // "/usr/lib/gcc/i686-pc-cygwin/4.3.4/include/c++/bits/istream.tcc:944:
  //error: expected primary-expression" etc.
  #ifdef _MSC_VER //if MS-compiler
      LOGN("creating power scheme succeeded")
  #endif
      bDesiredPowerSchemeExists = true ;
    }
    else
    {
      //many gcc compile errors with LOGN _here_:
      // "/usr/lib/gcc/i686-pc-cygwin/4.3.4/include/c++/bits/istream.tcc:944:
      //error: expected primary-expression" etc.
      #ifdef _MSC_VER //if MS-compiler
      LOGN("creating power scheme failed")
       #endif
    }
  }
  if( bDesiredPowerSchemeExists )
  {
    mp_dynfreqscalingaccess->DisableDVFSforPowerSchemeToSet() ;
    mp_dynfreqscalingaccess->ActivatePowerSchemeToSet() ;
  }
  return false ;
}

unsigned char PowerProfDynLinked::EnableFrequencyScalingByOS()
{
  return mp_dynfreqscalingaccess->EnableFrequencyScalingByOS() ;
}

bool PowerProfDynLinked::OtherDVFSisEnabled()
{
  return mp_dynfreqscalingaccess->OtherDVFSisEnabled() ;
}

void PowerProfDynLinked::OutputAllPowerSchemes()
{
  mp_dynfreqscalingaccess->OutputAllPowerSchemes() ;
}
