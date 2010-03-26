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
//#include <Windows/GetWindowsVersion.h>
//#include <Windows/PowerProf/PowerProfUntilWin6DynLinked.hpp>
//#include <Windows/LocalLanguageMessageFromErrorCode.h>
#include <Controller/I_CPUcontroller.hpp>
#include <Controller/tchar_conversion.h> //for GetCharPointer(...)
#include <Controller/X86InfoAndControlExceptions.hpp> //for VoltageSafetyException
#include <ModelData/ModelData.hpp>
#include <wxWidgets/UserInterface/MainFrame.hpp>
//#include <wxWidgets/wxStringHelper.h>
#include <wxWidgets/Controller/wxStringHelper.h> //getwxString(...)

#include "DynFreqScalingThread.hpp"
#ifdef _WINDOWS
  #include <Windows/PowerProf/PowerProfDynLinked.hpp>
  //#include <Windows/WinRing0dynlinked.hpp>
  //#include <Windows/WinRing0/WinRing0_1_3LoadTimeDynLinked.hpp>
  #include <Windows/WinRing0/WinRing0_1_3RunTimeDynLinked.hpp>
#else
  #include <Linux/MSRdeviceFile.h>
#endif
#include <strstream> //ostrstream
#include <string> //
//#include "../Windows/DynFreqScalingThread.hpp"
//#include "CpuUsage.h"
//#include "Controller/RunAsService.h" //for MyServiceStart etc.

FILE * fileDebug ; //for debug logging.
//This global (important for using preprocessor macros) object is used for 
//easy logging.
Logger g_logger ;
CPUcontrolBase * gp_cpucontrolbase ;

//Erzeugt ein wxAppConsole-Object auf dem Heap.
IMPLEMENT_APP(wxX86InfoAndControlApp)

