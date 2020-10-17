/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH (=Trilobyte SE), Berlin,
 * Germany 2012.
 * You are allowed to modify and use the source code from
 * Trilobyte SE for free if you are not
 * making profit directly or indirectly with it or its adaption.
 * Else you may contact Trilobyte SE. */
/** dllmain.cpp
 *  Created on: 03.05.2012
 *      Author: Stefan */
  #include <preprocessor_macros/logging_preprocessor_macros.h> ////DEBUGN(...)
  #include <Controller/CPU-related/AMD/NPT_family_0Fh/AMD_NPT_family_0Fh.hpp>
  //DYN_LIB_CALLING_CONVENTION
  #include <Controller/CPUcontrollerDynLib/calling_convention.h>
  //GetCurrentVoltageAndFrequencyAMD_NPT_family_0Fh(...)
  #include <Controller/CPU-related/AMD/NPT_family_0Fh/AMD_NPT_family_0FH_SetVoltageAndMulti.hpp>

  #include <preprocessor_macros/export_function_symbols.h> //EXPORT macro
  #include <preprocessor_macros/value_difference.h> //ULONG_VALUE_DIFF
  #include <Controller/AssignPointersToExportedExeFunctions/\
AssignPointersToExportedExeMSRfunctions.h>
  #include <Controller/AssignPointersToExportedExeFunctions/\
AssignPointerToExportedExeReadPCIconfig.h>
//  #include <Controller/CPU-related/AMD/Griffin/Griffin.hpp>

  #include <typeinfo> //for typeid<>
#ifdef _WIN32
  #include <windows.h> //for
  //#include <winuser.h> //MessageBox(...)
  static HMODULE g_hModule;
  #include <Windows/Process/GetDLLfileName.hpp> //GetDLLfileName(...)
 #ifdef _DEBUG
//  #include <Windows/Logger/LogEntryOutputter.hpp> //class Windows_API::LogEntryOutputter
  #include <Windows/Process/GetCurrentProcessExeFileNameWithoutDirs/GetCurrentProcessExeFileNameWithoutDirs.hpp>
 #endif
#endif

#if defined(COMPILE_WITH_LOG) && defined(USE_PANTHEIOS)
  #include <pantheios/pantheios.hpp>
  #include <pantheios/implicit_link/core.h>
  #include <pantheios/implicit_link/fe.simple.h>
  #include <pantheios/backends/bec.file.h>      // be.file header
  #include <pantheios/implicit_link/be.file.h> //We use a log file.
#endif

#if defined(COMPILE_WITH_LOG) && defined(USE_LOG4CPLUS)
  #include <log4cplus/logger.h>
  #include <log4cplus/loggingmacros.h>
  //#include <log4cplus/configurator.h>
  #include <log4cplus/fileappender.h>
  #include <log4cplus/layout.h>
#endif

//  log4cplus::Logger log4cplus_logger;
//  Windows_API::Logger g_windows_api_logger;

  #define MAX_TIME_SPAN_IN_MS_FOR_TSC_DIFF 10000

  ReadPCIconfigSpace_func_type g_pfnReadPCIconfigSpace ;
/** Needed for call to AssignPointersToExportedExeMSRfunctions(...) */
ReadMSR_func_type g_pfnreadmsr ;
WriteMSR_func_type g_pfn_write_msr ;
  //static
    BYTE s_minimumFID;

  //static std::map<BYTE, MinAndMaxFID> s_std_mapFID2MinAndMaxFID;

#ifndef COMPILE_WITH_MODEL_ACCESS
  #define COMPILE_WITH_MODEL_ACCESS
#endif
#ifdef COMPILE_WITH_MODEL_ACCESS
  #include <Controller/I_CPUaccess.hpp> //class I_CPUaccess
  #include <ModelData/ModelData.hpp> //class Model
#else
  //Only forward declaration
  class I_CPUaccess;
