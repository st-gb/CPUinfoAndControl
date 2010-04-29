//compile errors (indirect (sub-)includes of "sstream" or other) when it wasn't the
//1st include.
#include <global.h> //for LOGN(...)
#include <stdio.h> //wprintf()
#include <set> //std::set
//#include <iostream>
#include "PowerProfDynLinked.hpp"
#include <Windows/GetWindowsVersion.h>
#include <Windows/PowerProf/PowerProfFromWin6DynLinked.hpp>
#include <Windows/PowerProf/PowerProfUntilWin6DynLinked.hpp>
#include <Controller/stdtstr.hpp> //class tstring

PowerProfDynLinked::PowerProfDynLinked(
  //std::wstring & r_stdwstrProgramName 
  std::tstring & r_stdtstrProgramName 
  )
  : mp_i_powerprofdynlinked ( NULL)
{
  DWORD dwMajor = 0, dwMinor ;
  std::string strWindowsVersion = ::GetWindowsVersion(dwMajor, dwMinor ) ;
  if( dwMajor >= 6 //&& dwMinor >= 1 
    ) 
  {
    mp_i_powerprofdynlinked = new PowerProfFromWin6DynLinked(
      //r_stdwstrProgramName 
      r_stdtstrProgramName 
      ) ;
    //mp_i_cpucontroller->SetOtherDVFSaccess( mp_dynfreqscalingaccess ) ;
  }
  else
  {
    mp_i_powerprofdynlinked = new PowerProfUntilWin6DynLinked(
      //r_stdwstrProgramName 
      r_stdtstrProgramName ) ;
  }
//  #ifdef _DEBUG
//    mp_i_powerprofdynlinked->OutputAllPowerSchemes() ;
//  #endif
}

PowerProfDynLinked::~PowerProfDynLinked()
{
  if( mp_i_powerprofdynlinked )
    delete mp_i_powerprofdynlinked ;
}

bool PowerProfDynLinked::ChangeOtherDVFSaccessPossible()
{
  if( mp_i_powerprofdynlinked )
    return mp_i_powerprofdynlinked->ChangeOtherDVFSaccessPossible() ;
  return false ;
}

BYTE PowerProfDynLinked::CreatePowerScheme(
  const std::wstring & cr_stdwstrPowerSchemeName )
{
  if( mp_i_powerprofdynlinked )
    return mp_i_powerprofdynlinked->CreatePowerScheme(
      cr_stdwstrPowerSchemeName) ;
  return 0 ;
}

BYTE PowerProfDynLinked::DeletePowerScheme( 
  const std::tstring & cr_stdtstrPowerSchemeName )
{
  if( mp_i_powerprofdynlinked )
    return mp_i_powerprofdynlinked->DeletePowerScheme(
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
//  mp_i_powerprofdynlinked->OutputAllPowerSchemes() ;
  //Even if the access to the power scheme differs between Windows Vista 
  //and XP the logic for setting the power scheme is the same.
  //So to ensure the same implementation, implement the logic here
  //in the superordinate access.
  if( mp_i_powerprofdynlinked->PowerSchemeToSetExists() == 1 )
  {
    //mp_dynfreqscalingaccess->ActivatePowerSchemeToSet() ;
    bDesiredPowerSchemeExists = true ;
  }
  else
  {
    if( mp_i_powerprofdynlinked->CreatePowerSchemeWithWantedName() 
      == CREATE_POWER_SCHEME_SUCCEEDED )
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
    mp_i_powerprofdynlinked->DisableDVFSforPowerSchemeToSet() ;
    mp_i_powerprofdynlinked->ActivatePowerSchemeToSet() ;
  }
  return false ;
}

unsigned char PowerProfDynLinked::EnableFrequencyScalingByOS()
{
  return mp_i_powerprofdynlinked->EnableFrequencyScalingByOS() ;
}

bool PowerProfDynLinked::EnablingIsPossible()
{
  return mp_i_powerprofdynlinked->EnablingIsPossible() ;
}

void PowerProfDynLinked::GetAllPowerSchemeNames(
  std::set<std::wstring> & r_stdset_stdwstrPowerSchemeName )
{
  mp_i_powerprofdynlinked->//OutputAllPowerSchemes() ;
    GetAllPowerSchemeNames(r_stdset_stdwstrPowerSchemeName) ;
}

std::tstring PowerProfDynLinked::GetEnableDescription()
{
  return mp_i_powerprofdynlinked->GetEnableDescription() ;
}

bool PowerProfDynLinked::OtherDVFSisEnabled()
{
  return mp_i_powerprofdynlinked->OtherDVFSisEnabled() ;
}

void PowerProfDynLinked::OutputAllPowerSchemes()
{
  std::set<std::wstring> stdset_stdwstrPowerSchemeName ;
  mp_i_powerprofdynlinked->//OutputAllPowerSchemes() ;
    GetAllPowerSchemeNames(stdset_stdwstrPowerSchemeName) ;
  for(std::set<std::wstring>::const_iterator c_iter =
    stdset_stdwstrPowerSchemeName.begin() ;
    c_iter != stdset_stdwstrPowerSchemeName.end() ; ++ c_iter
    )
  {
#if defined(_DEBUG)
//      #if defined(__MINGW32__) //MinGW does not know "wcout"
      wprintf( L"%s\n", c_iter->c_str() ) ;
//      #else
//        std::wcout << *c_iter << L'\n' ;
//      #endif
    #endif
  }
}

BYTE PowerProfDynLinked::SetActivePowerScheme(
  const std::wstring & r_stdwstrPowerSchemeName )
{
  return mp_i_powerprofdynlinked->SetActivePowerScheme(
    r_stdwstrPowerSchemeName ) ;
}
