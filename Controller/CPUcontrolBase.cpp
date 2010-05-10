/*
 * CPUcontrolBase.cpp
 *
 *  Created on: May 1, 2010
 *      Author: Stefan
 */
#include "CPUcontrolBase.hpp"

  CPUcontrolBase::CPUcontrolBase( )
    :
    mp_cpucontroller( NULL)
    , mp_cpucoreusagegetter( NULL)
    , mp_userinterface (NULL)
    , mp_wxdynlibcpucontroller (NULL)
    , mp_wxdynlibcpucoreusagegetter ( NULL )
  {
  }
  void CPUcontrolBase::DeleteCPUcontroller()
  {
    DEBUGN("CPUcontrolBase::DeleteCPUcontroller cpu controller:" <<
        mp_cpucontroller )
    if( mp_cpucontroller )
      //Release memory.
      delete mp_cpucontroller ;
    mp_cpucontroller = NULL ;
    //May be NULL at startup.
    if( mp_cpucoreusagegetter )
      mp_cpucoreusagegetter->SetCPUcontroller( NULL ) ;
    DEBUGN("CPUcontrolBase::DeleteCPUcontroller() end:" )
  }

  void CPUcontrolBase::PossiblyDeleteCPUcontrollerDynLib()
  {
    DEBUG("PossiblyDeleteCPUcontroller cpu controller:" << mp_cpucontroller
        << "dyn lib controller:" << mp_wxdynlibcpucontroller )
    if( //gp_cpucontrolbase->
      //mp_wxdynlibcpucontroller
      //This may either point to a built-in CPU controller or to a dyn lib
      //CPU controller.
      mp_cpucontroller &&
      (I_CPUcontroller * ) mp_wxdynlibcpucontroller ==  mp_cpucontroller
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
        mp_userinterface->CPUcontrollerDeleted() ;
    }
    DEBUG("CPUcontrolBase::PossiblyDeleteCPUcontroller() end" )
  }

  void CPUcontrolBase::PossiblyDeleteCPUcoreUsageGetter()
  {
    if( //gp_cpucontrolbase->
        mp_wxdynlibcpucoreusagegetter )
    {
      //mp_wxx86infoandcontrolapp->SetCPUcontroller( NULL ) ;
      //mp_wxx86infoandcontrolapp->DeleteCPUcontroller() ;

      EndDVFS() ;
      //mp_wxx86infoandcontrolapp->
      LOGN("deleting existing CPU core usage getter")
      //Must delete instance of ICPUcoreUsageGetter (base class of
      //wxDynLibCPUcoreUsageGetter).
      //Else the destructor of wxDynLibCPUcoreUsageGetter was not called and
      //so the DLL was not unloaded.
      delete mp_cpucoreusagegetter ;
        mp_cpucoreusagegetter = NULL ;
//      delete //gp_cpucontrolbase->
//        mp_wxdynlibcpucoreusagegetter ;
      //gp_cpucontrolbase->
        mp_wxdynlibcpucoreusagegetter = NULL ;
      //delete mp_wxdynlibcpucontroller ;
      //mp_wxdynlibcpucontroller = NULL ;
      //PossiblyReleaseMemForCPUcontrollerUIcontrols() ;
      //mp_model->m_cpucoredata.ClearCPUcontrollerSpecificAtts() ;
      if( mp_userinterface )
        //E.g. do stuff like disable "unload dyn lib CPU controller" in menue.
        mp_userinterface->CPUcoreUsageGetterDeleted() ;
    }
  }