#endif

  //Must create in DllMain?
//  Logger * p_g_logger;

  //from http://www.pantheios.org/doc/html/cpp_2backends_2example_8cpp_8backends_8file_2example_8cpp_8backends_8file_8cpp-example.html
  /* Define the stock front-end process identity, so that it links when using
   * fe.N, fe.simple, etc. */
//  PANTHEIOS_EXTERN_C const PAN_CHAR_T PANTHEIOS_FE_PROCESS_IDENTITY[] = PANTHEIOS_LITERAL_STRING("example.cpp.backends.file");

#ifdef _WIN32
  EXPORT BOOL APIENTRY DllMain(
    HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
    )
  {
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
#ifdef _DEBUG
      //see http://stackoverflow.com/questions/846044/how-to-get-the-filename-of-a-dll:
      g_hModule = hModule;
#endif //#ifdef _DEBUG
//      //Force the cond. "< min. time diff" to become true.
//      g_dwPreviousTickCountInMilliseconds = ::GetTickCount() ;
//      g_dwPreviousTickCountInMilliseconds
//        //->time diff gets > max. time diff, so it calcs a ref clock.
//        -= ( MAX_TIME_SPAN_IN_MS_FOR_TSC_DIFF + 1 );
//      //The reference clock is needed for setting the current frequency. So it
//      //must be determined prior to any call of this function.
//      GetCurrentReferenceClock(12.0, 100 , MAX_TIME_SPAN_IN_MS_FOR_TSC_DIFF ) ;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
      break;
    }
    return TRUE;
  }
#endif //#ifdef _WIN32

  //#define NEHALEM_DLL_CALLING_CONVENTION __stdcall
  #define NEHALEM_DLL_CALLING_CONVENTION

  /** @brief The reference clock might change, also during runtime.
  *   This is why it is a good idea to get the possible multipliers.
  * @return The array pointed to by the return value must be freed by the
  *  caller (i.e. x86I&C GUI or service) of this function. */
  EXPORT float * DYN_LIB_CALLING_CONVENTION GetAvailableMultipliers(
      WORD wCoreID
      , WORD * p_wNumberOfArrayElements
      )
  {
    DEBUGN("inside DLL--begin")
    return AMD::family0Fh::GetAvailableMultipliers( * p_wNumberOfArrayElements) ;
  }

  /** @return The array pointed to by the return value must be freed by the
  *  caller (i.e. x86I&C GUI or service) of this function. */
  EXPORT float * GetAvailableVoltagesInVolt(
    WORD wCoreID
    , WORD * p_wNumberOfArrayElements )
  {
    DEBUGN("begin")
    return AMD::family0Fh::GetAvailableVoltages( * p_wNumberOfArrayElements) ;
  }

  EXPORT BYTE DYN_LIB_CALLING_CONVENTION
    GetCurrentVoltageAndFrequency(
      float * p_fVoltageInVolt
      //multipliers can also be floats: e.g. 5.5 for AMD Griffin.
      , float * p_fMultiplier
      , float * p_fReferenceClockInMHz
      , WORD wCoreID
    )
  {
    static BYTE byRet;
    byRet = AMD::family0Fh::GetCurrentVoltageAndFrequency(
      p_fVoltageInVolt,
      p_fMultiplier ,
      p_fReferenceClockInMHz ,
      wCoreID
      ) ;
    DEBUGN("return " << (WORD) byRet << " ref.clock:"
      << * p_fReferenceClockInMHz << "MHz multi:" << * p_fMultiplier
      << " " << * p_fVoltageInVolt << "V")
    return byRet;
  }

  EXPORT float DYN_LIB_CALLING_CONVENTION GetTemperatureInCelsius(
    WORD wCoreID )
  {
//    DEBUGN("begin ")
    return //fTempInDegCelsius ;
      //0.0 ;
      AMD::family0Fh::GetTemperatureInDegCelsius();
  }

  //#define DLL_CALLING_CONVENTION __stdcall
  #define DLL_CALLING_CONVENTION

  void InsertDefaultVoltageForStartUpFID(I_CPUaccess * pi_cpuaccess)
  {
    DEBUGN("begin")
    uint32_t lowmostMSRbits;
    uint32_t highmostMSRbits;
    BYTE readMSRreturnValue;

    readMSRreturnValue = ReadMSR(
      FIDVID_STATUS_MSR_ADDRESS,
      & lowmostMSRbits,
      & highmostMSRbits,
      1
      );
    //"Startup VID (StartVID)—Bits 45–40."
    BYTE StartupVID = (highmostMSRbits >> 8 ) & BITMASK_FOR_LOWMOST_6BIT;
    float fVoltageInVolt = AMD::family0Fh::GetVoltageInVolt( StartupVID);

    //"Startup FID (StartFID)—Bit 13–8"
    BYTE StartupFID = (lowmostMSRbits >> 8 ) & BITMASK_FOR_LOWMOST_6BIT;
#ifdef USE_STARTUP_FID_AS_MIN_FID
    s_minimumFID = StartupFID;
#endif //#ifdef USE_STARTUP_FID_AS_MIN_FID
    float fMultiplier = AMD::family0Fh::GetMultiplier(StartupFID);
    DEBUGN("before inserting default voltage for frequency")
    //TODO insert a check (check sum etc.) that ensures that both the executable
    //(GUI or service) and the dynlib have the same here!
    pi_cpuaccess->mp_model->m_cpucoredata.m_stdsetvoltageandfreqDefault.insert(
      VoltageAndFreq(fVoltageInVolt, (WORD) ( fMultiplier * //g_fReferenceClockInMHz
        200.0f)
        )
      );
    DEBUGN("end")
  }

