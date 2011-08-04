/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
/*
 * CPUcontrolBase.cpp
 *
 *  Created on: May 1, 2010
 *      Author: Stefan
 */
#include "CPUcontrolBase.hpp"
#include <Controller/character_string/stdtstr.hpp> //GetStdString(...)
#include <Controller/GetNumberOfLogicalCPUcores.h>
#include <Controller/I_CPUaccess.hpp>
//class DynFreqScalingThreadBase
#include <Controller/DynFreqScalingThreadBase.hpp>
#include <Controller/IDynFreqScalingAccess.hpp> //class IDynFreqScalingAccess
#ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
  //class PowerProfDynLinked
  #include <Windows/PowerProfAccess/PowerProfDynLinked.hpp>
  #include <Windows/WinRing0/WinRing0_1_3RunTimeDynLinked.hpp>
#else
  #include <Linux/MSRdeviceFile.hpp>
#endif
#include <Controller/MainController.hpp>//MainController::GetSupportedCPUs(...)
//ReadFileContent(std::string & )
#include <InputOutput/ReadFileContent/ReadFileContent.hpp>
#include <ModelData/PerCPUcoreAttributes.hpp> //class PerCPUcoreAttributes
#include <preprocessor_macros/logging_preprocessor_macros.h> //LOGN(...)
#include <UserInterface/UserInterface.hpp>
#include <Xerces/XercesHelper.hpp> //for x86InfoAndControl::InitializeXerces()

#include <iostream> //std::cout

//Needed for the exported functions.
I_CPUaccess * g_p_cpuaccess = NULL;
UserInterface * g_p_user_interface = NULL;

//static
I_CPUaccess * CPUcontrolBase::s_p_hardware_access = NULL;
//static
UserInterface * CPUcontrolBase::s_p_userinterface = NULL;

void RemoveCarriageReturn(std::string & r_stdstr )
{
  std::string::size_type stdstrsizetype = r_stdstr.find( 0x0A ) ;
  while( stdstrsizetype != std::string::npos )
  {
    r_stdstr.erase(stdstrsizetype,1) ;
    stdstrsizetype = r_stdstr.find(0x0A) ;
  }
}

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
  m_maincontroller( //Avoid Linux g++ error "invalid conversion from ‘const
    //UserInterface*’ to ‘UserInterface*’
    (UserInterface *) cpc_userinterface) ,
  mp_userinterface ( //Avoid Linux g++ error "invalid conversion from ‘const
    //UserInterface*’ to ‘UserInterface*’
    (UserInterface *)
    cpc_userinterface )
{
  LOGN(//"CPUcontrolBase()"
    FULL_FUNC_NAME << "--begin")
  g_p_user_interface = (UserInterface *) cpc_userinterface;
  InitMemberVariables() ;
  m_bXercesSuccessfullyInitialized = x86InfoAndControl::InitializeXerces() ;
  LOGN(//"CPUcontrolBase() end"
    FULL_FUNC_NAME << "--end")
}

CPUcontrolBase::~CPUcontrolBase()
{
  LOGN("~CPUcontrolBase() begin")
  FreeRessources() ;
  if( m_bXercesSuccessfullyInitialized )
  {
    x86InfoAndControl::TerminateXerces();
  }
  LOGN("~CPUcontrolBase() end")
}

