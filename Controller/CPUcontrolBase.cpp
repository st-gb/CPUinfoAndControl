/*
 * CPUcontrolBase.cpp
 *
 *  Created on: May 1, 2010
 *      Author: Stefan
 */
#include "CPUcontrolBase.hpp"
#include <Controller/I_CPUaccess.hpp>
#include <Controller/ReadFileContent.hpp> //ReadFileContent(std::string & )
#include <UserInterface/UserInterface.hpp>
#include <Xerces/XercesHelper.hpp> //for x86InfoAndControl::InitializeXerces()

CPUcontrolBase::CPUcontrolBase()
  :
  m_p_cpucontrollerDynLib ( NULL) ,
  m_p_cpucoreusagegetterDynLib (NULL) ,
  mp_cpucontroller( NULL)
  , mp_cpucoreusagegetter( NULL)
  , m_dynlibhandler( * this)
  , mp_i_cpuaccess(NULL)
  , mp_userinterface (NULL)
{
  LOGN("CPUcontrolBase()")
  m_bXercesSuccessfullyInitialized = x86InfoAndControl::InitializeXerces() ;
  LOGN("CPUcontrolBase() end")
}

CPUcontrolBase::~CPUcontrolBase()
{
  LOGN("~CPUcontrolBase()")
  if( m_bXercesSuccessfullyInitialized )
  {
    //http://xerces.apache.org/xerces-c/program-3.html:
    //"Independent of the API you want to use, DOM, SAX, or SAX2, your
    //application must [...] and terminate it after you are done.
    //When Terminate() was called in another block (even if in a function that
    //is called in the same block) than program crash.
    XERCES_CPP_NAMESPACE::XMLPlatformUtils::Terminate();
    LOG( "Xerces access terminated"//"\n"
      ) ;
  }
  LOGN("~CPUcontrolBase() end")
}

void CPUcontrolBase::CreateDynLibCPUcontroller(
  const std::string & stdstrCPUtypeRelativeDirPath
//  , I_CPUcontroller * & r_p_cpucontroller
  )
{
  std::string stdstr = stdstrCPUtypeRelativeDirPath + "CPUcontroller.cfg" ;
  if( ReadFileContent( stdstr ) )
  {
    std::string stdstrFilePath = "CPUcontrollerDynLibs/" + stdstr ;
//    std::string stdstrFullFilePath =
    m_model.m_stdstrCPUcontrollerDynLibPath = m_dynlibhandler.
      GetDynLibPath(stdstrFilePath) ;
    LOGN("should load/ attach "
//      << stdstrFullFilePath
      << m_model.m_stdstrCPUcontrollerDynLibPath
      << " as CPU controller" )
    m_dynlibhandler.CreateDynLibCPUcontroller(
//      stdstrFullFilePath //,
      m_model.m_stdstrCPUcontrollerDynLibPath
//      mp_i_cpuaccess,
//      mp_userinterface
      ) ;
  }
}