#if defined (_DEBUG) && defined(USE_LOG4CPLUS)
  void InitLog4cplusLogger(const std::tstring & std_tstrDLLfileName)
  {
    //from http://log4cplus.sourceforge.net/hello_world.html:
    log4cplus_logger = log4cplus::Logger::getInstance(
      LOG4CPLUS_TEXT("main") );
    log4cplus::SharedAppenderPtr append_1(
      new log4cplus::RollingFileAppender( LOG4CPLUS_TEXT(std_tstrDLLfileName
        + "_log4cplus_logger.txt"),
        5*1024, 5));
    log4cplus_logger.addAppender(append_1);
    append_1->setLayout( std::auto_ptr<log4cplus::Layout>(
      new log4cplus::PatternLayout("%d{%H:%M:%s}") ) );

    LOG4CPLUS_INFO(log4cplus_logger, LOG4CPLUS_TEXT( FULL_FUNC_NAME ));
  }
#endif //#ifdef _DEBUG

  inline void InitPantheiosLogger()
  {
//      //from http://www.pantheios.org/doc/html/cpp_2backends_2example_8cpp_8backends_8file_2example_8cpp_8backends_8file_8cpp-example.html:
//      pantheios_be_file_setFilePath(
//        PSTR("AMD_NPT_ControllerDLL_pantheios_log.txt"),
//        PANTHEIOS_BE_FILE_F_TRUNCATE,
//        PANTHEIOS_BE_FILE_F_TRUNCATE,
//        PANTHEIOS_BEID_ALL);
  }

  void AssignPointersToExportedExefunctions()
  {
    AssignPointersToExportedExeMSRfunctions_inline(g_pfnreadmsr, g_pfn_write_msr) ;
    AssignPointerToExportedExeReadPCIconfig(g_pfnReadPCIconfigSpace) ;
    DEBUGN( "g_pfnReadPCIconfigSpace:" << (void *) g_pfnReadPCIconfigSpace )

    if( ! g_pfnreadmsr || ! g_pfn_write_msr )
    {
#ifdef _WIN32
      MessageBox(NULL,
        "Pointers could not be assigned to the execu-tables export functions\n"
        "Does the executable that loads this DLL have ReadMSR and WriteMSR"
        "export functions at all?(analyze this with a tool)"
        //Title
        ,"error"
        , MB_OK) ;
      return ;
#endif //#ifdef _WIN32
    }
  }

