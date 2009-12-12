#pragma once

#include <Controller/IDynFreqScalingAccess.hpp>
#include <string> //for std::wstring
#include <Windows_compatible_typedefs.h>

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
  virtual unsigned char CreatePowerSchemeWithWantedName() { return 0 ; }
  //bool DisableFrequencyScalingByOS() ;
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
  virtual unsigned char PowerSchemeToSetExists() 
  { 
    return 0 ; 
  }
  virtual BYTE DeletePowerScheme( 
    const std::tstring & cr_stdtstrPowerSchemeName ) = 0 ;
} ;
