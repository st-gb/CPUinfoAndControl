/* 
 * File:   console.c
 * Author: sgebauer
 *
 * Created on 16. November 2009, 23:56
 */

//Else: "fatal error C1189: #error :  Building MFC application with /MD[d] (CRT dll version) requires MFC shared dll version. Please #define _AFXDLL or do not use /MD[d]"
//#define _AFXDLL
//#include <AfxWin.h> //for AfxMessageBox(...)->muss MultiThreaded DLL ("/MD") sein
#include <stdio.h> //for sprintf(...)
//#include "PumaStateCtrl.h"
#include <fstream> //for class ofstream
//#include "GriffinStateControlService.hpp"
#include <Controller/stdtstr.hpp> //std::tstring
//#include "Windows/CPUcontrolService.hpp"
//#include "Windows/ServiceBase.hpp"
//#include <Windows/LocalLanguageMessageFromErrorCode.h>
//#include <Windows/CurrentDir.h> // for SetExePathAsCurrentDir()
#include <string>
//#include <conio.h> //for getche()
#include <Controller/DynVoltAndFreqScaling.hpp> //DynVoltAndFreqScaling
#include <Controller/ICPUcoreUsageGetter.hpp>
#include <Controller/IDynFreqScalingAccess.hpp> //
#include <Controller/Logger.hpp>
#include <Controller/MainController.hpp> //MainController
#include <Controller/I_CPUaccess.hpp>
#include <Controller/I_CPUcontroller.hpp>
#ifdef LINUX
#include <Linux/daemon.h>
#endif //#ifdef LINUX
#include <Linux/MSRdeviceFile.h> // class MSRdeviceFile
#include <Windows_compatible_typedefs.h>
#include <DummyUserInterface.hpp>
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

FILE * fileDebug ;
Logger g_logger ;

//#include "wx/window.h"
//#include "wx/power.h" //for power mgmt notification (wxPowerType et.c), EVT_POWER_RESUME

//SERVICE_STATUS          g_servicestatus;
//SERVICE_STATUS_HANDLE   g_servicestatushandle;
//volatile bool g_bProcess = true ;
//volatile bool g_bRun = true ;
//
////from wx/event.h:
////error C2374: 'sm_eventTableEntries': Neudefinition; Mehrfachinitialisierung
////static const wxEventTableEntry sm_eventTableEntries[];
////static const wxEventTable        sm_eventTable;
////error C2270: 'GetEventTable': Modifizierer für Funktionen, die keine Memberfunktionen sind, nicht zulässig
////virtual const wxEventTable*      GetEventTable() const;
////error C2374: 'sm_eventHashTable': Neudefinition; Mehrfachinitialisierung
////static wxEventHashTable          sm_eventHashTable;
////error C2270: 'GetEventHashTable': Modifizierer für Funktionen, die keine Memberfunktionen sind, nicht zulässig
////virtual wxEventHashTable&        GetEventHashTable() const;
//
////void OnResume(wxPowerEvent& WXUNUSED(event)) ;
////
////const wxEventTableEntry sm_eventTableEntries[] = {
//// #ifdef wxHAS_POWER_EVENTS
////  //EVT_POWER_SUSPENDING(MyFrame::OnSuspending)
////  //EVT_POWER_SUSPENDED(MyFrame::OnSuspended)
////  //EVT_POWER_SUSPEND_CANCEL(MyFrame::OnSuspendCancel)
////#endif // wxHAS_POWER_EVENTS
////  EVT_POWER_RESUME(OnResume)
////    END_EVENT_TABLE()
////    //DECLARE_EVENT_TABLE_ENTRY( wxEVT_NULL, 0, 0, 0, 0 )
////  //};
////const wxEventTable sm_eventTable = { //&wxWindow::sm_eventTable,
////    sm_eventTableEntries[0] };
//////error C2270: 'GetEventTable': Modifizierer für Funktionen, die keine Memberfunktionen sind, nicht zulässig
//////const wxEventTable GetEventTable() const
//////{
//////    return & sm_eventTable;
//////}
////wxEventHashTable sm_eventHashTable( sm_eventTable);
////error C2270: 'GetEventHashTable': Modifizierer für Funktionen, die keine Memberfunktionen sind, nicht zulässig
////wxEventHashTable & GetEventHashTable() const
////{
////    return sm_eventHashTable;
////}
//
//VOID SvcDebugOut(LPSTR String, DWORD Status);
//