//  float GetMaxVoltageinVolt()
//  {
//    DWORD eax, edx;
//    if( ReadMSR(FIDVID_STATUS_MSR_ADDRESS, & eax, & edx, 1) )
//    {
//
//    }
//  }

#ifdef _DEBUG
  void OpenLogFile()
  {
#ifdef _WIN32
    TCHAR fileName[MAX_PATH];
#endif
    std::tstring std_tstrDLLfileName
#ifdef _WIN32 //win 32 or 64 bit
    = GetDLLfileName(g_hModule, fileName)
#endif //#ifdef _WIN32
#ifdef __linux__
    = "AMD_NPT_family_0F"
#endif //#ifdef __linux__
    ;

    std::string strExeFileNameWithoutDirs
#ifdef _WIN32
    = CurrentProcess::GetExeFileNameWithoutDirs_inline()
#endif
      ;
    std::string stdstrFilename;
//    if( typeid(g_logger) == typeid(::Logger) )
//      stdstrFilename = //strExeFileNameWithoutDirs +
//        //("AMD_NPT_ControllerDLL_log.txt") ;
//        std_tstrDLLfileName + "_std_ofstream_logger.txt";
//    if( typeid(g_logger) == typeid(Windows_API::Logger) )
//      stdstrFilename =
//        std_tstrDLLfileName + "_Windows_API_logger.txt";
    stdstrFilename = std_tstrDLLfileName + strExeFileNameWithoutDirs +
      "_log.txt";
//      g_logger.OpenFile2( stdstrFilename ) ;
    g_logger.OpenFileA(stdstrFilename);

//    DEBUGN("chars for module name needed:" //<< dwChars //<< ar_strModuleName
//          << strExeFileNameWithoutDirs )
//    g_windows_api_logger.OpenFile2(
//      std_tstrDLLfileName + "Windows_API_logger2.txt") ;

#ifdef _WIN32
    DEBUGN( "this module's file name:" << fileName)
#endif
//    DEBUGN_LOGGER_NAME( g_windows_api_logger, "this module's file name:"
//      //<< fileName << " "
//      << std_tstrDLLfileName)

    InitPantheiosLogger();

//    InitLog4cplusLogger(std_tstrDLLfileName);
//      p_g_logger = new Logger();
//      if( ! p_g_logger )
//        return FALSE;
//      DEBUGN_LOGGER_NAME( *p_g_logger, "this Log file is open")
  }
