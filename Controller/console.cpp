/* 
 * File:   console.c
 * Author: sgebauer
 *
 * Created on 16. November 2009, 23:56
 */

//Else: "fatal error C1189: #error :  Building MFC application with /MD[d]
//(CRT dll version) requires MFC shared dll version. Please #define _AFXDLL or
//do not use /MD[d]"
//#define _AFXDLL
#include <stdio.h> //for sprintf(...)
#include <Controller/character_string/stdtstr.hpp> //std::tstring
//#include "Windows/CPUcontrolService.hpp"
//#include "Windows/ServiceBase.hpp"
#include <string>
#include <Controller/CPUcontrolBase.hpp> //for CPUcontrolBase::OuputCredits()
#include <Controller/DynVoltAndFreqScaling.hpp> //class DynVoltAndFreqScaling
#include <Controller/CPU-related/ICPUcoreUsageGetter.hpp>
#include <Controller/CPU-related/I_CPUcontroller.hpp>
#include <Controller/IDynFreqScalingAccess.hpp> //
#include <Controller/Logger/Logger.hpp> //class Logger
#include <Controller/MainController.hpp> //MainController
#include <Controller/I_CPUaccess.hpp>
#ifdef __LINUX__
#include <Linux/daemon/daemon.h>
#endif //#ifdef LINUX
#include <Linux/MSRdeviceFile.hpp> // class MSRdeviceFile
#include <preprocessor_macros/Windows_compatible_typedefs.h>
#include <UserInterface/DummyUserInterface.hpp>
#include <ModelData/ModelData.hpp> //class "Model"

#define NUMBER_OF_IMPLICITE_PROGRAM_ARGUMENTS 2

#ifdef USE_VISUAL_LEAK_DETECTOR
  //Visual Leak Detector--detects memory leaks.
  //Laufzeitfehler bei vld.dll!03207452()
  //("03207452" ist  wahrscheilich die Adresse der fehlgeschlagenen Funktion)
  #include <vld.h>
#endif //#ifdef USE_VISUAL_LEAK_DETECTOR

enum ProgramErrorCodes
{
  noError = 0
  , failedToInitCPUcontroller
  , CPUaccessExceptionError
};

Logger g_logger ;

//volatile bool g_bProcess = true ;
//volatile bool g_bRun = true ;