//bool IsWithinCmdLineArgs( int argc, char *  argv[], LPCTSTR p_tstr )
//{
//    bool bIsWithinCmdLineArgs = false ;
//    BYTE byIndex = 1 ;
//    for( ; byIndex < argc ; ++ byIndex )
//        if ( strcmp(argv[byIndex], p_tstr) == 0 )
//        {
//            bIsWithinCmdLineArgs = true ;
//            break ;
//        }
//    return bIsWithinCmdLineArgs ;
//}
//
//bool ShouldCreateService(int argc, char *  argv[])
//{
//    bool bShouldCreateService = false ;
//    //if( argc > 1 )
//    //    //if( strcmp(argv[1],"-i") == 0 )
//    //    if( std::string(argv[1]) == "-i" )
//    if ( IsWithinCmdLineArgs(argc, argv, "-i" )
//        )
//       bShouldCreateService = true ;
//
//    if( ! bShouldCreateService )
//    {
//      std::cout << "\n"
//          "Should the undervolting service be installed now? [Y]es/ "
//        "other key: no\n"//; finish with ENTER/RETURN key\n"
//        ;
//        char ch ;
//        //std::cin >> ch ;
//        ch = getche() ;
//        if( ch == 'Y' )
//            bShouldCreateService = true ;
//    }
//    return bShouldCreateService ;
//}
//
//bool ShouldDeleteService(int argc, char *  argv[])
//{
//    bool bShouldDeleteService = false ;
//    //if( argc > 1 )
//    //    //if( strcmp(argv[1],"-i") == 0 )
//    //    if( std::string(argv[1]) == "-d" )
//    if ( IsWithinCmdLineArgs(argc, argv, "-d" )
//        )
//       bShouldDeleteService = true ;
//
//    return bShouldDeleteService ;
//}

void OuputCredits()
{
    WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE(
        //"This program is an undervolting program for AMD family 17 CPUs.\n"
        "This program is CPU information and control program for .\n"
        //maincontroller.GetSupportedCPUtypes() ;
        "license/ info: http://amd.goexchange.de / http://sw.goexchange.de\n" )
    std::cout <<
        "This executable is both in one:\n"
        "-an (de-)installer for the undervolting service\n"
        "-the undervolting service itself\n" ;
    WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE(
        "Build time: " __DATE__ " " __TIME__ " (Greenwich Mean Time + 1)" );
}