void CPUcontrolBase::CreateDynLibCPUcontroller(
  const std::string & stdstrCPUtypeRelativeDirPath
//  , I_CPUcontroller * & r_p_cpucontroller
  )
{
  std::string stdstrFilePath ;
  stdstrFilePath = GetCPUcontrollerConfigFilePath(
    stdstrCPUtypeRelativeDirPath ) ;
  std::string stdstr = stdstrFilePath ;
  if( ReadFileContent( stdstr ) )
  {
    if( stdstr.empty() )
      LOGN("Do not load/ attach CPU core usage getter because string read "
        "from file \"" << stdstrFilePath
        << " that should contain the dynamic libary name seems to be empty" )
    else
    {
      //Linux text editors like "gedit" automatically add a carriage return
      //character at the end of the (last) line.
      RemoveCarriageReturn(stdstr) ;
      std::string stdstrFilePath = "CPUcontrollerDynLibs/" + stdstr ;
  //    std::string stdstrFullFilePath =
      m_model.m_stdstrCPUcontrollerDynLibPath = m_dynlibhandler.
        GetDynLibPath(stdstrFilePath) ;
      LOGN("should load/ attach \""
  //      << stdstrFullFilePath
        << m_model.m_stdstrCPUcontrollerDynLibPath
        << "\" as CPU controller" )
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
      //Linux text editors like "gedit" automatically add a carriage return
      //character at the end of the (last) line.
      RemoveCarriageReturn(stdstr) ;
      std::string stdstrFilePath = "CPUcoreUsageGetterDynLibs/" + stdstr ;
      std::string stdstrFullFilePath = m_dynlibhandler.
        GetDynLibPath(stdstrFilePath) ;
      m_model.m_stdstrCPUcoreUsageGetterDynLibPath =
        stdstrFullFilePath ;
      LOGN("should load/ attach \"" << stdstrFullFilePath
        << "\" as CPU core usage getter" )
      m_dynlibhandler.CreateDynLibCPUcoreUsageGetter(
        stdstrFullFilePath
  //      ,
  //      mp_i_cpuaccess,
  //      mp_userinterface
        ) ;
    }
  }
}

