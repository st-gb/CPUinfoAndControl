#define _AFXDLL

#ifdef USE_VISUAL_LEAK_DETECTOR
  //Visual Leak Detector--detects memory leaks.
  //Laufzeitfehler bei vld.dll!03207452()
  //("03207452" ist  wahrscheilich die Adresse der fehlgeschlagenen Funktion)
  #include <vld.h>
#endif //#ifdef USE_VISUAL_LEAK_DETECTOR

#ifndef WX_PRECOMP
  #include "wx/app.h"
#endif
#include "wx/wx.h" //for wxMessageBox(...) (,etc.)
#include "App.hpp"

#include "wxDynFreqScalingTimer.hpp"
//#include "Controller/CPUcoreUsageGetterIWbemServices.hpp"
//#include "../Windows/CPUcoreUsageGetterIWbemServices.hpp"
//#include <Windows/CPUcoreUsageGetterNTQSI_WintopVxd.hpp>
//#include "wxDynLinkedCPUcoreUsageGetter.hpp"
//#include <Windows/WinRing0dynlinked.hpp>
//#include <Windows/GetWindowsVersion.h>
//#include <Windows/PowerProf/PowerProfDynLinked.hpp>
//#include <Windows/PowerProf/PowerProfUntilWin6DynLinked.hpp>
//#include <Windows/LocalLanguageMessageFromErrorCode.h>
#include <Controller/I_CPUcontroller.hpp>
#include <ModelData/ModelData.hpp>
#include "MainFrame.hpp"
#include "DynFreqScalingThread.hpp"
#include "Linux/MSRdeviceFile.h"
#include <strstream> //ostrstream
#include <string> //
//#include "../Windows/DynFreqScalingThread.hpp"
//#include "CpuUsage.h"
//#include "Controller/RunAsService.h" //for MyServiceStart etc.

FILE * fileDebug ; //for debug logging.
//This global (important for using preprocessor macros) object is used for 
//easy logging.
Logger g_logger ;

//Erzeugt ein wxAppConsole-Object auf dem Heap.
IMPLEMENT_APP(wxPumaStateCtrlApp) 

bool wxPumaStateCtrlApp::Confirm(const std::string & str)
{
  //::AfxMessageBox(str.c_str());

  //To not show too many dialogs that the timer would bring up.
  if( m_bConfirmedYet )
  {
    m_bConfirmedYet = false ;
    #ifdef wxUSE_WCHAR_T
      std::wstring wstr(str.begin(), str.end() ) ;
      wxString wxstr( wstr) ;
    #else
      wxString wxstr(( const unsigned char * ) str.c_str() ) :
    #endif
    ::wxMessageBox( 
      #ifdef _DEBUG_
      wxT("gg"), wxT("bla"),wxOK
      #else
      wxstr
      #endif
      );
//      #ifdef _DEBUG
//    ::wxMessageBox( wxT("This is the message."), wxT("This is the title"),
//      wxOK|wxICON_INFORMATION);
//      #endif
//    wxMessageDialog * pdlg = new wxMessageDialog(wxGetApp().mp_frame,
//      wxT("hh"),wxT("ca")
//      );
//    pdlg->ShowModal() ;
    m_bConfirmedYet = true ;
  }
  //m_bConfirmedYet = true ;
  return true;
}

bool wxPumaStateCtrlApp::Confirm(std::ostrstream & r_ostrstream
  //std::ostream & r_ostream
  )
{
  bool bReturn = true ;
  DEBUG("App::Confirm--begin\n");
  //Must set this, else text may follow after the string we want.
  //I had program crashes with the following method:
  //pch[r_ostrstream.pcount()] = '\0' ;
  //r_ostrstream.ends();
  r_ostrstream.put('\0'); //the same as "ends()" does.
  char *pch = r_ostrstream.str() ;
    #ifdef wxUSE_WCHAR_T
      std::string str(pch) ;
      std::wstring wstr(str.begin(),str.end() ) ;
      wxString wxstr( wstr) ;
    #else
      wxString wxstr(( const unsigned char * ) pch ) :
    #endif
  //r_ostrstream.flush();
  //To not show too many dialogs that the timer would bring up.
  if( m_bConfirmedYet )
  {
    m_bConfirmedYet = false ;
    int nReturn = ::wxMessageBox(//pch
      wxstr ,_T("Message"),wxCANCEL|wxOK);
    if( nReturn == wxCANCEL )
      bReturn = false ;
    m_bConfirmedYet = true ;
  }
  //return true;
  DEBUG("App::Confirm--end\n");
  return bReturn ;
}

