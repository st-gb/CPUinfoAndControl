/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
/*
 * CPUcontrolServiceBase.cpp
 *
 *  Created on: Oct 11, 2010
 *      Author: Stefan
 */
#ifdef _WIN32
//  #define USE_WINDOWS_THREAD
#endif

#include <Controller/CPUcontrolServiceBase.hpp>
#include <Controller/CPUcoreLoadBasedDynVoltnFreqScaling.hpp>
//namespace DynFreqScalingThreadBase::ThreadFunc
#include <Controller/DynFreqScalingThreadBase.hpp>
//GetNumberOfLogicalCPUcores(...)
#include <Controller/GetNumberOfLogicalCPUcores.h>
//GetErrorMessageFromErrorCodeA(...)
#include <Controller/GetErrorMessageFromLastErrorCode.hpp>
#include <Controller/I_CPUaccess.hpp> //class CPUaccessException
//class SAX2ServiceConfigHandler
#include <Xerces/service/SAX2ServiceConfigHandler.hpp>
#include <Xerces/XMLAccess.hpp> //ReadXMLfileInitAndTermXerces(...)
#ifdef USE_WINDOWS_THREAD
  #include <Windows/DynFreqScalingThread.hpp>
#else //#ifdef USE_WINDOWS_THREAD
  #include <wxWidgets/DynFreqScalingThread.hpp>
#endif //#ifdef USE_WINDOWS_THREAD
#include <stdlib.h> //EXIT_SUCCESS
#include <wx/thread.h> //wxThread::GetCPUcount()

#define NUMBER_OF_IMPLICITE_PROGRAM_ARGUMENTS 2
#ifndef NO_ERROR
  //from winerror.h
  #define NO_ERROR 0L
#endif

extern CPUcontrolBase * gp_cpucontrolbase ;

////Needed for the exported functions.
//I_CPUaccess * g_p_cpuaccess ;
extern I_CPUaccess * g_p_cpuaccess ;

CPUcontrolServiceBase::CPUcontrolServiceBase(UserInterface * p_userinterface)
  :
  CPUcontrolBase(p_userinterface) ,
  m_maincontroller( & m_dummyuserinterface )
//  , m_wxthreadbasedi_thread(
//  , m_x86iandc_thread_typeDVFSthread(
//    //wxThread::Wait() only works for joinable threads
//    I_Thread::joinable)
{
  LOGN( FULL_FUNC_NAME << "--begin")
}

CPUcontrolServiceBase::~CPUcontrolServiceBase()
{
  // TODO Auto-generated destructor stub
}

void CPUcontrolServiceBase::CreateDVFSthreadObject_Inline(
  //BYTE byVoltageAndFrequencyScalingType
  )
{
  LOGN("creating the DVFS thread object--pointer to CPU core usage getter "
      "object:" << mp_cpucoreusagegetter )
  //Windows_API::DynFreqScalingThread dynfreqscalingthread(
  #ifdef USE_WINDOWS_THREAD
    mp_dynfreqscalingthreadbase =
    new Windows_API::DynFreqScalingThread(
      * this
      , m_model.m_cpucoredata
      ) ;
  #else //USE_WINDOWS_THREAD
    //This variant of starting the DVFS thread did not work with the Windows
    //and Linux service(?) .
  //    new wxWidgets::DynFreqScalingThread(
//      * this
  if( //mp_cpucoreusagegetter
    m_byVoltageAndFrequencyScalingType == CPUcontrolBase::LoadBased )
  {
    LOGN("Creating a CPUcoreLoadBasedDynVoltnFreqScaling object.")
    mp_dynfreqscalingthreadbase =
    //This is the Linux/ Unix variant of starting the DVFS thread (did not
    //work with wxThread/ wxWidgets::DynFreqScalingThread on Windows or Linux).
    new CPUcoreLoadBasedDynVoltnFreqScaling(
      * gp_cpucontrolbase
      , m_model.m_cpucoredata
      ) ;
  }
  else if( m_byVoltageAndFrequencyScalingType ==
      CPUcontrolBase::TemperatureBased )
  {
    LOGN("Creating a DynFreqScalingThreadBase object.")
    mp_dynfreqscalingthreadbase =
    //This is the Linux/ Unix variant of starting the DVFS thread (did not
    //work with wxThread/ wxWidgets::DynFreqScalingThread on Windows or Linux).
    new DynFreqScalingThreadBase(
      * gp_cpucontrolbase
      , m_model.m_cpucoredata
  //    , wxTHREAD_JOINABLE //wxThread::Wait() only works for detached threads.
      ) ;
  }
  #endif //USE_WINDOWS_THREAD
}

