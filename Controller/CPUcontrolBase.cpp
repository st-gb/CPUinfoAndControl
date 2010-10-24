/*
 * CPUcontrolBase.cpp
 *
 *  Created on: May 1, 2010
 *      Author: Stefan
 */
#include "CPUcontrolBase.hpp"
#include <Controller/character_string/stdtstr.hpp> //GetStdString(...)
#include <Controller/I_CPUaccess.hpp>
//class DynFreqScalingThreadBase
#include <Controller/DynFreqScalingThreadBase.hpp>
#include <Controller/IDynFreqScalingAccess.hpp> //class IDynFreqScalingAccess
#ifdef _WIN32
  //class PowerProfDynLinked
  #include <Windows/PowerProfAccess/PowerProfDynLinked.hpp>
#endif
#include <Controller/MainController.hpp>//MainController::GetSupportedCPUs(...)
#include <InputOutput/ReadFileContent.hpp> //ReadFileContent(std::string & )
#include <ModelData/PerCPUcoreAttributes.hpp> //class PerCPUcoreAttributes
#include <UserInterface/UserInterface.hpp>
#include <Xerces/XercesHelper.hpp> //for x86InfoAndControl::InitializeXerces()

#include <iostream> //std::cout

//CPUcontrolBase::CPUcontrolBase()
//  :
//  m_dynlibhandler ( * this ) ,
//  mp_userinterface ( NULL )
//{
//  LOGN("CPUcontrolBase()")
//  InitMemberVariables() ;
//  m_bXercesSuccessfullyInitialized = x86InfoAndControl::InitializeXerces() ;
//  LOGN("CPUcontrolBase() end")
//}

CPUcontrolBase::CPUcontrolBase(const UserInterface * const cpc_userinterface )
  :
  m_dynlibhandler ( * this ) ,
  mp_userinterface ( //Avoid Linux g++ error "invalid conversion from ‘const
    //UserInterface*’ to ‘UserInterface*’
    (UserInterface *)
    cpc_userinterface )
{
  LOGN("CPUcontrolBase()")
  InitMemberVariables() ;
  m_bXercesSuccessfullyInitialized = x86InfoAndControl::InitializeXerces() ;
  LOGN("CPUcontrolBase() end")
}

