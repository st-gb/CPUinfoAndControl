#pragma once

#include <Controller/IDynFreqScalingAccess.hpp>
#include <Controller/stdtstr.hpp> //for std::tstring 
//#include <string> //for std::wstring 

class I_PowerProfDynLinked ;

//Class for both Window version >=6 and Windows version < 6 
//Windows  DVFS access
class PowerProfDynLinked
  : public IDynFreqScalingAccess
{
public:
    //return: true=success
  PowerProfDynLinked(
    //std::wstring & r_stdwstrProgramName ) ;
    std::tstring & r_stdtstrProgramName ) ;
  //PowerProfDynLinked(std::string & stdstrPowerSchemeNameToChoose ) ;
  ~PowerProfDynLinked() ;
  //Must be a pointer because the concrete type is determined at
  //runtime. The advantage of the other DVFS access as a member function 
  //instead of a factory is that as a member THIS class kümmert sich um
  //deletion of the dyn. allocated object.
  //IDynFreqScalingAccess * mp_dynfreqscalingaccess ;
  I_PowerProfDynLinked * mp_dynfreqscalingaccess ;
  virtual unsigned char CreatePowerSchemeWithWantedName() { return 0 ; }
  bool DisableFrequencyScalingByOS() ;
  unsigned char EnableFrequencyScalingByOS() ;
  bool OtherDVFSisEnabled() ;
  virtual unsigned char PowerSchemeToSetExists() { return 0 ; }
};