//@return 0=success
bool CPUcontrolServiceBase::HandleStartDynVoltAndFreqScalingThread(
  //BYTE byVoltageAndFrequencyScalingType
  )
{
  DisableOtherVoltageOrFrequencyChange() ;
  LOGN("service HandleStartDynVoltAndFreqScalingThread--CPU controller: "
    << mp_cpucontroller
    << "CPU usage getter: " << mp_cpucoreusagegetter )
//#ifdef __linux__
  CreateDVFSthreadObject_Inline(//byVoltageAndFrequencyScalingType
    ) ;
//  bool bContinue = false ;
#ifdef USE_WINDOWS_THREAD
  StartDynamicVoltageAndFrequencyScaling() ;
#else
  if( //for instance of the _base_ class "DynFreqScalingThreadBase" we need
      //to create a separate thread object.
      mp_dynfreqscalingthreadbase )
  {
    return StartDVFSviaThreadType_Inline(//bContinue
      ) ;
  }
  else
    LOGN("Failed to create the Dynamic Voltage and Frequency Scaling thread "
      "object/ the class instance is NULL" )
#endif
//#else
//  CreateDVFSthreadObject_Inline() ;
////  return mp_dynfreqscalingthreadbase ;
////  StartDynVoltnFreqScaling() ;
//  LOGN("mp_dynfreqscalingthreadbase:" << mp_dynfreqscalingthreadbase )
//  if( mp_dynfreqscalingthreadbase )
//    return mp_dynfreqscalingthreadbase->Start() ;
//#endif
  return 1 ;
}

//bool CPUcontrolServiceBase::HandleStartGetCPUcoreInformationThread()
//{
//  LOGN("service HandleStartGetCPUcoreInformationThread--CPU controller: "
//    << mp_cpucontroller
//    << "CPU usage getter: " << mp_cpucoreusagegetter )
//  bool bContinue = false ;
//  StartGetCPUcoreInformationThread_Inline(bContinue) ;
//  return 1 ;
//}

