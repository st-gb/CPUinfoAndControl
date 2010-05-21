/*
 * CPUcontrolBase.hpp
 *
 *  Created on: Mar 23, 2010
 *      Author: Stefan
 */

#ifndef CPUCONTROLBASE_HPP_
#define CPUCONTROLBASE_HPP_

#include <UserInterface.hpp>
#include <Controller/I_CPUcontroller.hpp>
#include <Controller/ICPUcoreUsageGetter.hpp>
#include "global.h" //LOGN()

class CPUcoreData ;
class I_CPUaccess ;
class I_CPUcontroller ;
class ICPUcoreUsageGetter ;
//class UserInterface ;
class wxDynLibCPUcontroller ;
class wxDynLibCPUcoreUsageGetter ;

//#define NULL 0

//Base class for the service and the GUI.
class CPUcontrolBase
{
protected:
  I_CPUaccess * mp_i_cpuaccess ;
  UserInterface * mp_userinterface ;
public:
  I_CPUcontroller * mp_cpucontroller ;
  ICPUcoreUsageGetter * mp_cpucoreusagegetter ;
  wxDynLibCPUcontroller * mp_wxdynlibcpucontroller ;
  wxDynLibCPUcoreUsageGetter * mp_wxdynlibcpucoreusagegetter ;
  CPUcontrolBase( ) ;

//  //dtor must exist, else g++: "undefined reference to `vtable for
//  //CPUcontrolBase'"?
//  //virtual because else g++ error:
//  // `class CPUcontrolBase' has virtual functions but non-virtual destructor
//  virtual ~CPUcontrolBase( ) {}
  virtual void DeleteCPUcontroller() ;

  virtual void EndDVFS() {}
  //Declare here because e.g. either a service or a GUI may delete a CPU
  //controller.
  //"Possibly" because: if the controller is NULL it is not being deleted.
  void PossiblyDeleteCPUcontrollerDynLib() ;

  //Declare here because e.g. either a service or a GUI may delete a CPU core
  //usage getter.
  void PossiblyDeleteCPUcoreUsageGetter() ;

  I_CPUaccess * GetCPUaccess() { return mp_i_cpuaccess ; }
  virtual void SetCPUcontroller( I_CPUcontroller * p_cpucontrollerNew ) {}
} ;

#endif /* CPUCONTROLBASE_HPP_ */