bool wxX86InfoAndControlApp::Confirm(const std::string & str)
{
  //::AfxMessageBox(str.c_str());

  //To not show too many dialogs that the timer would bring up.
  if( m_bConfirmedYet )
  {
    m_bConfirmedYet = false ;
    #ifdef wxUSE_WCHAR_T
      std::wstring wstr(str.begin(), str.end() ) ;
      wxString wxstr( wstr.c_str() ) ;
    #else
      wxString wxstr(( const unsigned char * ) str.c_str() ) :
    #endif
    ::wxMessageBox( 
      #ifdef _DEBUG_
      wxT("gg"), wxT("bla"),wxOK
      #else
      wxstr
      #endif
      , m_stdtstrProgramName
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

bool wxX86InfoAndControlApp::Confirm(std::ostrstream & r_ostrstream
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
  char * pch = r_ostrstream.str() ;
    //#ifdef wxUSE_WCHAR_T
    //  std::string str(pch) ;
    //  std::wstring wstr(str.begin(),str.end() ) ;
    //  wxString wxstr( wstr) ;
    //#else
    //  wxString wxstr(( const unsigned char * ) pch ) :
    //#endif
  std::string stdstr(pch) ;
  wxString wxstr( getwxString( stdstr ) ) ;
  //r_ostrstream.flush();
  //To not show too many dialogs that the timer would bring up.
  if( m_bConfirmedYet )
  {
    m_bConfirmedYet = false ;
    int nReturn = ::wxMessageBox(//pch
      wxstr
      //, _T("Message")
      , m_stdtstrProgramName
      , wxCANCEL | wxOK );
    if( nReturn == wxCANCEL )
      bReturn = false ;
    m_bConfirmedYet = true ;
  }
  //return true;
  DEBUG("App::Confirm--end\n");
  return bReturn ;
}

void wxX86InfoAndControlApp::CPUcontrollerChanged()
{
  //May be NULL.
  if( mp_cpucontroller )
  {
    mp_cpucontroller->SetUserInterface(this) ;
    //Set the CPU access BEFORE getting number of CPU cores in
    //SetModelData(...) .
    //#ifdef _WINDOWS
    //mp_i_cpucontroller->SetCPUaccess( //mp_winring0dynlinked
    //  mp_i_cpuaccess ) ;
    //#else
    ////mp_i_cpucontroller->SetCPUaccess(NULL);
    ////mp_i_cpucontroller->SetCPUaccess( & m_MSRdeviceFile) ;
    //#endif
    
    mp_cpucontroller->SetCPUaccess( mp_i_cpuaccess ) ;
    mp_cpucontroller->SetModelData( //& m_modelData
       mp_modelData ) ;
    //Needed for drawing the voltage-frequency curves.
    mp_cpucontroller->GetMaximumFrequencyInMHz() ;
    #ifdef _WINDOWS
    mp_cpucontroller->SetCalculationThread(& m_calculationthread) ;
    #else
    mp_i_cpucontroller->SetCalculationThread(NULL) ;
    #endif
    
    mp_cpucontroller->SetOtherDVFSaccess( mp_dynfreqscalingaccess ) ;

    //Gets the data from the CPU and sets the info into the model data
    //(important step for drawing overvolt prot curve)
    //mp_pstatectrl->GetMaximumFrequencyInMHz() ;
    mp_cpucontroller->GetMaximumFrequencyInMHz() ;
  }
  //m_modelData.SetCPUcontroller( mp_i_cpucontroller);
  mp_modelData->SetCPUcontroller( mp_cpucontroller);
  #ifdef _WINDOWS
    m_calculationthread.SetCPUcontroller(mp_cpucontroller);
  #endif
  //At the 1st call of this function mp_frame is NULL.
  if( mp_frame )
    mp_frame->SetCPUcontroller(mp_cpucontroller) ;
}

void wxX86InfoAndControlApp::CurrenCPUfreqAndVoltageUpdated()
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
int wxX86InfoAndControlApp::OnExit()
{
    //Release heap mem.
#ifdef COMPILE_WITH_CPU_SCALING
    //if(mp_wxdynfreqscalingtimer)
    //  delete mp_wxdynfreqscalingtimer ;
#endif //#ifdef COMPILE_WITH_CPU_SCALING
  //Release dynamically allocated memory (inside OnInit() ) :
  //#ifdef _WINDOWS
  //delete mp_winring0dynlinked ;
  //#endif
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

bool wxX86InfoAndControlApp::OnInit()
{
  //Init to NULL for "CPUcontrollerChanged()"
  mp_frame = NULL ;
  gp_cpucontrolbase = this ;

#ifdef COMPILE_WITH_SHARED_MEMORY
  mp_voidMappedViewStartingAddress = NULL ;
  m_handleMapFile = NULL ;
  mp_modelData = NULL ;
  m_stdstrSharedMemoryName = "CPUcontrolService" ;
#endif //#ifdef COMPILE_WITH_SHARED_MEMORY
  m_stdtstrProgramName = _T("X86_info_and_control") ;

  m_arartchCmdLineArgument = new TCHAR * [NUMBER_OF_IMPLICITE_PROGRAM_ARGUMENTS];
  mp_modelData = new Model() ;

  //If allocation succeeded.
  if( m_arartchCmdLineArgument && mp_modelData )
  {
    //ISpecificController
    //MyFrame * p_frame ;
    std::tstring stdtstrLogFilePath ;
    if( argc > 0 )
    {
      std::tstring tstrArg0(argv[0]) ;
      stdtstrLogFilePath = std::tstring( //Getstdtstring(argv) +
        tstrArg0 +
        _T("_log.txt") ) ;
    }
    else
      stdtstrLogFilePath = std::tstring( mp_modelData->m_stdtstrProgramName +
        _T("_log.txt") ) ;
    //Maybe it's better to use a file name for the log file that is derived 
    //from THIS executable's file name: e.g. so different log files for the 
    //x86I&C service and the x86I&C GUI are possible.
    g_logger.OpenFile( stdtstrLogFilePath ) ;
    //Intitialise to be valid.
    m_arartchCmdLineArgument[ 0 ] = _T("") ;
    m_arartchCmdLineArgument[ NUMBER_OF_IMPLICITE_PROGRAM_ARGUMENTS - 1 ] = 
      //"-config=config.xml" ;
      //_T("-config=GriffinControl_config.xml") ;
      (TCHAR * ) (mp_modelData->m_stdtstrProgramName + _T("_config.xml") ).c_str() ;

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
#ifdef COMPILE_WITH_NAMED_WINDOWS_PIPE
  	m_ipcclient.Init() ;
#endif
    //if( mp_modelData )
    {
      try //catch CPUaccessexception
      {
    #ifdef _WINDOWS
      //WinRing0dynLinked winring0dynlinked(p_frame) ;
      //If allocated statically within this block / method the object 
      //gets invalid after leaving the block where it was declared.
      //mp_winring0dynlinked 
      //mp_i_cpuaccess = new WinRing0dynLinked(//p_frame
#ifdef _MSC_VER_ //possible because the import library is for MSVC
      mp_i_cpuaccess = new WinRing0_1_3LoadTimeDynLinked(
        this ) ;
#else //because no import library is available
      mp_i_cpuaccess = new WinRing0_1_3RunTimeDynLinked(
        this ) ;
#endif
      //m_maincontroller.SetCPUaccess( //mp_winring0dynlinked
      //  mp_i_cpuaccess ) ;
    #else
      //m_maincontroller.SetCPUaccess(NULL) ;
      //m_MSRdeviceFile.SetUserInterface(this) ;
      mp_i_cpuaccess = new MSRdeviceFile(this) ;
      //m_maincontroller.SetCPUaccess(&m_MSRdeviceFile) ;
    #endif
			//the main controller needs CPUID (I_CPUaccess class ) access in order to
			//retrieve the CPU by model, family etc.
      m_maincontroller.SetCPUaccess( mp_i_cpuaccess );
      mp_i_cpuaccess->mp_model = mp_modelData ;
      m_maincontroller.Init( //m_modelData
        * mp_modelData, this );
      m_maincontroller.SetAttributeData( mp_modelData ) ;
      //m_winring0dynlinked.SetUserInterface(p_frame);
      //MyServiceStart ( NUMBER_OF_IMPLICITE_PROGRAM_ARGUMENTS, argv) ;
     
      #ifdef _WINDOWS
      mp_dynfreqscalingaccess = new PowerProfDynLinked( m_stdtstrProgramName ) ;
      #else
      mp_dynfreqscalingaccess = NULL ;
      #endif
      //mp_i_cpucontroller = //CPUcontrollerFactory::
      if(
        m_maincontroller.
        //Creates e.g. an AMD Griffin oder Intel Pentium M controller
        CreateCPUcontrollerAndUsageGetter( 
          mp_cpucontroller 
          , mp_cpucoreusagegetter
          ) 
        )
      {
  			//Now we have created the CPU controller. It knows how many cores it has.
  		  //The core count is an important information e.g. for the Linux MSR device
  		  //file access.
  			mp_i_cpuaccess->InitPerCPUcoreAccess( mp_cpucontroller->
          GetNumberOfCPUcores() ) ;
        mp_cpucontroller->SetCmdLineArgs(
          NUMBER_OF_IMPLICITE_PROGRAM_ARGUMENTS,
          m_arartchCmdLineArgument ) ;
        
        //#ifdef _WINDOWS
        //mp_dynfreqscalingaccess = new PowerProfDynLinked( m_stdtstrProgramName ) ;
        //#else
        //mp_dynfreqscalingaccess = NULL ;
        //mp_i_cpucontroller->SetOtherDVFSaccess( NULL ) ;
        //#endif

        CPUcontrollerChanged() ;
      }

      ////The user interface must be created before the controller because
      ////it should show error messages because of e.g. missing privileges.
      //p_frame = new MyFrame( 
      mp_frame = new MainFrame(
        //_T(PROGRAM_NAME)
        //m_stdtstrProgramName
        mp_modelData->m_stdtstrProgramName +_T(" GUI")
        , 
        wxPoint(50,50), 
        wxSize(450,340)
        //,mp_pstatectrl
        , mp_cpucontroller
        //, & m_modelData.m_cpucoredata
        //, & mp_modelData->m_cpucoredata
        , mp_modelData
        , this
        );
      //p_frame->Show(TRUE);
      //SetTopWindow(p_frame);
      mp_frame->Show(TRUE);
      //http://docs.wxwidgets.org/stable/wx_wxappoverview.html:
      //"You call wxApp::SetTopWindow to let wxWidgets know about the top window."
      SetTopWindow(mp_frame);
    //#ifdef _WINDOWS
    //  m_calculationthread.SetCPUcontroller(mp_i_cpucontroller);
    //#endif
      mp_frame->SetCPUcontroller(mp_cpucontroller) ;
      
      if( mp_cpucontroller )
      {
        //char * archCPUID ;
        std::string strCPUID ;
        
        DEBUG("initialization of dialog--after get processor name\n");

        //InitCpuUsage() ;
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
//          default:
  //          DEBUG("Before starting timer\n");
            //mp_wxdynfreqscalingtimer->mp_pumastatectrl = mp_pstatectrl ;
//            DWORD dwValue = 0 ;
            //TODO read values from CPU at first because the other values should not 
            //be affected.

  #ifdef COMPILE_WITH_CPU_SCALING

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
            //  if( mp_i_cpucontroller->mp_dynfreqscalingaccess->OtherDVFSisEnabled() 
            //    )
            //    mp_i_cpucontroller->DisableFrequencyScalingByOS();
            //  PerCPUcoreAttributes * p_percpucoreattributes = 
            //    & m_modelData.m_cpucoredata.
            //    m_arp_percpucoreattributes[ //p_atts->m_byCoreID 
            //    0 ] ;
            //  //DynFreqScalingThread * p_dynfreqscalingthread
            //  if ( ! p_percpucoreattributes->mp_dynfreqscalingthread )
            //  {
            //    if( ! //mp_pumastatectrl->mp_dynfreqscalingaccess->OtherDVFSisEnabled() 
            //        mp_i_cpucontroller->mp_dynfreqscalingaccess->OtherDVFSisEnabled() 
            //      )
            //    {
            //      //p_percpucoreattributes->mp_dynfreqscalingthread 
            //      p_percpucoreattributes->SetCPUcontroller( mp_i_cpucontroller ) ;
            //      p_percpucoreattributes->CreateDynFreqScalingThread( 
            //        mp_cpucoreusagegetter
            //        ) ;
            //    }
            //  }
            //}
//ifdef COMPILE_WITH_SHARED_MEMORY the SERVICE should do the DVFS
#ifndef COMPILE_WITH_SHARED_MEMORY
            mp_i_cpucontroller->EnableOwnDVFS() ;
#endif
            DEBUG("After starting CPU freq thread\n");

            //DEBUG("Address of AMD family 17 controller:%x\n",mp_pstatectrl);
            //LOG("Address of AMD family 17 controller:" << mp_pstatectrl << "\n" );
  //          mp_pstatectrl->DisableFrequencyScalingByOS();
  #endif //#ifdef COMPILE_WITH_CPU_SCALING
        }
      } //if( mp_i_cpucontroller )
      //else //CreateCPUcontrollerAndUsageGetter(...) failed
      //  mp_userinterface->Confirm("got no CPU controller and/ or CPU usage getter");
      }
      catch(//ReadMSRexception 
          CPUaccessException e)
      {
          //this->Confirm("Reading from MSR failed kjj");
          //mp_frame->Confirm("CPU access error.\n(insuff rights?)");
          return FALSE ;
      }
      catch( VoltageSafetyException e )
      {
        Confirm( //GetCharPointer( ( e.m_stdtstrMessage + _T("\n->Exiting") ).
          //c_str() ) 
          GetStdString( e.m_stdtstrMessage + _T("\n->Exiting") )
          ) ;
        return FALSE ;
      }
    }// if (mp_modelData)
  }
  else
    return FALSE ;
  return TRUE;
} 

void wxX86InfoAndControlApp::outputAllPstates(unsigned char byCurrentP_state, int & vid)
{

}

void wxX86InfoAndControlApp::RedrawEverything()
{
  mp_frame->RedrawEverything() ;
}

void wxX86InfoAndControlApp::SetCPUcontroller( 
  I_CPUcontroller * p_cpucontrollerNew )
{
  if( p_cpucontrollerNew )
  {
    //Avoid porgram crash because of the mainframe tries to get the current
    //performance state.
    mp_frame->DenyCPUcontrollerAccess() ;
    if( mp_cpucontroller )
      //Release memory.
      delete mp_cpucontroller ;
    mp_cpucontroller = p_cpucontrollerNew ;
    //May be NULL at startup.
    if( mp_cpucoreusagegetter )
      mp_cpucoreusagegetter->SetCPUcontroller( p_cpucontrollerNew ) ;
    //mp_i_cpucontroller->SetModelData( //& m_modelData
    //  mp_modelData ) ;
    CPUcontrollerChanged() ;
    mp_frame->AllowCPUcontrollerAccess() ;
    //Force an update of the canvas.
    mp_frame->RedrawEverything() ;
  }
}

void wxX86InfoAndControlApp::DeleteCPUcontroller( )
{
  //if( p_cpucontroller )
  //{
    //Avoid porgram crash because of the mainframe tries to get the current
    //performance state.
    mp_frame->DenyCPUcontrollerAccess() ;
    if( mp_cpucontroller )
      //Release memory.
      delete mp_cpucontroller ;
    mp_cpucontroller = NULL ;
    //May be NULL at startup.
    if( mp_cpucoreusagegetter )
      mp_cpucoreusagegetter->SetCPUcontroller( NULL ) ;
    //mp_i_cpucontroller->SetModelData( //& m_modelData
    //  mp_modelData ) ;
    CPUcontrollerChanged() ;
    mp_frame->AllowCPUcontrollerAccess() ;
    //Force an update of the canvas.
    mp_frame->RedrawEverything() ;
  //}
}

//int main(int argc, char **argv)
//{
//  wxPumaStateCtrlApp myapp;
//  ::wxMessageBox("jjj");
//
//  //myapp.OnInit();
//}