#endif //#ifdef _DEBUG

  /** All init code into this function and not into "DLLMain" because Dllmain can
  * blocks other processes from load a DLL according to U. Pohl. */
  EXPORT void /*WINAPI*/ DYN_LIB_CALLING_CONVENTION Init( //I_CPUcontroller * pi_cpu
    //CPUaccess object inside the exe.
    I_CPUaccess * pi_cpuaccess
  //  Trie *
  //  , ReadMSR_func_type pfnreadmsr
    //BYTE by
    )
  {
    DEBUGN("begin")
//#ifdef __linux__
//    AssignPointersToExportedExeMSRfunctions()
//      g_pfnreadmsr , g_pfn_write_msr ) ;
//#else
    AssignPointersToExportedExefunctions();
//#endif
#ifdef _DEBUG
    OpenLogFile();
    //  LPSTR = new STR[dwChars] ;
    //  DEBUGN()
#endif
  g_s_stepping = GetStepping();
  GetAdvancedPowerManagementInformationViaCPUID();
//  s_maxVoltageInVolt = GetMaxVoltage();

  //  g_pi_cpuaccess = pi_cpuaccess ;
    std::string stdstrFilename = //strExeFileNameWithoutDirs +
      ("PentiumM_DLL_log.txt") ;
  #ifdef _DEBUG
    //ReadMSR_func_type rdmsr = (ReadMSR_func_type) (void*) & pi_cpuaccess->RdmsrEx ;
    std::stringstream stdstrstream ;
//    //For checking if the members are on the same RAM address between MSVC and MinGW:
//    stdstrstream << "DLL::Init(...)--\naddress of I_CPUaccess:" << & pi_cpuaccess << "\n"
//      << "address of I_CPUaccess::mp_model: " << & pi_cpuaccess->mp_model <<"\n"
//      << "address in I_CPUaccess::mp_model: " << pi_cpuaccess->mp_model <<"\n"
//      //<< "address in I_CPUaccess::RdmsrEx: " << & pi_cpuaccess->RdmsrEx()
//      << "address of I_CPUaccess::mp_cpu_controller: " <<
//        & pi_cpuaccess->mp_cpu_controller <<"\n"
//      << "address in I_CPUaccess::mp_cpu_controller: " <<
//        pi_cpuaccess->mp_cpu_controller ;
//    MessageBoxA( NULL, stdstrstream.str().c_str() , //TEXT("")
//      "", MB_OK) ;
  #endif
  #ifdef COMPILE_WITH_MAX_MULTI_FOR_P_STATE_LIMIT
    g_pi_cpuaccess = pi_cpuaccess ;
//    g_p_cpucontroller = pi_cpuaccess->mp_cpu_controller ;
    g_p_stdsetvoltageandfreqWanted = & g_pi_cpuaccess->mp_model->m_cpucoredata.
      m_stdsetvoltageandfreqWanted ;
  #endif //INSERT_DEFAULT_P_STATES
//    CreateFIDtoPortalCoreFIDmapping();
  #ifdef InsertDefaultVoltages
    InsertDefaultVoltageForStartUpFID(pi_cpuaccess);
  #endif //#ifdef InsertDefaultVoltages
  //  AssignExeFunctionPointers() ;
    //g_nehalemcontroller.SetCPUaccess( pi_cpuaccess ) ;
    //MSC-generated version has no problems
  //#ifndef _MSC_VER
  //  std::stringstream str ;
  //  str << "DLL::Init--Adress of CPUaccess: " << pi_cpuaccess ;
  //  MessageBox( NULL, str.str().c_str() , TEXT("") , MB_OK) ;
  //#endif
    //g_clocksnothaltedcpucoreusagegetter.SetCPUaccess( pi_cpuaccess ) ;
  }

  EXPORT BYTE DYN_LIB_CALLING_CONVENTION //can be omitted.
    SetCurrentVoltageAndMultiplier(
      float fVoltageInVolt
      //multipliers can also be floats: e.g. 5.5 for AMD Griffin.
      , float fMultiplier
      , WORD wCoreID
    )
  {
#ifdef _DEBUG
//    LOG4CPLUS_INFO(log4cplus_logger, LOG4CPLUS_TEXT( FULL_FUNC_NAME ));
#endif //#ifdef _DEBUG
    DEBUGN("begin--fVoltageInVolt:" << fVoltageInVolt
      << " fMultiplier:" << fMultiplier)
    //TODO move this documentation to the implementing function
    /** see "10.5 Processor Performance States"
     *  10.5.1.1 P-state Recognition Algorithm
     *  "10.5.7.2 P-state Transition Algorithm"
     * "Note: Software must hold the FID constant when changing the VID." */
    /** "Odd FID values are supported in revision G and later revisions" */
    //TODO return return value of this function?
    SetCurrentVoltageAndMultiplier_AMD_NPT_family_0FH( fVoltageInVolt ,
      fMultiplier ,
      wCoreID ) ;
    DEBUGN("end--return 1")
    return 1 ;
  }