void wxPumaStateCtrlApp::CurrenCPUfreqAndVoltageUpdated()
{
  //Only when true the cross is drawn.
  mp_frame->m_bDrawFreqAndVoltagePointForCurrCoreSettings = true ;
  //Force redraw of the client area.
  mp_frame->//ReDraw() ;
    Refresh() ;
}

//http://docs.wxwidgets.org/stable/wx_wxappoverview.html:
//"Another aspect of the application shutdown is OnExit which is called when 
//the application exits but before wxWidgets cleans up its internal 
//structures. You should delete all wxWidgets object that you created by 
//the time OnExit finishes. 
//In particular, do not destroy them from application class' destructor!"
int wxPumaStateCtrlApp::OnExit()
{
    //Release heap mem.
#ifdef COMPILE_WITH_CPU_SCALING
    //if(mp_wxdynfreqscalingtimer)
    //  delete mp_wxdynfreqscalingtimer ;
#endif //#ifdef COMPILE_WITH_CPU_SCALING
  //Release dynamically allocated memory (inside OnInit() ) :
  #ifdef _WINDOWS
  delete mp_winring0dynlinked ;
  #endif
  if( mp_i_cpuaccess )
    delete mp_i_cpuaccess ;
  delete [] m_arartchCmdLineArgument ;
  //delete mp_frame ;
  //delete mp_pstatectrl ;
  if( mp_cpucontroller )
    delete mp_cpucontroller ;
  if( mp_dynfreqscalingaccess )
    delete mp_dynfreqscalingaccess ;
  if( mp_modelData )
    delete mp_modelData ;

#ifdef COMPILE_WITH_SHARED_MEMORY
  if( mp_voidMappedViewStartingAddress )
    ::UnmapViewOfFile(mp_voidMappedViewStartingAddress);
  if( m_handleMapFile != NULL )
    ::CloseHandle(m_handleMapFile);
#endif //#ifdef COMPILE_WITH_SHARED_MEMORY
  return 0;
}