CPUcontrolBase::~CPUcontrolBase()
{
  LOGN("~CPUcontrolBase() begin")
  FreeRessources() ;
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
  std::string stdstrFilePath = stdstrCPUtypeRelativeDirPath +
    "CPUcontroller.cfg" ;
  std::string stdstr = stdstrFilePath ;
  if( ReadFileContent( stdstr ) )
  {
    if( stdstr.empty() )
      LOGN("Do not load/ attach CPU core usage getter because string read "
        "from file \"" << stdstrFilePath
        << " that should contain the dynamic libary name seems to be empty" )
    else
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
}

void CPUcontrolBase::CreateDynLibCPUcoreUsageGetter(
  const std::string & stdstrCPUtypeRelativeDirPath
//  , ICPUcoreUsageGetter * & r_p_icpucoreusagegetter
  )
{
  std::string stdstrFilePath = stdstrCPUtypeRelativeDirPath +
    "CPUcoreUsageGetter.cfg" ;
  std::string stdstr = stdstrFilePath ;
  if( ReadFileContent( stdstr ) )
  {
    if( stdstr.empty() )
      LOGN("Do not load/ attach CPU core usage getter because string read from "
        "file " << stdstrFilePath
        << " that should contain the dynamic libary name seems to be empty" )
    else
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

  void CPUcontrolBase::EndDVFS()
  {
    LOGN("CPUcontrolBase::EndDVFS begin")
    mp_dynfreqscalingthreadbase->m_vbRun = false ;
    LOGN("CPUcontrolBase::EndDVFS end")
  }

  //Freeing the resources can't be placed into the destructor because if
  // wxWidgets' DynLib class is used the must not be deleted in the wxApp's
  // destructor( but in wxApp::OnExit(...) ).
  void CPUcontrolBase::FreeRessources()
  {
    LOGN("FreeRessources begin")
    //If pointer is not NULL.
    if( mp_cpucontroller )
    {
      LOGN("before deleting the CPU controller")
      delete mp_cpucontroller ;
    }
    //If pointer is not NULL.
    if( mp_cpucoreusagegetter )
    {
      LOGN("before deleting the CPU core usage getter")
      //TODO problem under _Linux_: when unloading the dyn lib the global
      // variables ( "g_stdstrLog" etc.) that
      // are _automatically shared between the exe and DLL) are destroyed.->Program
      // crash when accessing them.
      delete mp_cpucoreusagegetter ;
    }
    //if( ! mp_dynfreqscalingthread )
    //    delete mp_dynfreqscalingthread ;
    if( //mp_winring0dynlinked
      mp_i_cpuaccess
      )
    {
      LOGN("before deleting the CPU access object")
      delete mp_i_cpuaccess ;
    }
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

//Sourcecode that is the same for _all_ constructors.
void CPUcontrolBase::InitMemberVariables()
{
  mp_dynfreqscalingthreadbase = NULL ;
  m_p_cpucontrollerDynLib = NULL;
  m_p_cpucoreusagegetterDynLib = NULL ;
  mp_cpucontroller = NULL ;
  mp_cpucoreusagegetter = NULL ;
  mp_i_cpuaccess = NULL ;
#ifdef COMPILE_WITH_OTHER_DVFS_ACCESS
//  #ifdef _WIN32
//    mp_dynfreqscalingaccess = new PowerProfDynLinked( ) ;
//  #else
    mp_dynfreqscalingaccess = NULL ;
//  #endif
#endif //#ifdef COMPILE_WITH_OTHER_DVFS_ACCESS
//  mp_userinterface = NULL ;
}

void CPUcontrolBase::OuputCredits()
{
  std::tstring stdtstr ;
  std::string stdstr ;
  std::vector<std::tstring> stdvecstdtstring ;
  MainController::GetSupportedCPUs(stdvecstdtstring) ;
  stdstr = GetStdString( stdtstr ) ;
  for(BYTE by = 0 ; by < stdvecstdtstring.size() ; by ++ )
    stdtstr += _T("-") + stdvecstdtstring.at(by) + _T("\n") ;
  WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE(
    //"This program is an undervolting program for AMD family 17 CPUs.\n"
    "This program is a CPU information and control program for (built-in):\n"
//    + stdtstr +
    + stdstr +
    //"license/ info: http://amd.goexchange.de / http://sw.goexchange.de\n"
    "license/ info: http://www.trilobyte-se.de/x86iandc/\n"
    )
  std::cout <<
    "This executable is both in one:\n"
    "-a(n) (de-)installer for the CPU controlling service\n"
    "-the CPU controlling service itself\n" ;
  WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE(
    "Build time: " __DATE__ " " __TIME__ " (Greenwich Mean Time + 1)" );
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

void CPUcontrolBase::StartDynamicVoltageAndFrequencyScaling()
{
  LOGN("CPUcontrolBase::StartDynamicVoltageAndFrequencyScaling() begin")
  if( m_model.m_cpucoredata.m_stdsetvoltageandfreqWanted.empty() )
  {
    mp_userinterface->Confirm( "No wanted voltages for frequencies available->"
      "Dynamic Voltage And Frequency Scaling not possible"
      "\nYou may use the frequency and voltage settings dialog set at "
      "least 2 voltages for 2 different CPU core multipliers/ frequencies"
      ) ;
    return ;
  }
  if( m_model.m_cpucoredata.m_stdsetvoltageandfreqWanted.size() == 1 )
  {
    mp_userinterface->Confirm( "Only 1 wanted voltage for a frequency available->"
      "Dynamic Voltage And Frequency Scaling not possible"
      "\nYou may use the frequency and voltage settings dialog set at "
      "least 2 voltages for 2 different CPU core multipliers/ frequencies"
      ) ;
    return ;
  }
  //May be NULL at startup.
  if( mp_cpucontroller &&
    //May be NULL at startup.
    mp_cpucoreusagegetter
    )
  {
//    WORD wEventID = wxevent.GetId() ;
//    #ifdef COMPILE_WITH_NAMED_WINDOWS_PIPE
//    //TODO
//    if( ::wxGetApp().m_ipcclient.IsConnected() )
//    {
//      ::wxGetApp().m_ipcclient.SendCommandAndGetResponse(start_DVFS) ;
//    }
//    else
//    #endif //#ifdef COMPILE_WITH_NAMED_WINDOWS_PIPE
    {
      //SpecificCPUcoreActionAttributes * p_atts = (CalculationThread *)
      //  m_stdmapwxuicontroldata.find( wEventID )->second ;
      PerCPUcoreAttributes * p_percpucoreattributes = & m_model.m_cpucoredata.
        m_arp_percpucoreattributes[ //p_atts->m_byCoreID
        0 ] ;
      //DynFreqScalingThread * p_dynfreqscalingthread
      if ( p_percpucoreattributes->mp_dynfreqscalingthread )
      {
//        EndDynVoltAndFreqScalingThread(p_percpucoreattributes) ;
        EndDVFS() ;
      }
      else
      {
        PossiblyAskForOSdynFreqScalingDisabling() ;
#ifdef COMPILE_WITH_OTHER_DVFS_ACCESS
        if( !
            //mp_i_cpucontroller->mp_dynfreqscalingaccess->OtherDVFSisEnabled()
            //mp_i_cpucontroller->OtherPerfCtrlMSRwriteIsActive()
            mp_dynfreqscalingaccess->OtherDVFSisEnabled()
          )
#endif //#ifdef COMPILE_WITH_OTHER_DVFS_ACCESS
        {
          //TODO crashes after/ at closing the DVFS dialog
//          //TODO is the memory released?
//          DynFreqScalingDlg * p_dynfreqscalingdlg = new DynFreqScalingDlg(
//            this
//            , *mp_cpucoredata
//            ) ;
//          if( p_dynfreqscalingdlg )
////              if(
//              p_dynfreqscalingdlg->
//              //Showing it "modal" is important because else the instructions below
//              //continue before the dialog ic closed.
//              ShowModal()
//              ;
////                == //wxID_OK
////                    wxID_APPLY
////                )
//            {
//              LOGN("closed the DVFS dialog with OK or apply")
//              //p_percpucoreattributes->mp_dynfreqscalingthread
//              //TODO uncomment
                p_percpucoreattributes->SetCPUcontroller( //mp_i_cpucontroller
                  mp_cpucontroller ) ;
                p_percpucoreattributes->CreateDynFreqScalingThread(
                  mp_cpucoreusagegetter
//                  gp_cpucontrolbase->mp_wxdynlibcpucoreusagegetter
                  ) ;
                mp_userinterface->DynVoltnFreqScalingEnabled() ;
//            }
        }
      }
    }
  }
  else
  {
    mp_userinterface->Confirm(
      "no CPU controller and/ or CPU usage getter\n"
      "->no usage based scaling possible. "
      "Maybe attach a CPU controller and/ or "
      "usage getter dynamic library (menu \"File\") "
      ) ;
  }
  LOGN("CPUcontrolBase::StartDynamicVoltageAndFrequencyScaling() end")
}