void CPUcontrolBase::CreateDynLibCPUcoreUsageGetter(
  const std::string & stdstrCPUtypeRelativeDirPath
//  , ICPUcoreUsageGetter * & r_p_icpucoreusagegetter
  )
{
  std::string stdstr = stdstrCPUtypeRelativeDirPath + "CPUcoreUsageGetter.cfg" ;
  if( ReadFileContent( stdstr ) )
  {
    std::string stdstrFilePath = "CPUcoreUsageGetterDynLibs/" + stdstr ;
    std::string stdstrFullFilePath = m_dynlibhandler.
      GetDynLibPath(stdstrFilePath) ;
    m_model.m_stdstrCPUcoreUsageGetterDynLibPath =
      stdstrFullFilePath ;
    LOGN("should load/ attach " << stdstrFullFilePath
      << " as CPU core usage getter" )
    m_dynlibhandler.CreateDynLibCPUcoreUsageGetter(
      stdstrFullFilePath
//      ,
//      mp_i_cpuaccess,
//      mp_userinterface
      ) ;
  }
}

  void CPUcontrolBase::DeleteCPUcontroller()
  {
    LOGN("CPUcontrolBase::DeleteCPUcontroller cpu controller:"
      << mp_cpucontroller )
    if( mp_cpucontroller )
      //Release memory.
      delete mp_cpucontroller ;
    mp_cpucontroller = NULL ;
    //May be NULL at startup.
    if( mp_cpucoreusagegetter )
      mp_cpucoreusagegetter->SetCPUcontroller( NULL ) ;
    LOGN("CPUcontrolBase::DeleteCPUcontroller() end:" )
  }

  //Freeing the resources can't be placed into the destructor because if
  // wxWidgets' DynLib class is used the must not be deleted in the wxApp's
  // destructor( but in wxApp::OnExit(...) ).
  void CPUcontrolBase::FreeRessources()
  {
    LOGN("FreeRessources begin")
    //If pointer is not NULL.
    if( mp_cpucontroller )
      delete mp_cpucontroller ;
    //If pointer is not NULL.
    if( mp_cpucoreusagegetter )
      delete mp_cpucoreusagegetter ;
    //if( ! mp_dynfreqscalingthread )
    //    delete mp_dynfreqscalingthread ;
    if( //mp_winring0dynlinked
      mp_i_cpuaccess
      )
      //delete mp_winring0dynlinked ;
      delete mp_i_cpuaccess ;
    //if( mar_tch )
    //    //delete [] mar_tch ;
    //    //runtime error here for some reason. I do not understand because
    //    //the array was allocated via "new".
    //    delete mar_tch ;
  #ifdef COMPILE_WITH_MEMORY_MAPPED_FILE
    if( mp_voidMappedViewStartingAddress )
      ::UnmapViewOfFile(mp_voidMappedViewStartingAddress);
    if( m_handleMapFile != NULL )
      ::CloseHandle(m_handleMapFile);
  #endif //#ifdef COMPILE_WITH_MEMORY_MAPPED_FILE
    LOGN("FreeRessources end")
  }

  //TODO delete CPU controller _dynlib_ or CPU controller?
  void CPUcontrolBase::PossiblyDeleteCPUcontrollerDynLib()
  {
    //I_CPUcontroller * i_cpucontrollerDynLib =
      //m_dynlibhandler.GetDynLibCPUcontroller() ;
    LOGN("PossiblyDeleteCPUcontroller cpu controller:" << mp_cpucontroller
      << "dyn lib controller:" << //i_cpucontrollerDynLib
      m_p_cpucontrollerDynLib )
    if( //This may either point to a built-in CPU controller or to a dyn lib
      //CPU controller.
      mp_cpucontroller &&
      //(I_CPUcontroller * ) i_cpucontrollerDynLib ==  mp_cpucontroller
      m_p_cpucontrollerDynLib ==  mp_cpucontroller
      )
    {
      EndDVFS() ;
//        SetCPUcontroller( NULL ) ;
      DeleteCPUcontroller() ;
//        //This may either point to a built-in CPU controller or to a dyn lib
//        //CPU controller.
//        mp_cpucontroller ;

      //If the CPU controller was a dyn lib CPU controller this pointer also
      //has do be set to NULL.
      m_p_cpucontrollerDynLib = NULL ;
      if( mp_userinterface )
        //E.g. do stuff like disable "unload dyn lib CPU controller" in menu.
        mp_userinterface->CPUcontrollerDeleted() ;
    }
    LOGN("CPUcontrolBase::PossiblyDeleteCPUcontroller() end" )
  }

  //TODO delete CPU controller _dynlib_ or CPU controller?
  void CPUcontrolBase::PossiblyDeleteCPUcoreUsageGetter()
  {
//    ICPUcoreUsageGetter * i_cpucoreusagegetterDynLib =
//      m_dynlibhandler.GetDynLibCPUcoreUsageGetter() ;
    if( //i_cpucoreusagegetterDynLib
        mp_cpucoreusagegetter )
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
//      delete mp_wxdynlibcpucoreusagegetter ;
      m_p_cpucoreusagegetterDynLib = NULL ;
      //PossiblyReleaseMemForCPUcontrollerUIcontrols() ;
      //mp_model->m_cpucoredata.ClearCPUcontrollerSpecificAtts() ;
      if( mp_userinterface )
        //E.g. do stuff like disable "unload dyn lib CPU controller" in menu.
        mp_userinterface->CPUcoreUsageGetterDeleted() ;
    }
  }
