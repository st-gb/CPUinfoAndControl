#pragma once

#include <Controller/IDynFreqScalingAccess.hpp>
#include <string> //for std::wstring
#include <Windows_compatible_typedefs.h>

//Base class of PowerProf access for Windows < version 6 and Windows >
//version 6. Because even if the access (API) to the power scheme differs
//between Windows Vista
//and XP the logic (order of equivalent function calls for setting the
//power scheme is the same.
//So to ensure the same implementation, implement the logic here
//in the superordinate access.
class I_PowerProfDynLinked
  : public IDynFreqScalingAccess
{
protected:
  //std::string * mp_stdstr
  //std::wstring m_stdwstrWantedPowerScheme = L"CPUcontrol" ;
  //Power scheme names are in WIDE chars.
  //std::wstring * mp_stdwstrPowerSchemeName ;
  //use a copy rather than a pointer because with a copy I do not
  // need to are abput the validity of pointer (if it points 
  //to valid content )
  std::wstring m_stdwstrPowerSchemeName ;
  //stlpmtx_std::wstring m_stdwstrPowerSchemeName ;
public:
  virtual unsigned char ActivatePowerSchemeToSet() { return 0 ; }
  virtual unsigned char CreatePowerScheme(const std::wstring & wstr) { return 0 ; }
  virtual unsigned char CreatePowerSchemeWithWantedName() { return 0 ; }
  bool DisableFrequencyScalingByOS()
  {
    bool bDesiredPowerSchemeExists = false ;
    //many gcc compile errors with LOGN _here_:
    // "/usr/lib/gcc/i686-pc-cygwin/4.3.4/include/c++/bits/istream.tcc:944:
    //error: expected primary-expression" etc.
    #ifdef _MSC_VER //if MS-compiler
    LOGN("Should disable Windows' Dynamic Voltage And Frequency Scaling.\n"
      "All available power schemes:")
    #endif
    OutputAllPowerSchemes() ;
    //Even if the access to the power scheme differs between Windows Vista
    //and XP the logic for setting the power scheme is the same.
    //So to ensure the same implementation, implement the logic here
    //in the superordinate access.
    if( PowerSchemeToSetExists() == 1 )
    {
      //mp_dynfreqscalingaccess->ActivatePowerSchemeToSet() ;
      bDesiredPowerSchemeExists = true ;
    }
    else
    {
      if( CreatePowerSchemeWithWantedName() )
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
      DisableDVFSforPowerSchemeToSet() ;
      ActivatePowerSchemeToSet() ;
    }
    return false ;
  }
  //unsigned char EnableFrequencyScalingByOS() ;
  //bool OtherDVFSisEnabled() ;
  virtual unsigned char DisableDVFSforPowerSchemeToSet()
  {
    return 0 ; 
  }
  const std::wstring & GetPowerSchemeName() const 
  {
    return m_stdwstrPowerSchemeName ;
  }
  virtual void OutputAllPowerSchemes() = 0 ;
  virtual unsigned char PowerSchemeToSetExists() 
  { 
    return 0 ; 
  }
  virtual BYTE DeletePowerScheme( 
    const std::tstring & cr_stdtstrPowerSchemeName ) = 0 ;
} ;