//Pass ERROR_SUCCESS (0L) or NO_ERROR (0L) if no error
// Stub initialization function.
DWORD CPUcontrolServiceBase::Initialize(
  DWORD   argc,
//  LPTSTR  * argv
  CPU_CONTROL_SERVICE_BASE_STRING_POINTER_TYPE * argv,
  BYTE & r_byVoltageAndFrequencyScalingType
  )
{
  //Initialize the variable.
  r_byVoltageAndFrequencyScalingType = CPUcontrolBase::none;
  //DWORD dwReturnValue = ERROR_SUCCESS ;
  DWORD dwReturnValue = ! NO_ERROR ;
  //argv;
//  LPTSTR  ** ppartch ;
  //std::vector<std::string> stdvecstdstrFurtherCmdLineArgs ;
  LOGN( "initializing the service--begin. argument count:" << argc )
//  if( argc < 2 )
//  {
//    LPTSTR ptchFirstArg
//      //Initialize to avoid g++ warning (too many warnings are unuebersichtlich)
//      = NULL ;
//
//    if( argc == 1 )
//      ptchFirstArg = argv[0] ;
//    mar_tch = new TCHAR * [NUMBER_OF_IMPLICITE_PROGRAM_ARGUMENTS];
//    argc = NUMBER_OF_IMPLICITE_PROGRAM_ARGUMENTS ;
//    m_stdtstrProgramArg = //std::tstring("-config=") + //m_stdwstrProgramName +
//      _T("-config=") +
//      m_stdtstrProgramName +
////      std::tstring("_config.xml") ;
//      _T("_config.xml") ;
//    mar_tch[0] = ptchFirstArg ;
//    mar_tch[1] = (LPTSTR) m_stdtstrProgramArg.c_str() ;
//    ppartch = & mar_tch ;
//  }
//  else
//    ppartch = & argv ;
  //Intension for this try-block is:
  //The program should not continue if initializing the CPU access failed.
  try
  {
    //LOG here because:
    //A user sent me a log file because the service crashed around this
    //place.
    LOGN("Before initializing the CPU access"//\n"
      )
//    SetInitHardwareAccessWaitHint() ;
    CreateHardwareAccessObject() ;
    g_p_cpuaccess = mp_i_cpuaccess ;
    DEBUGN("Initialize() CPU access address: " << mp_i_cpuaccess )
//    LOGN("Address of service attributes: " << mp_modelData)
    {
      //Important! because the DLLs assign their pointers to the model
      //from the I_CPUaccess::mp_model
      mp_i_cpuaccess->mp_model = & m_model ;

      //This assignment is needed for "CPUID" calls
      m_maincontroller.SetCPUaccess( //mp_winring0dynlinked) ;
        mp_i_cpuaccess ) ;
      m_maincontroller.ReadMainAndPstateConfig(
        m_model
        , & m_dummyuserinterface );
      std::string stdstrServiceCfgFile = m_model.m_std_strConfigFilePath + "/"
        + "service.xml" ;
      SAX2ServiceConfigHandler sax2serviceconfighandler(
        m_model ,
        & m_dummyuserinterface );
      if( ReadXMLfileWithoutInitAndTermXercesInline(
          stdstrServiceCfgFile.c_str()
//          , m_model
//          , p_userinterface
          , & m_dummyuserinterface
          //Base class of implementing Xerces XML handlers.
          //This is useful because there may be more than one XML file to read.
          //So one calls this functions with different handlers passed.
          //DefaultHandler & r_defaulthandler
          , sax2serviceconfighandler
          )
        )
      {
//        byRet = 1 ;
      }
      else
      {
        //std::tstring tstr(
        //  _T("Running this program is unsafe because theres was an error ") ) ;
        //tstr +=  _T("with the file containg the maximum voltages (") ;
        //tstr = tstr + strProcessorFilePath ;
        //tstr += _T(")") ;
        //throw VoltageSafetyException(
        //  tstr ) ;
//        byRet = 1 ;
      }
      int nCPUcount = //wxThread::GetCPUcount();
        GetNumberOfLogicalCPUcores();
//      LOGN( FULL_FUNC_NAME << "# CPU cores via wxWidgets:" << nCPUcount)
      LOGN( FULL_FUNC_NAME << "# CPU cores from Operating System:" << nCPUcount)
      if( nCPUcount != -1 )
        m_model.m_cpucoredata.m_byNumberOfCPUCores = nCPUcount;
      //if( !
        m_maincontroller.
        //Creates e.g. an AMD Griffin oder Intel Pentium M controller
        CreateCPUcontrollerAndUsageGetter(
          mp_cpucontroller
          , mp_cpucoreusagegetter
          ) ;
        //)
      SetDynVoltnFreqScalingType_Inline();
      if( m_byVoltageAndFrequencyScalingType == CPUcontrolBase::none )
        return 1;
      SetCPUcontroller(mp_cpucontroller) ;
//      mp_cpucontroller->SetCmdLineArgs(
//        NUMBER_OF_IMPLICITE_PROGRAM_ARGUMENTS,
//        *ppartch ) ;
      mp_cpucontroller->SetUserInterface(
        & m_dummyuserinterface ) ;
      //Set the CPU access BEFORE getting number of CPU cores in
      //SetModelData(...) .
      mp_cpucontroller->SetCPUaccess( //mp_winring0dynlinked) ;
        mp_i_cpuaccess ) ;
      mp_cpucontroller->SetModelData( & m_model) ;
      //mp_cpucontroller->SetCalculationThread(& m_calculationthread) ;
      //mp_cpucontroller->SetOtherDVFSaccess(& m_powerprofdynlinked) ;

      //mp_cpucontroller->SetOtherDVFSaccess( mp_dynfreqscalingaccess )
//      mp_cpucontroller->SetOtherDVFSaccess( & m_powerprofdynlinked ) ;
      //m_model.SetCPUcontroller( mp_cpucontroller);
      m_model.SetCPUcontroller( mp_cpucontroller);

      //mp_cpucontroller->SetCmdLineArgs(
      //  NUMBER_OF_IMPLICITE_PROGRAM_ARGUMENTS ,
      //  m_arartchCmdLineArgument
      //  ) ;

      //DEBUG("initializing the service--end\n",argc)
      LOG( "End of initializing the service\n" //<< argc
          )
//      if( byReturn == SUCCESS )
//        //It does not make much sense to run this service if not voltages
//        //are changed/ set due to an error.
//        dwReturnValue = NO_ERROR ;
      dwReturnValue = NO_ERROR ;
      }
  }
  //catch(DLLnotLoadedException e)
  catch( const CPUaccessException & cr_cpuaccessexception)
  {
    //WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE(
    //    "Exception: DLL was not loaded\n" ) ;
    dwReturnValue = ! NO_ERROR ;
    ShowMessage( cr_cpuaccessexception.m_stdstrErrorMessage ) ;
    //delete e ;
  }
  return dwReturnValue;
}