//g++: "‘::main’ must return ‘int’"
int main( int argc, char *  argv[] )
{
  LPTSTR ptstrProgramName = _T("X86_info_and_control") ;
  //LPTSTR ptstrProgramArg = _T("-config=GriffinControl_config.xml") ;
  std::tstring stdtstrProgramName(ptstrProgramName) ;
  std::tstring stdtstrProgramArg = std::tstring( _T("-config=") )
    + ptstrProgramName + std::tstring( _T("_config.xml") ) ;
  std::tstring stdtstrLogFileName = ptstrProgramName
    + std::tstring(_T("_log.txt") ) ;
  //try
  //{
    DEBUG("Begin of main program entry point\n");
//    //Convert std::string to wstring or remain std::string.
//    std::tstring stdtstr = Getstdtstring( stdtstrLogFileName) ;
    //Must set the exe path as current dir before (else the file is located in
    //: C:\WINDOWS\System32) !
    g_logger.OpenFile( stdtstrLogFileName ) ;
    #ifdef __LINUX__
    daemonize( std::string ( "/var/lock/subsys/" + stdtstrProgramName .c_str() );
    #endif
    //PossiblyOutputUsage() ;
    CPUcontrolBase::OuputCredits() ;

//  m_stdtstrProgramName = _T("X86_info_and_control") ;
//
//  m_arartchCmdLineArgument = new char * [NUMBER_OF_IMPLICITE_PROGRAM_ARGUMENTS];
//
//  if(m_arartchCmdLineArgument)
//  {
//    //ISpecificController
//    //Intitialise to be valid.
//    m_arartchCmdLineArgument[ 0 ] = "" ;
//    m_arartchCmdLineArgument[ NUMBER_OF_IMPLICITE_PROGRAM_ARGUMENTS - 1 ] =
//      //"-config=config.xml" ;

//    UserInterface * mp_userinterface = //p_frame ;
//      this ;
    IDynFreqScalingAccess * p_dynfreqscalingaccess = NULL ;
    I_CPUcontroller * p_cpucontroller ;
    ICPUcoreUsageGetter * p_cpucoreusagegetter ;
    I_CPUaccess * p_i_cpuaccess ;
    Model modelData ;
    MainController maincontroller ;
    DummyUserInterface dummyuserinterface ;
    TCHAR ** arartchCmdLineArgument = new TCHAR *
      [NUMBER_OF_IMPLICITE_PROGRAM_ARGUMENTS];
    //Model mp_modelData = & modelData ;
    if(arartchCmdLineArgument)
    {
      //ISpecificController
      //Intitialize to be valid.
      arartchCmdLineArgument[ 0 ] = _T("");
      arartchCmdLineArgument[ NUMBER_OF_IMPLICITE_PROGRAM_ARGUMENTS - 1 ] =
          //"-config=config.xml" ;
          (TCHAR *) stdtstrProgramArg.c_str() ;
      try //catch CPUaccessexception
      {
    #ifdef _WIN32 //Built-in macro for MinGW and MSVC (also under 64 bit)
      //If allocated statically within this block / method the object
      //gets invalid after leaving the block where it was declared.
      //p_i_cpuaccess = new WinRing0dynLinked(//p_frame
      //  this ) ;
      WinRing0dynLinked winring0dynlinked( & dummyuserinterface ) ;
      p_i_cpuaccess = & winring0dynlinked ;
    #else
      //m_maincontroller.SetCPUaccess(NULL) ;
      //m_MSRdeviceFile.SetUserInterface(this) ;
      //p_i_cpuaccess = new MSRdeviceFile(this) ;
      MSRdeviceFile msrdevicefile( & dummyuserinterface ) ;
      p_i_cpuaccess = & msrdevicefile ;
      //m_maincontroller.SetCPUaccess(&m_MSRdeviceFile) ;
    #endif
      maincontroller.SetCPUaccess( p_i_cpuaccess );
      if( ! maincontroller.Init( //m_modelData
       modelData, 
        & dummyuserinterface )
        )
        return failedToInitCPUcontroller ;

      //mp_cpucontroller = //CPUcontrollerFactory::
        maincontroller.
        //Creates e.g. an AMD Griffin oder Intel Pentium M controller
        CreateCPUcontrollerAndUsageGetter(
          p_cpucontroller
          , p_cpucoreusagegetter
          ) ;
      p_cpucontroller->SetCmdLineArgs(
        NUMBER_OF_IMPLICITE_PROGRAM_ARGUMENTS,
        arartchCmdLineArgument ) ;
      p_cpucontroller->SetUserInterface( & dummyuserinterface ) ;
      //Set the CPU access BEFORE getting number of CPU cores in
      //SetModelData(...) .
      #ifdef _WIN32 //Built-in macro for MinGW and MSVC (also under 64 bit)
      mp_cpucontroller->SetCPUaccess(mp_winring0dynlinked) ;
      #else
      //mp_cpucontroller->SetCPUaccess(NULL);
      //mp_cpucontroller->SetCPUaccess( & m_MSRdeviceFile) ;
      #endif
      p_cpucontroller->SetCPUaccess( p_i_cpuaccess ) ;
      p_cpucontroller->SetModelData( //& m_modelData
         & modelData ) ;
      if( p_cpucontroller )
        //Needed for drawing the voltage-frequency curves.
        p_cpucontroller->GetMaximumFrequencyInMHz() ;
      #ifdef _WIN32 //Built-in macro for MinGW and MSVC (also under 64 bit)
      mp_cpucontroller->SetCalculationThread(& m_calculationthread) ;
      #else
      p_cpucontroller->SetCalculationThread(NULL) ;
      #endif
      //mp_cpucontroller->SetOtherDVFSaccess(& m_powerprofdynlinked) ;
      #ifdef _WIN32 //Built-in macro for MinGW and MSVC (also under 64 bit)
      mp_dynfreqscalingaccess = new PowerProfDynLinked( m_stdtstrProgramName ) ;
      mp_cpucontroller->SetOtherDVFSaccess( mp_dynfreqscalingaccess ) ;
      #else
      p_dynfreqscalingaccess = NULL ;
      p_cpucontroller->SetOtherDVFSaccess( NULL ) ;
      #endif
      //m_modelData.SetCPUcontroller( mp_cpucontroller);
      modelData.SetCPUcontroller( p_cpucontroller);

      //Gets the data from the CPU and sets the info into the model data
      //(important step for drawing overvolt prot curve)
      p_cpucontroller->GetMaximumFrequencyInMHz() ;

    #ifdef _WIN32 //Built-in macro for MinGW and MSVC (also under 64 bit)
      m_calculationthread.SetCPUcontroller(mp_cpucontroller);
    #endif
      {
        //#endif //	#ifdef _EMULATE_TURION_X2_ULTRA_ZM82

        p_cpucontroller->SetCmdLineArgs(
          NUMBER_OF_IMPLICITE_PROGRAM_ARGUMENTS ,
          arartchCmdLineArgument
          ) ;
        BYTE byReturn = p_cpucontroller->HandleCmdLineArgs( ) ;

        DEBUG("initialization of dialog--after handling cmd line args\n");
        //DEBUG("return value of handleCmdLineArgs(): %u\n",(WORD)byReturn);
        LOG("return value of handling command line args: " << (WORD) byReturn
          //<< "\n"
          );
        switch(byReturn)
        {
          case FAILURE:
            dummyuserinterface.Confirm("An error occured (a message should have been "
              "shown previously)->exiting");
            return FALSE;
            break;
          case EXIT:
            return FALSE;
            break;
          default:
            DWORD dwValue = 0 ;
            //TODO read values from CPU at first because the other values should not
            //be affected.

  #ifdef COMPILE_WITH_CPU_SCALING
            //TODO remove memory leaks by dyn. alloc.
            //mp_wxDynLinkedCPUcoreUsageGetter = new
            //  wxDynLinkedCPUcoreUsageGetter(
            //mp_dynfreqscalingthread = new DynFreqScalingThread(
            //  //mp_wxDynLinkedCPUcoreUsageGetter
            //  mp_icpucoreusagegetter
            //  m_modelData.m_cpucoredata
            //);
            //mp_dynfreqscalingthread->Create();
            //mp_dynfreqscalingthread->Run();

            //m_dynfreqscalingthread.Start();
  #endif //#ifdef COMPILE_WITH_CPU_SCALING
        }
      }
      if( modelData.m_cpucoredata.m_stdsetvoltageandfreqWanted.size() > 0 )
      {
        DynVoltAndFreqScaling dynvoltandfreqscaling(
          p_cpucoreusagegetter //ICPUcoreUsageGetter * p_icpu
          , p_cpucontroller //I_CPUcontroller * p_cpucontroller
          , modelData.m_cpucoredata //CPUcoreData & r_cpucoredata
          ) ;
        dynvoltandfreqscaling.Entry() ;
      }
      }
      catch(//ReadMSRexception
          const CPUaccessException & e )
      {
        //this->Confirm("Reading from MSR failed kjj");
        return CPUaccessExceptionError ;
      }
    //}// if (mp_modelData)
  }
  return (EXIT_SUCCESS);
}
