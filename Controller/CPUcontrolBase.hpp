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

#define NULL 0

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
  CPUcontrolBase( )
    : 
    mp_cpucontroller( NULL)
    , mp_cpucoreusagegetter( NULL)
    , mp_userinterface (NULL)
    , mp_wxdynlibcpucontroller (NULL)
    , mp_wxdynlibcpucoreusagegetter ( NULL )
  {
  }
  //dtor must exist, else g++: "undefined reference to `vtable for
  //CPUcontrolBase'"?
  ~CPUcontrolBase( ) {}
  virtual void DeleteCPUcontroller()
  {
    if( mp_cpucontroller )
      //Release memory.
      delete mp_cpucontroller ;
    mp_cpucontroller = NULL ;
    //May be NULL at startup.
    if( mp_cpucoreusagegetter )
      mp_cpucoreusagegetter->SetCPUcontroller( NULL ) ;
  }
  virtual void EndDVFS() {}
  //Declare here because e.g. either a service or a GUI may delete a CPU core
  //usage getter.a
  void PossiblyDeleteCPUcontroller()
  {
    if( //gp_cpucontrolbase->
      //mp_wxdynlibcpucontroller
      //This may either point to a built-in CPU controller or to a dyn lib
      //CPU controller.
      mp_cpucontroller
      )
    {
      EndDVFS() ;
      //mp_wxx86infoandcontrolapp->
//        SetCPUcontroller( NULL ) ;
      //mp_wxx86infoandcontrolapp->
        DeleteCPUcontroller() ;
//      delete //mp_wxx86infoandcontrolapp->
//        //mp_wxdynlibcpucontroller ;
//        //This may either point to a built-in CPU controller or to a dyn lib
//        //CPU controller.
//        mp_cpucontroller ;
      //delete mp_wxdynlibcpucontroller ;

      //If the CPU controller was a dyn lib CPU controller this pointer also
      //has do be set to NULL.
      //gp_cpucontrolbase->
        mp_wxdynlibcpucontroller = NULL ;
      if( mp_userinterface )
        //E.g. do stuff like disable "unload dyn lib CPU controller" in menue.
        mp_userinterface->CPUcoreUsageGetterDeleted() ;
    }
  }
  //Declare here because e.g. either a service or a GUI may delete a CPU core
  //usage getter.
  void PossiblyDeleteCPUcoreUsageGetter()
  {
    if( //gp_cpucontrolbase->
        mp_wxdynlibcpucoreusagegetter )
    {
      //mp_wxx86infoandcontrolapp->SetCPUcontroller( NULL ) ;
      //mp_wxx86infoandcontrolapp->DeleteCPUcontroller() ;

      EndDVFS() ;
      //mp_wxx86infoandcontrolapp->
        mp_cpucoreusagegetter = NULL ;
      LOGN("deleting existing CPU core usage getter")
      delete //gp_cpucontrolbase->
        mp_wxdynlibcpucoreusagegetter ;
      //gp_cpucontrolbase->
        mp_wxdynlibcpucoreusagegetter = NULL ;
      //delete mp_wxdynlibcpucontroller ;
      //mp_wxdynlibcpucontroller = NULL ;
      //PossiblyReleaseMemory() ;
      //mp_model->m_cpucoredata.ClearCPUcontrollerSpecificAtts() ;
    }
  }
  I_CPUaccess * GetCPUaccess() { return mp_i_cpuaccess ; }
  virtual void SetCPUcontroller( I_CPUcontroller * p_cpucontrollerNew ) {}
} ;


#endif /* CPUCONTROLBASE_HPP_ */