//Called e.g. when to continue the service.
bool CPUcontrolServiceBase::StartDynVoltnFreqScaling()
{
  bool bAlreadyContinued = false ;
  if( ! mp_dynfreqscalingthreadbase )
//    mp_dynfreqscalingthreadbase = new
//#ifdef USE_WINDOWS_THREAD
//    Windows_API::DynFreqScalingThread(
//#else //#ifdef USE_WINDOWS_THREAD
//      wxWidgets::DynFreqScalingThread(
//#endif //#ifdef USE_WINDOWS_THREAD
////      mp_cpucontroller->mp_icpucoreusagegetter
////      , mp_cpucontroller
//      * this
//      , m_model.m_cpucoredata
//      ) ;
    CreateDVFSthreadObject_Inline() ;
  //If allocating memory succeeded.
  if( mp_dynfreqscalingthreadbase )
  {
    LOGN( FULL_FUNC_NAME << "--already running: " <<
      (mp_dynfreqscalingthreadbase->m_vbRun ? "yes" : "no") )
    if( mp_dynfreqscalingthreadbase->m_vbRun )
      bAlreadyContinued = true ;
    else
//      mp_dynfreqscalingthreadbase->Start() ;
      StartDVFSviaThreadType_Inline(//bContinue
        ) ;

  }
  return bAlreadyContinued ;
}

//bool
//void
DWORD CPUcontrolServiceBase::StartDVFSviaThreadType_Inline(//bool & bContinue
  )
{
//  bool bContinue = false ;
  LOGN("Trying to start the Dynamic Voltage and Frequency Scaling thread.")
  //dynfreqscalingthread.Run() ;
  //    DWORD dwRet = mp_dynfreqscalingthreadbase->Start() ;
  //    DWORD dwRet = m_wxthreadbasedi_thread.start(
  //    DWORD dwRet = m_pthreadbasedi_thread.start(
  assert ( // <> NULL
    mp_dynfreqscalingthreadbase ) ;
  DWORD dwRet = m_x86iandc_thread_typeDVFSthread.start(
    DynFreqScalingThreadBaseNameSpace::ThreadFunction,
    mp_dynfreqscalingthreadbase ) ;
  if( dwRet ==  EXIT_SUCCESS )
  {
    LOGN( "Successfully started the Dynamic Voltage and Frequency Scaling "
        "thread.")
//    bContinue = true ;
  }
  else
  {
    LOGN( "Error starting Dynamic Voltage and Frequency Scaling thread :"
  //        << LocalLanguageMessageFromErrorCodeA(dwRet)
      << GetErrorMessageFromErrorCodeA(dwRet)
      )
  }
//  return bContinue ;
  return dwRet;
}

//void StartGetCPUcoreInformationThread_Inline()
//{
//  DWORD dwRet = m_x86iandc_thread_typeDVFSthread.start(
//    GetCPUcoreInformationNameSpace::ThreadFunction,
//    0 ) ;
//  if( dwRet ==  EXIT_SUCCESS )
//  {
//    LOGN( "Successfully started the Dynamic Voltage and Frequency Scaling "
//        "thread.")
//    bContinue = true ;
//  }
//  else
//  {
//    LOGN( "Error starting Dynamic Voltage and Frequency Scaling thread :"
//  //        << LocalLanguageMessageFromErrorCodeA(dwRet)
//      << GetErrorMessageFromErrorCodeA(dwRet)
//      )
//  }
//}