bool wxPumaStateCtrlApp::OnInit()
{
  //m_cpucoreusagegetteriwbemservices.Init() ;
  //CPUcoreUsageGetterIWbemServices cpucoreusagegetteriwbemservices ;

#ifdef COMPILE_WITH_SHARED_MEMORY
  mp_voidMappedViewStartingAddress = NULL ;
  m_handleMapFile = NULL ;
  mp_modelData = NULL ;
  m_stdstrSharedMemoryName = "CPUcontrolService" ;
#endif //#ifdef COMPILE_WITH_SHARED_MEMORY
  m_stdtstrProgramName = _T("X86_info_and_control") ;

  m_arartchCmdLineArgument = new char * [NUMBER_OF_IMPLICITE_PROGRAM_ARGUMENTS];

  if(m_arartchCmdLineArgument)
  {
    //ISpecificController
    //MyFrame * p_frame ;
    //Intitialise to be valid.
    m_arartchCmdLineArgument[ 0 ] = "" ;
    m_arartchCmdLineArgument[ NUMBER_OF_IMPLICITE_PROGRAM_ARGUMENTS - 1 ] = 
        //"-config=config.xml" ;
        "-config=GriffinControl_config.xml" ;

    mp_userinterface = //p_frame ;
      this ;
#ifdef COMPILE_WITH_SHARED_MEMORY
    bool bSharedMemOk = false ;
    m_handleMapFile = OpenFileMapping(
      FILE_MAP_ALL_ACCESS,   // read/write access
      FALSE,                 // do not inherit the name
      m_stdstrSharedMemoryName.c_str()  // name of mapping object 
      );
 
    if ( m_handleMapFile == NULL) 
    { 
      DWORD dwError = ::GetLastError() ;
      LOG("unable to open shared memory: \""
        << ::LocalLanguageMessageFromErrorCode(
          dwError
          )             
        << "\" (error code: " << dwError << ")" 
        );
      //return 1;
    }
    else
    {
      mp_voidMappedViewStartingAddress = MapViewOfFile(
        m_handleMapFile ,   // handle to map object
        FILE_MAP_ALL_ACCESS, // read/write permission
        0,                   
        0,                   
        sizeof(Model) 
        );
	  LOGN("Size of attributes:" << sizeof(Model) ) ;
   
      if ( mp_voidMappedViewStartingAddress == NULL) 
      { 
        DWORD dwError = ::GetLastError() ;
        LOG("Could not map view of file : \"" << 
          ::LocalLanguageMessageFromErrorCode(
            dwError
            )             
          << "\" (error code: " << dwError << ")" 
          );
	      CloseHandle(m_handleMapFile);
        //return 1 ;
      }
      else
      {
        bSharedMemOk = true ;
        mp_modelData = (Model*) mp_voidMappedViewStartingAddress ;
		LOGN("Address of attributes:" << mp_modelData ) ;
      }
    }
    if( ! bSharedMemOk )
#endif //COMPILE_WITH_SHARED_MEMORY
      mp_modelData = new Model() ;
#ifdef COMPILE_WITH_NAMED_WINDOWS_PIPE
  	m_ipcclient.Init() ;
#endif
    if( mp_modelData )
    {
      try //catch CPUaccessexception
      {
    #ifdef _WINDOWS
      //WinRing0dynLinked winring0dynlinked(p_frame) ;
      //If allocated statically within this block / method the object 
      //gets invalid after leaving the block where it was declared.
      mp_winring0dynlinked = new WinRing0dynLinked(//p_frame
        this ) ;
      m_maincontroller.SetCPUaccess(mp_winring0dynlinked) ;
    #else
      //m_maincontroller.SetCPUaccess(NULL) ;
      //m_MSRdeviceFile.SetUserInterface(this) ;
      mp_i_cpuaccess = new MSRdeviceFile(this) ;
      //m_maincontroller.SetCPUaccess(&m_MSRdeviceFile) ;
    #endif
      m_maincontroller.SetCPUaccess( mp_i_cpuaccess );
      m_maincontroller.Init( //m_modelData
        * mp_modelData, this );
      //m_winring0dynlinked.SetUserInterface(p_frame);
      //MyServiceStart ( NUMBER_OF_IMPLICITE_PROGRAM_ARGUMENTS, argv) ;

      //new WinRing0dynLinked(p_frame) ;

      ////The controller must be created before the main frame because 
      ////its view depends of values retrieved from the controller
      ////(e.g. for every core a single menu)
      //mp_pstatectrl = new GriffinController(
      //  //0, 
      //  NUMBER_OF_IMPLICITE_PROGRAM_ARGUMENTS,
      //  //NULL,
      //  m_arartchCmdLineArgument,
      //  //UserInterface()
      //  //&PumaStateCtrlDlg()
      //  //p_frame, 
      //  this ,
      //  m_modelData,
      //  //&winring0dynlinked 
      //  mp_winring0dynlinked,
      //  //&m_winring0dynlinked,
      //  & m_calculationthread,
      //  //m_dynfreqscalingaccess
      //  m_powerprofdynlinked
      //  ) ;
      //mp_cpucontroller = //CPUcontrollerFactory::
        m_maincontroller.
        //Creates e.g. an AMD Griffin oder Intel Pentium M controller
        CreateCPUcontrollerAndUsageGetter( 
          mp_cpucontroller 
          , mp_cpucoreusagegetter
          ) ;
      mp_cpucontroller->SetCmdLineArgs(
        NUMBER_OF_IMPLICITE_PROGRAM_ARGUMENTS,
        m_arartchCmdLineArgument ) ;
      mp_cpucontroller->SetUserInterface(this) ;
      //Set the CPU access BEFORE getting number of CPU cores in
      //SetModelData(...) .
      #ifdef _WINDOWS
      mp_cpucontroller->SetCPUaccess(mp_winring0dynlinked) ;
      #else
      //mp_cpucontroller->SetCPUaccess(NULL);
      //mp_cpucontroller->SetCPUaccess( & m_MSRdeviceFile) ;
      #endif
      mp_cpucontroller->SetCPUaccess( mp_i_cpuaccess ) ;
      mp_cpucontroller->SetModelData( //& m_modelData
         mp_modelData ) ;
      if( mp_cpucontroller )
        //Needed for drawing the voltage-frequency curves.
        mp_cpucontroller->GetMaximumFrequencyInMHz() ;
      #ifdef _WINDOWS
      mp_cpucontroller->SetCalculationThread(& m_calculationthread) ;
      #else
      mp_cpucontroller->SetCalculationThread(NULL) ;
      #endif
      //mp_cpucontroller->SetOtherDVFSaccess(& m_powerprofdynlinked) ;
      //DWORD dwMajor = 0, dwMinor ;
      //GetWindowsVersion(dwMajor, dwMinor ) ;
      //if( dwMajor >= 6 //&& dwMinor >= 1 
      //  ) 
      //{
      //  mp_dynfreqscalingaccess = new PowerProfDynLinked() ;
      //  //mp_cpucontroller->SetOtherDVFSaccess( mp_dynfreqscalingaccess ) ;
      //}
      //else
      //{
      //  mp_dynfreqscalingaccess = new PowerProfUntilWin6DynLinked() ;
      //}
      #ifdef _WINDOWS
      mp_dynfreqscalingaccess = new PowerProfDynLinked( m_stdtstrProgramName ) ;
      mp_cpucontroller->SetOtherDVFSaccess( mp_dynfreqscalingaccess ) ;
      #else
      mp_dynfreqscalingaccess = NULL ;
      mp_cpucontroller->SetOtherDVFSaccess( NULL ) ;
      #endif
      //m_modelData.SetGriffinController(mp_pstatectrl) ;
      //m_modelData.SetCPUcontroller( mp_cpucontroller);
      mp_modelData->SetCPUcontroller( mp_cpucontroller);

      //Gets the data from the CPU and sets the info into the model data
      //(important step for drawing overvolt prot curve)
      //mp_pstatectrl->GetMaximumFrequencyInMHz() ;
      mp_cpucontroller->GetMaximumFrequencyInMHz() ;

      ////The user interface must be created before the controller because
      ////it should show error messages because of e.g. missing privileges.
      //p_frame = new MyFrame( 
      mp_frame = new MainFrame(
        _T(//"wxPumaStateCtrlDlg"
          "GriffinControl GUI" )
        , 
        wxPoint(50,50), 
        wxSize(450,340)
        //,mp_pstatectrl
        , mp_cpucontroller
        //, & m_modelData.m_cpucoredata
        , & mp_modelData->m_cpucoredata
        );
      //p_frame->Show(TRUE);
      //SetTopWindow(p_frame);
      mp_frame->Show(TRUE);
      //http://docs.wxwidgets.org/stable/wx_wxappoverview.html:
      //"You call wxApp::SetTopWindow to let wxWidgets know about the top window."
      SetTopWindow(mp_frame);
    #ifdef _WINDOWS
      //m_calculationthread.SetPumaStateCtrl(mp_pstatectrl);
      m_calculationthread.SetCPUcontroller(mp_cpucontroller);
    #endif
      //p_frame->SetPumaStateController(mp_pstatectrl);
      //mp_frame->SetPumaStateController(mp_pstatectrl);
      mp_frame->SetCPUcontroller(mp_cpucontroller) ;
      
      //if( mp_pstatectrl->InitWinRing0() )
      //if( gp_pstatectrl->InitWinRing0() )
      {
        //char * archCPUID ;
        std::string strCPUID ;
        ////F3x190 = Downcore Control Register
        //pstatectrl.SetCPUMiscControlDWORD(
        //  //F3x190 Downcore Control Register
        //  0x190,) ;
        //if( //mp_pstatectrl->GetProcessorNameByCPUID(//archCPUID
        //  mp_cpucontroller->GetProcessorNameByCPUID(
        //    strCPUID ) )
        //{
        //  //std::string strCPUID(archCPUID) ;
        //  //m_model.m_parchCPUID = parchCPUID ;
        //  //mp_pstatectrl->m_model.m_strProcessorName = //new std::string(strCPUID) ;
        //  m_modelData.m_strProcessorName = //new std::string(strCPUID) ;
        //    strCPUID ;
        //}
        
        DEBUG("initialization of dialog--after get processor name\n");

        //from PDF: "Basic Performance Measurements for AMD Athlon� 64,
        //  AMD Opteron� and AMD Phenom� Processors":
        //chapter "4.2.1. Instructions per cycle (IPC)." 
        //  "Computation of IPC requires collection of only two basic events:
        //    0x76 N/A CPU_clocks CPU Clocks Not Halted
        //    0xC0 N/A Ret_instructions Retired Instructions
        //"
        //mp_pstatectrl->AccuratelyStartPerformanceCounting( 
        //  1 , //aff. mask
        //  0, 
        //  PERFORMANCE_EVENT_SELECT_RETIRED_INSTRUCTIONS 
        //  , false //InvertCounterMask
        //  ) ;
        //InitCpuUsage() ;
  //    mp_pstatectrl->PerformanceEventSelectRegisterWrite(
  //      0 ,
  //      //PERFORMANCE_EVENT_SELECT_RETIRED_INSTRUCTIONS ,
  //      PERFORMANCE_EVENT_SELECT_RETIRED_MICRO_OPS ,
  //      //0x0C1 ,
  ////      0x076 ,
  //      //byCounterMask: 00h: The corresponding PERF_CTR[3:0] register is incremented by the number of events
  //      //occurring in a clock cycle. Maximum number of events in one cycle is 3.
  //      0,
  //      //bInvertCounterMask
  //      0,
  //      //bEnablePerformanceCounter
  //      true,
  //      //bEnableAPICinterrupt
  //      0,
  //      //bEdgeDetect
  //      0,
  //      //bOSmode
  //      0,
  //      //bUserMode
  //      0,
  //     //byEventQualification
  //      0
  //      ) ;
        //mp_pstatectrl->AccuratelyStartPerformanceCounting( 
        //  1 , //aff. mask
        //  1, 
        //  //CPU_CLOCKS_NOT_HALTED 
        //  //PERFORMANCE_EVENT_SELECT_DISPATCHED_FPU_OPERATIONS
        //  PERFORMANCE_EVENT_SELECT_CYCLES_IN_WHICH_THE_FPU_IS_EMPTY
        //  //"[...] Invert this (MSRC001_00[03:00][Invert]=1) to count cycles" 
        //  //in which at least one FPU operation is present in the FPU."
        //  , true //InvertCounterMask
        //  ) ;
        //mp_pstatectrl->AccuratelyStartPerformanceCounting( 
        //  2, 
        //  CPU_CLOCKS_NOT_HALTED
        //  , false //InvertCounterMask
        //  ) ;
        //if( //! mp_pstatectrl->m_model.m_bSkipCPUtypeCheck && 
        //  ! m_modelData.m_bSkipCPUtypeCheck && 
        //  ! mp_pstatectrl->IsSupportedCPUModel() )
        //{
        //  mp_userinterface->Confirm("This CPU model is not supported by this program."
        //    "Use \"skip_cpu_type_check\" (see help) if you think it makes "
        //    "sense.\n->exiting");
        //  return FALSE;
        //}
  //#endif //	#ifdef _EMULATE_TURION_X2_ULTRA_ZM82

        DEBUG("initialization of dialog--after a possible CPU type check\n");

        mp_cpucontroller->SetCmdLineArgs(
          NUMBER_OF_IMPLICITE_PROGRAM_ARGUMENTS ,
          m_arartchCmdLineArgument
          ) ;
        BYTE byReturn = //mp_pstatectrl->handleCmdLineArgs() ;
          mp_cpucontroller->HandleCmdLineArgs( ) ;

        DEBUG("initialization of dialog--after handling cmd line args\n");
        //DEBUG("return value of handleCmdLineArgs(): %u\n",(WORD)byReturn);
        LOG("return value of handling command line args: " << (WORD) byReturn << "\n" );

        switch(byReturn)
        {
          case FAILURE:
            mp_userinterface->Confirm("An error occured (a message should have been "
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
            //When (the code in) Init() was executed earlier (inside the 
            //of contructor of "CPUcoreUsageGetterIWbemServices" when it was a member 
            //of this class) there came an error message: "failed to initialize COM"
            //m_cpucoreusagegetteriwbemservices.Init() ;
            //So this class can show up messages to the user.
  #ifdef COMPILE_WITH_IWBEMSERVICES
            m_cpucoreusagegetteriwbemservices.SetUserInterface(mp_userinterface);
  #endif //#ifndef COMPILE_WITH_IWBEMSERVICES

            //if( ! m_cpucoreusagegetteriwbemservices.Init() )

            //http://docs.wxwidgets.org/stable/wx_wxtimer.html#wxtimer:
            //"Note: A timer can only be used from the main thread."
              //mp_wxdynfreqscalingtimer = new wxDynFreqScalingTimer(
              //  //new CPUcoreUsageGetterIWbemServices()
              //  //& cpucoreusagegetteriwbemservices 
              //  & m_cpucoreusagegetteriwbemservices,
              //  mp_pstatectrl
              //  ) ; 
            
            //mp_wxdynfreqscalingtimer->Start(400);

            //TODO remove memory leaks by dyn. alloc.
            //mp_wxDynLinkedCPUcoreUsageGetter = new //wxDynLinkedCPUcoreUsageGetter(
            //mp_icpucoreusagegetter = new
            //  //std::string("CPUcoreUsageGetterNTQSI_Wintop.Vxd.dll")
            //  //  , std::string("GetCPUusageOfCore")
            //  //  , m_modelData.m_cpucoredata
            //  //  ) ;
            //  CPUcoreUsageGetterNTQSI_WintopVxd(
            //    m_modelData.m_cpucoredata) ;
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

            //m_dynfreqscalingthread.SetMembers(
            //  &m_cpucoreusagegetteriwbemservices,mp_pstatectrl);
            //m_dynfreqscalingthread.Start();
            //DEBUG("After starting timer\n");

            //if( m_modelData.m_cpucoredata.m_bEnableDVFS )
            //{
            //  if( mp_cpucontroller->mp_dynfreqscalingaccess->OtherDVFSisEnabled() 
            //    )
            //    mp_cpucontroller->DisableFrequencyScalingByOS();
            //  PerCPUcoreAttributes * p_percpucoreattributes = 
            //    & m_modelData.m_cpucoredata.
            //    m_arp_percpucoreattributes[ //p_atts->m_byCoreID 
            //    0 ] ;
            //  //DynFreqScalingThread * p_dynfreqscalingthread
            //  if ( ! p_percpucoreattributes->mp_dynfreqscalingthread )
            //  {
            //    if( ! //mp_pumastatectrl->mp_dynfreqscalingaccess->OtherDVFSisEnabled() 
            //        mp_cpucontroller->mp_dynfreqscalingaccess->OtherDVFSisEnabled() 
            //      )
            //    {
            //      //p_percpucoreattributes->mp_dynfreqscalingthread 
            //      p_percpucoreattributes->SetCPUcontroller( mp_cpucontroller ) ;
            //      p_percpucoreattributes->CreateDynFreqScalingThread( 
            //        mp_cpucoreusagegetter
            //        ) ;
            //    }
            //  }
            //}
//ifdef COMPILE_WITH_SHARED_MEMORY the SERVICE should do the DVFS
#ifndef COMPILE_WITH_SHARED_MEMORY
            mp_cpucontroller->EnableOwnDVFS() ;
#endif
            DEBUG("After starting CPU freq thread\n");

            //DEBUG("Address of AMD family 17 controller:%x\n",mp_pstatectrl);
            //LOG("Address of AMD family 17 controller:" << mp_pstatectrl << "\n" );
  //          mp_pstatectrl->DisableFrequencyScalingByOS();
  #endif //#ifdef COMPILE_WITH_CPU_SCALING
        }
  //#ifndef _EMULATE_TURION_X2_ULTRA_ZM82
  //      mp_pstatectrl->ApplyAllPStates() ;
  //#endif //#ifndef _EMULATE_TURION_X2_ULTRA_ZM82
      }
  //    else
  //    {
  //      mp_userinterface->Confirm("WinRing0 not correctly initialized->exiting");
  //      //p_frame->Close();
  //      mp_frame->Close();
  //    }

      //const wxCmdLineParser& parser
      //  size_t count = parser.GetParamCount();
      //  for ( size_t param = 0; param < count; param++ )
      //  {
      //      s << parser.GetParam(param) << ' ';
      //  }
      }
      catch(//ReadMSRexception 
          CPUaccessException e)
      {
          //this->Confirm("Reading from MSR failed kjj");
          //mp_frame->Confirm("CPU access error.\n(insuff rights?)");
          return FALSE ;
      }
    }// if (mp_modelData)
  }
  else
    return FALSE ;
  return TRUE;
} 

void wxPumaStateCtrlApp::outputAllPstates(unsigned char byCurrentP_state, int & vid)
{

}
//int main(int argc, char **argv)
//{
//  wxPumaStateCtrlApp myapp;
//  ::wxMessageBox("jjj");
//
//  //myapp.OnInit();
//}