//g++: "‘::main’ must return ‘int’"
int main( int argc, char *  argv[] )
{
  LPTSTR ptstrProgramName = _T("X86_info_and_control") ;
  //LPTSTR ptstrProgramArg = _T("-config=GriffinControl_config.xml") ;
  std::tstring stdtstrProgramName(ptstrProgramName) ;
  std::tstring stdtstrProgramArg = std::tstring("-config=") + ptstrProgramName +
    std::tstring("_config.xml") ;
  std::string stdstrLogFileName = ptstrProgramName + std::tstring("_log.txt") ;
  //try
  //{
    ////TODO make file path variable.
    //fileDebug = fopen ("C:\\Temp\\GriffinStateControlSvc_debug.txt","w");
      //= fopen("GriffinStateControlSvc_debug.txt","w");
    //g_logger.OpenFile( std::string("C:\\Temp\\GriffinStateControlSvc_log.txt") ) ;

    //GriffinStateControlService griffinstatecontrolservice ;
    //griffinstatecontrolservice.SetCommandLineArgs( argc, argv ) ;
    //std::string stdstrLogFilePath =
    //    griffinstatecontrolservice.GetLogFilePath() ;
    //if( //argc > 1 && //GriffinStateControlService::PathOption(argc, argv)
    //    stdstrLogFilePath != _T("")
    //  )
    //  g_logger.OpenFile(//griffinstatecontrolservice.GetLogFilePa
    //        stdstrLogFilePath) ;
    //ofstream ofstreamLogFile;
    //if(
        //ofstreamLogFile.open("GriffinContro_log.txt")
        //if( ofstreamLogFile.good() )

    DEBUG("Begin of main program entry point\n");
/*
    SetExePathAsCurrentDir() ;
*/
    //Must set the exe path as current dir before (else the file is located in
    //: C:\WINDOWS\System32) !
    g_logger.OpenFile( //std::string("GriffinControl_log.txt")
      stdstrLogFileName ) ;
    #ifdef LINUX
    daemonize( std::string ( "/var/lock/subsys/" + stdtstrProgramName .c_str() );
    #endif
    //PossiblyOutputUsage() ;
    OuputCredits() ;
/*
    if( argc == 1 )
    {
        //IDEA: to distinguish between if called as service or by a user:
        //get the parent process name of this process.
        //Programs that run as service have "services.exe" as their parent
        //process.
        CPUcontrolService::outputUsage();
        std::vector<std::string> vecstdstrParams ;
//Useful for debugging purposes.
#ifdef EMULATE_EXECUTION_AS_SERVICE
#else
        CPUcontrolService::requestOption(vecstdstrParams);
#endif //#ifdef EMULATE_EXECUTION_AS_SERVICE
        if( vecstdstrParams.empty() )
        {
            LOG("This exe is started as service (and not as installer)\n");
        }
        else
        {
            DEBUG("Entered strings: ")
            for ( BYTE by = 0 ; by < vecstdstrParams.size() ; ++ by )
                //DEBUG("\"%s\" ", vecstdstrParams.at(by).c_str() );
                LOG("\"" << vecstdstrParams.at(by).c_str() << "\" " );
            DEBUG("\n" );
        }
        if( CPUcontrolService::ShouldCreateService(vecstdstrParams) )
        {
            if( vecstdstrParams.size() > 1 )
                ServiceBase::CreateService( vecstdstrParams.at(1).c_str() ) ;
        }
        else if( CPUcontrolService::ShouldDeleteService(vecstdstrParams) )
        {
            if( vecstdstrParams.size() > 1 )
                ServiceBase::DeleteService(//"GriffinStateService"
                   vecstdstrParams.at(1).c_str() ) ;
        }
        else
        {
            //GriffinStateControlService griffinstatecontrolservice ;
            CPUcontrolService cpucontrolservice(argc,argv,
              stdtstrProgramName ) ;
            cpucontrolservice.StartService();
        }
    }
    else
    {
        if( ShouldDeleteService(argc, argv) && argc > 2 )
            ServiceBase::DeleteService(//"GriffinStateService"
                argv[2] ) ;
        if( ShouldCreateService(argc, argv) && argc > 2 )
        //    CreateService(
        //cout << "hhjhj\n" ;
        //if( //::AfxMessageBox("Should the undervolting service be installed now?",
        //    //IDYES | IDNO ) == IDYES  )
           ServiceBase::CreateService(//"GriffinControlService"
                argv[2]) ;
        else
            std::cout << "\nNOT installing the service\n" ;
    }
    DEBUG("Run this program as \"local system account\" account within the "
        "service's properties in Windows->control panel->management->services "
        "else errors may occur\n");
    if( argc == 1 )
    {
        //GriffinStateControlService::outputUsage();
      //GriffinStateControlService griffinstatecontrolservice ;
       //griffinstatecontrolservice.StartService();
    }
  }
  catch( ConnectToSCMerror e )
  {
    WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE("Error connecting to the service control manager:\n"
      << LocalLanguageMessageFromErrorCode(e.m_dwErrorCode)
      << PossibleSolution(e.m_dwErrorCode) )
  }
*/
//  std::cout << "Waiting for input in order for the output to be readable."
//     " Hit any key to exit this program\n" ;
//   getche() ;
//   DEBUG("end of main program entry point\n");
//}
//
//#include <stdio.h>
//#include <stdlib.h>

/*
 * 
 */
//int main(int argc, char** argv)
//{
//  m_stdtstrProgramName = _T("X86_info_and_control") ;
//
//  m_arartchCmdLineArgument = new char * [NUMBER_OF_IMPLICITE_PROGRAM_ARGUMENTS];
//
//  if(m_arartchCmdLineArgument)
//  {
//    //ISpecificController
//    //MyFrame * p_frame ;
//    //Intitialise to be valid.
//    m_arartchCmdLineArgument[ 0 ] = "" ;
//    m_arartchCmdLineArgument[ NUMBER_OF_IMPLICITE_PROGRAM_ARGUMENTS - 1 ] =
//      //"-config=config.xml" ;
//      "-config=GriffinControl_config.xml" ;

//    UserInterface * mp_userinterface = //p_frame ;
//      this ;
    IDynFreqScalingAccess * p_dynfreqscalingaccess = NULL ;
    I_CPUcontroller * p_cpucontroller ;
    ICPUcoreUsageGetter * p_cpucoreusagegetter ;
    I_CPUaccess * p_i_cpuaccess ;
    Model modelData ;
    MainController maincontroller ;
    DummyUserInterface dummyuserinterface ;
    TCHAR ** arartchCmdLineArgument = new TCHAR * [NUMBER_OF_IMPLICITE_PROGRAM_ARGUMENTS];
    //Model mp_modelData = & modelData ;
    if(arartchCmdLineArgument)
    {
      //ISpecificController
      //MyFrame * p_frame ;
      //Intitialise to be valid.
      arartchCmdLineArgument[ 0 ] = "" ;
      arartchCmdLineArgument[ NUMBER_OF_IMPLICITE_PROGRAM_ARGUMENTS - 1 ] =
          //"-config=config.xml" ;
          (TCHAR *) stdtstrProgramArg.c_str() ;
      try //catch CPUaccessexception
      {
    #ifdef _WINDOWS
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
      #ifdef _WINDOWS
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
      #ifdef _WINDOWS
      mp_cpucontroller->SetCalculationThread(& m_calculationthread) ;
      #else
      p_cpucontroller->SetCalculationThread(NULL) ;
      #endif
      //mp_cpucontroller->SetOtherDVFSaccess(& m_powerprofdynlinked) ;
      #ifdef _WINDOWS
      mp_dynfreqscalingaccess = new PowerProfDynLinked( m_stdtstrProgramName ) ;
      mp_cpucontroller->SetOtherDVFSaccess( mp_dynfreqscalingaccess ) ;
      #else
      p_dynfreqscalingaccess = NULL ;
      p_cpucontroller->SetOtherDVFSaccess( NULL ) ;
      #endif
      //m_modelData.SetGriffinController(mp_pstatectrl) ;
      //m_modelData.SetCPUcontroller( mp_cpucontroller);
      modelData.SetCPUcontroller( p_cpucontroller);

      //Gets the data from the CPU and sets the info into the model data
      //(important step for drawing overvolt prot curve)
      //mp_pstatectrl->GetMaximumFrequencyInMHz() ;
      p_cpucontroller->GetMaximumFrequencyInMHz() ;

    #ifdef _WINDOWS
      //m_calculationthread.SetPumaStateCtrl(mp_pstatectrl);
      m_calculationthread.SetCPUcontroller(mp_cpucontroller);
    #endif
      {
        //#endif //	#ifdef _EMULATE_TURION_X2_ULTRA_ZM82

        p_cpucontroller->SetCmdLineArgs(
          NUMBER_OF_IMPLICITE_PROGRAM_ARGUMENTS ,
          arartchCmdLineArgument
          ) ;
        BYTE byReturn = //mp_pstatectrl->handleCmdLineArgs() ;
          p_cpucontroller->HandleCmdLineArgs( ) ;

        DEBUG("initialization of dialog--after handling cmd line args\n");
        //DEBUG("return value of handleCmdLineArgs(): %u\n",(WORD)byReturn);
        LOG("return value of handling command line args: " << (WORD) byReturn << "\n" );

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
  //          DEBUG("Before starting timer\n");
            //mp_wxdynfreqscalingtimer->mp_pumastatectrl = mp_pstatectrl ;
            DWORD dwValue = 0 ;
            //TODO read values from CPU at first because the other values should not
            //be affected.

  #ifdef COMPILE_WITH_CPU_SCALING
            //mp_wxdynfreqscalingtimer->Start(400);

            //TODO remove memory leaks by dyn. alloc.
            //mp_wxDynLinkedCPUcoreUsageGetter = new //wxDynLinkedCPUcoreUsageGetter(
            //mp_dynfreqscalingthread = new DynFreqScalingThread(
            //  //& m_cpucoreusagegetteriwbemservices
            //  //mp_wxDynLinkedCPUcoreUsageGetter
            //  mp_icpucoreusagegetter
            //  , mp_pstatectrl
            //  , //mp_pstatectrl->m_model.m_cpucoredata
            //  m_modelData.m_cpucoredata
            //);
            //mp_dynfreqscalingthread->Create();
            //mp_dynfreqscalingthread->Run();

            //m_dynfreqscalingthread.Start();
            //DEBUG("After starting timer\n");
  #endif //#ifdef COMPILE_WITH_CPU_SCALING
        }
      }
      //const wxCmdLineParser& parser
      //  size_t count = parser.GetParamCount();
      //  for ( size_t param = 0; param < count; param++ )
      //  {
      //      s << parser.GetParam(param) << ' ';
      //  }
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
          CPUaccessException e)
      {
        //this->Confirm("Reading from MSR failed kjj");
        //mp_frame->Confirm("CPU access error.\n(insuff rights?)");
        return CPUaccessExceptionError ;
      }
    //}// if (mp_modelData)
  }
  return (EXIT_SUCCESS);
}

