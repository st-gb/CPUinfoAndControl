#include "PowerProfDynLinked.hpp"
#include <Windows/GetWindowsVersion.h>
#include <Windows/PowerProf/PowerProfFromWin6DynLinked.hpp>
#include <Windows/PowerProf/PowerProfUntilWin6DynLinked.hpp>
#include "global.h" //LOGN

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

    //return: true=success
bool PowerProfDynLinked::DisableFrequencyScalingByOS()
{
  //return mp_dynfreqscalingaccess->DisableFrequencyScalingByOS() ; 
  bool bDesiredPowerSchemeExists = false ;
  //Even if the access to the power scheme differs between Windows Vista 
  //and XP the logic for setting the power scheme is the same.
  //So to ensure the same implementation, implement the logic here
  //in the superordinate access.
  if( mp_dynfreqscalingaccess->PowerSchemeToSetExists() )
  {
    //mp_dynfreqscalingaccess->ActivatePowerSchemeToSet() ;
    bDesiredPowerSchemeExists = true ;
  }
  else
  {
    if( mp_dynfreqscalingaccess->CreatePowerSchemeWithWantedName() )
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