void CPUcontrolBase::CreateHardwareAccessObject()
{
  try //catch CPUaccessexception
  {
#ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
  //WinRing0dynLinked winring0dynlinked(p_frame) ;
  //If allocated statically within this block / method the object
  //gets invalid after leaving the block where it was declared.
  //mp_winring0dynlinked
  //mp_i_cpuaccess = new WinRing0dynLinked(//p_frame
#ifdef _MSC_VER_ //possible because the import library is for MSVC
  mp_i_cpuaccess = new WinRing0_1_3LoadTimeDynLinked(
    this ) ;
#else //Use runtime dynamic linking because no import library is available for
  //MinGW.
  mp_i_cpuaccess = new WinRing0_1_3RunTimeDynLinked(
//    this
    mp_userinterface
    ) ;
#endif
  //m_maincontroller.SetCPUaccess( //mp_winring0dynlinked
  //  mp_i_cpuaccess ) ;
#else
    //m_maincontroller.SetCPUaccess(NULL) ;
    //m_MSRdeviceFile.SetUserInterface(this) ;
    mp_i_cpuaccess = new MSRdeviceFile(//this,
      mp_userinterface ,
      GetNumberOfLogicalCPUcores() ,
      m_model
      ) ;
    //m_maincontroller.SetCPUaccess(&m_MSRdeviceFile) ;
  #endif
    //Assign to the global variable so that the functions (ReadMSR(...) etc.)
    //that are exported by this executable can access the CPU registers.
    g_p_cpuaccess = mp_i_cpuaccess ;
    s_p_hardware_access = mp_i_cpuaccess;
    //the main controller needs CPUID (I_CPUaccess class ) access in order to
    //retrieve the CPU by model, family etc.
    m_maincontroller.SetCPUaccess( mp_i_cpuaccess );
    mp_i_cpuaccess->mp_model = //mp_modelData ;
      & m_model ;
  }
  catch(//ReadMSRexception
      CPUaccessException & r_cpuaccessexception )
  {
    LOGN("caught a CPUaccessException:"
      << r_cpuaccessexception.m_stdstrErrorMessage )
    LOGN("mp_i_cpuaccess:" << mp_i_cpuaccess)
    g_p_cpuaccess = NULL;
    //Important: show the message to the user so that he knows that there is
    //a problem.
    mp_userinterface->Confirm(r_cpuaccessexception.m_stdstrErrorMessage);
    //We may continue to use this program: e.g. for testing usage getter
    //DLLs or for showing the usage etc. via IPC.
    //If the construction of a I_CPUaccess object failed the pointer should
    //already be NULL.
//        mp_i_cpuaccess = NULL ;
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

  std::string CPUcontrolBase::GetCPUcontrollerConfigFilePath(
    const std::string & stdstrCPUtypeRelativeDirPath
     )
  {
    std::string stdstrFilePath = stdstrCPUtypeRelativeDirPath +
      "CPUcontroller.cfg" ;
    return stdstrFilePath ;
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
  std::string stdstrSupportedCPUs ;
  std::vector<std::tstring> stdvec_stdtstrSupportedCPUs ;
  MainController::GetSupportedCPUs(stdvec_stdtstrSupportedCPUs) ;
  stdstrSupportedCPUs = GetStdString( stdtstr ) ;
  for(BYTE by = 0 ; by < stdvec_stdtstrSupportedCPUs.size() ; by ++ )
    stdtstr += _T("-") + stdvec_stdtstrSupportedCPUs.at(by) + _T("\n") ;
  if( stdstrSupportedCPUs.empty() )
    WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE(
      "This program is a CPU information and control program.\n"
      //"license/ info: http://amd.goexchange.de / http://sw.goexchange.de\n"
      "license/ info: http://www.trilobyte-se.de/x86iandc/\n"
      )
  else
    WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE(
      "This program is a CPU information and control program for (built-in):\n"
  //    + stdtstr +
      + stdstrSupportedCPUs +
      //"license/ info: http://amd.goexchange.de / http://sw.goexchange.de\n"
      "license/ info: http://www.trilobyte-se.de/x86iandc/\n"
      )
  std::cout <<
    "This executable is both in one:\n"
    "-a console application for executing actions for the service\n"
    "-the CPU controlling/ information providing service itself\n" ;
  WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE(
    "Build time: " __DATE__ " " __TIME__ " (Greenwich Mean Time + 1)" );
}


//TODO delete CPU controller _dynlib_ or CPU controller?
void CPUcontrolBase::PossiblyDeleteCPUcontrollerDynLib()
{
  //I_CPUcontroller * i_cpucontrollerDynLib =
    //m_dynlibhandler.GetDynLibCPUcontroller() ;
  LOGN( //"PossiblyDeleteCPUcontroller"
      FULL_FUNC_NAME << " begin--cpu controller:" << mp_cpucontroller
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
  LOGN(//"CPUcontrolBase::PossiblyDeleteCPUcontroller()
      FULL_FUNC_NAME << "end" )
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

//see http://stackoverflow.com/questions/2094427/dll-export-as-a-c-c-function
#ifdef  __cplusplus
extern "C"
{
#endif //#ifdef __cplusplus

//see http://en.wikipedia.org/wiki/PCI_configuration_space:
//static
  CPU_CONTROL_BASE_CLASS_FUNCTION_STORAGE_CLASS
  BOOL CPUcontrolBase::ReadPCIconfigSpace(
  BYTE byPCIbus , //"8-bit PCI bus",
  BYTE byDeviceAndFunction ,//"5-bit device, and 3-bit function"
  DWORD dwRegisterAddress ,
  PDWORD p_dwValue
  )
{
//    LOGN("ReadPCIconfigSpace")
//    BOOL boolRet = FALSE ;
  //http://en.wikipedia.org/wiki/Static_variable:
  //"Static local variables: variables declared as static inside a function
  // are statically allocated while having the same scope as automatic local
  // variables. Hence whatever values the function puts into its static
  //local variables during one call will still be present when the function
  //is called again."
  //By using static: not created on stack (->faster)
  static BOOL boolRet ;
  boolRet = FALSE ;
//    MessageBox(NULL,"exe::ReadMSR","From Exe",MB_OK);
//    I_CPUaccess * p_cpuaccess = //wxGetApp().
//      gp_cpucontrolbase->
//      GetCPUaccess() ;
  //May be NULL if CPUaccess' init failed
  if( //p_cpuaccess
      //g_p_cpuaccess
      //mp_i_cpuaccess
      s_p_hardware_access
      )
  {
    boolRet = //p_cpuaccess->RdmsrEx(
      g_p_cpuaccess->ReadPciConfigDwordEx(
        ( byPCIbus << 8 ) | byDeviceAndFunction ,
        dwRegisterAddress,
        p_dwValue
        ) ;
  }
  else
    s_p_userinterface->MessageWithTimeStamp( L"the hardware /CPU access is "
      "not initalized yet.");
//    mp_userinterface->MessageWithTimeStamp("the hardware /CPU access is not "
//      "initalized yet.");
  #ifdef _DEBUG
  //if( dwIndex == 0x1AD )
//      DEBUG_COUT( "exe::ReadMSR(Index,affinityMask): "
//        << dwIndex << " "
//        << *p_dweax << " "
//        << *p_dwedx << " "
//        << affinityMask
//        << "\n" )
  #endif
  return boolRet ;
}

//EXPORT
//static
  CPU_CONTROL_BASE_CLASS_FUNCTION_STORAGE_CLASS
  BOOL CPUcontrolBase::ReadMSR(
  DWORD dwIndex,    // MSR index
  PDWORD p_dweax,     // bit  0-31
  PDWORD p_dwedx,     // bit 32-63
  DWORD_PTR affinityMask  // Thread Affinity Mask
  )
{
//    LOGN("ReadMSR")
//    BOOL boolRet = FALSE ;
  //http://en.wikipedia.org/wiki/Static_variable:
  //"Static local variables: variables declared as static inside a function
  // are statically allocated while having the same scope as automatic local
  // variables. Hence whatever values the function puts into its static
  //local variables during one call will still be present when the function
  //is called again."
  //By using static: not created on stack (->faster)
  static BOOL boolRet ;
  boolRet = FALSE ;
//    MessageBox(NULL,"exe::ReadMSR","From Exe",MB_OK);
//    I_CPUaccess * p_cpuaccess = //wxGetApp().
//      gp_cpucontrolbase->
//      GetCPUaccess() ;
  DEBUGN("Exe's exported ReadMSR("
    << "MSR index:" << dwIndex
    << " " << p_dweax
    << " " << p_dwedx
    << " Aff mask:" << affinityMask
    << ")"
    << " g_p_cpuaccess:" << g_p_cpuaccess
    )
  //May be NULL if CPUaccess' init failed
  if( //p_cpuaccess
    g_p_cpuaccess )
  {
    boolRet = //p_cpuaccess->RdmsrEx(
      g_p_cpuaccess->RdmsrEx(
      dwIndex,
      p_dweax,// bit  0-31 (register "EAX")
      p_dwedx,
      //m_dwAffinityMask
      affinityMask
      ) ;
    DEBUGN( "exe::ReadMSR after g_p_cpuaccess->RdmsrEx(Index,affinityMask): "
      << dwIndex << " "
      << * p_dweax << " "
      << * p_dwedx << " "
      << affinityMask
      //<< "\n"
      )
  }
  #ifdef _DEBUG
  //if( dwIndex == 0x1AD )
  #endif
  return boolRet ;
}

//static
  CPU_CONTROL_BASE_CLASS_FUNCTION_STORAGE_CLASS
  BOOL CPUcontrolBase::WriteMSR(
  DWORD dwIndex,    // MSR index
  DWORD dwEAX,     // bit  0-31
  DWORD dwEDX,     // bit 32-63
  DWORD_PTR affinityMask  // Thread Affinity Mask
  )
{
//    BOOL boolRet = FALSE ;
  //http://en.wikipedia.org/wiki/Static_variable:
  //"Static local variables: variables declared as static inside a function
  // are statically allocated while having the same scope as automatic local
  // variables. Hence whatever values the function puts into its static
  //local variables during one call will still be present when the function
  //is called again."
  //By using static: not created on stack (->faster)
  static BOOL boolRet ;
  boolRet = FALSE ;
//    LOGN("WriteMSR")
  DEBUGN("::WriteMSR(" << dwIndex
    << "," << dwEAX
    << "," << dwEDX
    << "," << affinityMask
    << ")")
//    MessageBox(NULL,"exe::ReadMSR","From Exe",MB_OK);
//    I_CPUaccess * p_cpuaccess = //wxGetApp().
//      gp_cpucontrolbase->
//      GetCPUaccess() ;
  DEBUGN("::WriteMSR(...)--CPUaccess pointer:" << //p_cpuaccess
    g_p_cpuaccess
//    s_p_cpuaccess
    )
  //May be NULL if CPUaccess' init failed
  if( //p_cpuaccess
      g_p_cpuaccess )
  {
    boolRet = //p_cpuaccess->WrmsrEx(
      g_p_cpuaccess->WrmsrEx(
      dwIndex,
      dwEAX,// bit  0-31 (register "EAX")
      dwEDX,
      //m_dwAffinityMask
      affinityMask
      ) ;
  }
  else
    s_p_userinterface->MessageWithTimeStamp(L"the hardware /CPU access is not "
      "initalized yet.");
//    mp_userinterface->MessageWithTimeStamp("the hardware /CPU access is not "
//      "initalized yet.");
  DEBUGN("::WriteMSR(...) after p_cpuaccess->WrmsrEx(...)")
  #ifdef _DEBUG
  //if( dwIndex == 0x1AD )
//      DEBUG_COUT( "exe::WriteMSR(Index,affinityMask): "
//        << dwIndex << " "
//        << dwEAX << " "
//        << dwEDX << " "
//        << affinityMask
//        << "\n" )
  #endif
  return boolRet ;
}

#ifdef __cplusplus
}
#endif //#ifdef __cplusplus

void CPUcontrolBase::SetDynVoltnFreqScalingType_Inline()
{
  LOGN("SetDynVoltnFreqScalingType_Inline begin")
  m_byVoltageAndFrequencyScalingType = CPUcontrolBase::none ;
  if( ! mp_cpucontroller )
  {
    if( ! mp_cpucoreusagegetter )
    {
      LOGN("No CPU controller and no usage getter->should exit.");
//      return 1 ;
    }
    else
    {
      LOGN("No CPU controller->should exit.");
    }
    //Without getting the CPU usage no CPU _load_ based scaling is
    //possible.
    m_byVoltageAndFrequencyScalingType = CPUcontrolBase::none ;
  }
  else
  {
    if( mp_cpucoreusagegetter )
    {
      m_byVoltageAndFrequencyScalingType = CPUcontrolBase::LoadBased ;
      LOGN("CPU core usage getter AND CPU controller->at this point: use CPU "
        "core load based Dynamic Voltage and Frequency Scaling");
    }
    else
    {
      LOGN("No CPU core usage getter->only temperature based Dynamic Voltage "
        "and Frequency Scaling");
  //        return 1 ;
      //Without getting the CPU usage no CPU _load_ based scaling is
      //possible.
      m_byVoltageAndFrequencyScalingType = CPUcontrolBase::TemperatureBased ;
    }
  }
  if( m_byVoltageAndFrequencyScalingType == CPUcontrolBase::LoadBased )
  {
    if( m_model.m_cpucoredata.m_stdsetvoltageandfreqDefault.size() == 0
      //mp_stdsetvoltageandfreqDefault->size() == 0
      )
    {
      LOGN("No default voltages specified->no overvoltage protection"
        "->only temperature based Dynamic Voltage and Frequency Scaling"
        );
    //        return CPUcontrolBase::no_default_voltages_specified ;
//      dwReturnValue = CPUcontrolBase::no_default_voltages_specified ;
      m_byVoltageAndFrequencyScalingType = CPUcontrolBase::TemperatureBased ;
    }
    if( m_model.m_cpucoredata.m_stdsetvoltageandfreqWanted.size() == 0
      //mp_stdsetvoltageandfreqWanted->size() == 0
      )
    {
      LOGN("No preferred voltages specified.");
      if( m_model.m_cpucoredata.m_stdsetvoltageandfreqDefault//.empty()
          .size() < 2
        )
      {
        LOGN("# of default voltages < 2"
          "->only temperature based Dynamic Voltage and Frequency Scaling"
          );
    //        return CPUcontrolBase::no_preferred_voltages_specified ;
//      dwReturnValue = CPUcontrolBase::no_preferred_voltages_specified ;
        m_byVoltageAndFrequencyScalingType = CPUcontrolBase::TemperatureBased ;
      }
      else
      {
        LOGN("Using default voltages as preferred voltages.");
        m_model.m_cpucoredata.CopyDefaultVoltageToPreferredVoltages();
      }
    }
    if( m_model.m_cpucoredata.m_stdsetvoltageandfreqDefault.size() == 0
      && m_model.m_cpucoredata.m_stdsetvoltageandfreqDefault.size() == 0 )
    {
      LOGN("No preferred voltages AND no default voltages specified.");
      m_byVoltageAndFrequencyScalingType = CPUcontrolBase::TemperatureBased ;
    }
  }
  LOGN("SetDynVoltnFreqScalingType_Inline end--DVFS type:" <<
    (WORD) m_byVoltageAndFrequencyScalingType )
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
