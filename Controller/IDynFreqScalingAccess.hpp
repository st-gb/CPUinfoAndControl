#pragma once

#include <Controller/stdtstr.hpp>
#ifdef __CYGWIN__
#include <mingw/tchar.h> //for _T()
#else //MS compiler, MinGW
#include <tchar.h> //for _T()
#endif
#define ALREADY_ENABLED

//TODO rename this class to "I_OtherVoltageRespectivelyFreqChange"
//because in general this should mean that any unwanted change in voltage
//or frequency should be able to disable.
//For instance change 
//-by the OS 
//-or tools like RMclock 
//-other programs that might write into MSR performance state select CPU 
// register(s)
class IDynFreqScalingAccess
{
public:
  //Using an enum rather than #defines ensures that there are no strings 
  //with the same value.
  enum EnableRetCodes { enable_success, enable_failure, already_enabled } ;
  //Possilby typedef "string_type" to "std::string" if compiling with MinGW.
  typedef std::tstring string_type ;
    //return: true=success
  virtual bool ChangeOtherDVFSaccessPossible() { return false ; }
  virtual bool DisableFrequencyScalingByOS() = 0;
  //Return value: 
  //-already enabled
  //-
  virtual unsigned char EnableFrequencyScalingByOS() = 0;
  virtual bool EnablingIsPossible() { return false ; }
  virtual string_type GetEnableDescription() {
    return std::tstring(_T("") ) ;
  }
  //"Other DVFS" because the implementaion may disable e.g. the OS's
  //DVFS or GNOME's DVFS, RMClock etc.
  virtual bool OtherDVFSisEnabled() 
  { 
    return false ;
  }
};
