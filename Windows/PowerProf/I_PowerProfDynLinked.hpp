#pragma once

#include <Controller/IDynFreqScalingAccess.hpp>
#include <string> //for std::wstring
#include <set> //std::set
#include <Windows_compatible_typedefs.h>

#define CREATE_POWER_SCHEME_SUCCEEDED 0

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
  virtual BYTE DeletePowerScheme(
    const std::tstring & cr_stdtstrPowerSchemeName ) = 0 ;
  bool DisableFrequencyScalingByOS() ;

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
//  virtual void OutputAllPowerSchemes() = 0 ;
  virtual void GetActivePowerSchemeName(
    std::wstring & r_stdwstrActivePowerSchemeName) {}
  virtual void GetAllPowerSchemeNames(std::set<std::wstring> &
      stdset_stdwstrPowerSchemeName ) {} ;
  virtual unsigned char PowerSchemeToSetExists() 
  { 
    return 0 ; 
  }
  //to avoid g++ compiler warning "`class PowerProfUntilWin6DynLinked' has
  // virtual functions but non-virtual destructor"
  virtual ~I_PowerProfDynLinked() { } ;
  virtual //BYTE
    DWORD SetActivePowerScheme(
    const std::wstring & r_stdwstrPowerSchemeName ) { return 0 ; }
} ;
