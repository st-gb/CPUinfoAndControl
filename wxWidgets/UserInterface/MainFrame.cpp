// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "MainFrame.hpp"

//wxWidgets include files
#include <wx/defs.h> //for wxBG_STYLE_CUSTOM
#include <wx/dcbuffer.h> //for class wxBufferedPaintDC
#include "wx/window.h"
#include "wx/frame.h" //for class wxFrame
#include <wx/menu.h> //for class wxMenu, class wxMenuBar
#include <wx/numdlg.h> //for ::wxGetNumberFromUser(...)
#include <wx/stattext.h> //for wxStaticText
#include <wx/wx.h> //for wxMessageBox(...) (,etc.)
#include <wx/icon.h> //for wxIcon
#include <wx/timer.h> //for EVT_TIMER (,...?)
#include <wx/string.h> //for wxString::Format(...)
//#include <wx/menubar.h>

//#include "../Controller/RunAsService.h" //for MyServiceStart etc.
//#include "../AMD_family17.h" //for FIRST_RESERVED_DIVISOR_ID
#include "FreqAndVoltageSettingDlg.hpp"
//#include "../Controller/GriffinController.hpp"
#include <Controller/I_CPUcontrollerAction.hpp>
#include <Controller/ICPUcoreUsageGetter.hpp> //::wxGetApp().mp_cpucoreusagegetter
#include <Controller/CalculationThreadProc.h>
#include <Controller/IPC/I_IPC.hpp> //enum IPCcontrolCodes
#include <Controller/MainController.hpp>
#include <Controller/stdtstr.hpp>
#include <BuildTimeString.h>
#include "ModelData/RegisterData.hpp"
#include <ModelData/HighLoadThreadAttributes.hpp>
#include <ModelData/SpecificCPUcoreActionAttributes.hpp>
#include <wxWidgets/ModelData/wxCPUcoreID.hpp>
#include <wxWidgets/DynFreqScalingThread.hpp>
#include <wxWidgets/UserInterface/DynFreqScalingDlg.hpp>
#include <Xerces/XMLAccess.h>
//#include <Windows/CalculationThread.hpp>
#ifdef _WINDOWS
#include <Windows/ServiceBase.hpp>
#endif
//#include <Windows/LocalLanguageMessageFromErrorCode.h>
//#include "Windows/CPUcoreUsageNTQSI_OO.hpp"
//#include <Windows/CPUcoreUsageNTQSI_OO2.hpp>
#include "wxDynamicDialog.hpp"
#include <wxWidgets/App.hpp> //for wxGetApp() / DECLARE_APP
//#include "Windows/CPUcoreUsageGetterIWbemServices.hpp"
//#include "CpuUsage.h" //Crystal CPU usage getter
#include <set>

class wxObject ;

enum
{
  ID_Quit = 1
  , ID_About
//#ifdef _TEST_PENTIUM_M
  , ID_MSR
//#endif
//#ifdef COMPILE_WITH_VISTA_POWERPROFILE_ACCESS
  , ID_MinAndMaxCPUcoreFreqInPercentOfMaxFreq //,
  , ID_EnableOrDisableOtherDVFS //,
  , ID_ContinueService
  , ID_PauseService
  , ID_StartService
  , ID_StopService
  , ID_UpdateViewInterval
  , ID_SaveAsDefaultPstates
  , ID_FindDifferentPstates
//#endif
  , TIMER_ID
//#ifdef _WINDOWS
#ifdef COMPILE_WITH_SERVICE_CAPABILITY
  , ID_Service
#endif
//  ID_SetPstate0ForCore0,
//  ID_SetPstate1ForCore0,
//  ID_SetPstate2ForCore0,
//  ID_SetPstate3ForCore0,
//  ID_SetPstate0ForCore1,
//  ID_SetPstate1ForCore1,
//  ID_SetPstate2ForCore1,
//  ID_FindLowestOperatingVoltage
#ifdef PRIVATE_RELEASE //hide the other possibilities
  ,ID_IncreaseVoltageForCurrentPstate
#endif //#ifdef PRIVATE_RELEASE //hide the other possibilities
  ,
  ID_LastStaticID
};

BEGIN_EVENT_TABLE(MainFrame, wxFrame)
    //http://www.informit.com/articles/article.aspx?p=405047:
    //[...]another thing you can do to make drawing smoother (particularly 
    //when resizing) is to paint the background in your paint handler, and 
    //not in an erase background handler. All the painting will then be 
    //done in your buffered paint handler, so you don't see the background 
    //being erased before the paint handler is called. Add an empty erase 
    //background handler[...]
    EVT_ERASE_BACKGROUND(MainFrame::OnEraseBackground)
  EVT_MENU(ID_Quit, MainFrame::OnQuit)
  EVT_MENU(ID_About, MainFrame::OnAbout)
//#ifdef _TEST_PENTIUM_M
#ifdef COMPILE_WITH_MSR_EXAMINATION
  EVT_MENU(ID_MSR, MainFrame::OnMSR)
#endif
//#ifdef COMPILE_WITH_VISTA_POWERPROFILE_ACCESS
  EVT_MENU( ID_MinAndMaxCPUcoreFreqInPercentOfMaxFreq ,
    MainFrame::OnDynamicallyCreatedUIcontrol )
  EVT_MENU( ID_EnableOrDisableOtherDVFS ,
    MainFrame::OnOwnDynFreqScaling )
  EVT_MENU( ID_UpdateViewInterval ,
    MainFrame::OnUpdateViewInterval )
  EVT_MENU( ID_SaveAsDefaultPstates ,
    MainFrame::OnSaveAsDefaultPStates )
  EVT_MENU( ID_FindDifferentPstates ,
    MainFrame::OnFindDifferentPstates )
#ifdef COMPILE_WITH_SERVICE_PROCESS_CONTROL
  EVT_MENU( ID_ContinueService ,
    MainFrame::OnContinueService )
  EVT_MENU( ID_PauseService ,
    MainFrame::OnPauseService )
  EVT_MENU( ID_StartService ,
    MainFrame::OnStartService )
  EVT_MENU( ID_StopService ,
    MainFrame::OnStopService )
#endif //#ifdef COMPILE_WITH_SERVICE_PROCESS_CONTROL
//#endif
  //#ifdef _WINDOWS
#ifdef COMPILE_WITH_SERVICE_CAPABILITY
  EVT_MENU(ID_Service, MainFrame::OnStartService)
#endif
  //EVT_MENU(ID_SetPstate0ForBothCores, MyFrame::OnSetPstate0ForBothCores)
  //EVT_MENU(ID_SetPstate1ForBothCores, MyFrame::OnSetPstate1ForBothCores)
  //EVT_MENU(ID_SetPstate2ForBothCores, MyFrame::OnSetPstate2ForBothCores)
  //EVT_MENU(ID_FindLowestOperatingVoltage, MainFrame::OnFindLowestOperatingVoltage)
#ifdef PRIVATE_RELEASE //hide the other possibilities
  EVT_MENU(ID_IncreaseVoltageForCurrentPstate, MainFrame::OnIncreaseVoltageForCurrentPstate)
#endif //#ifdef PRIVATE_RELEASE //hide the other possibilities
#ifdef wxHAS_POWER_EVENTS
  //EVT_POWER_SUSPENDING(MyFrame::OnSuspending)
  //EVT_POWER_SUSPENDED(MyFrame::OnSuspended)
  //EVT_POWER_SUSPEND_CANCEL(MyFrame::OnSuspendCancel)
  EVT_POWER_RESUME(MainFrame::OnResume)
#endif // wxHAS_POWER_EVENTS

  //If no EVT_PAINT macro and Connect(wxEVT_PAINT, 
  //  wxPaintEventHandler(MyFrame::OnPaint));
  // : 100% CPU load.
  EVT_PAINT  (MainFrame::OnPaint)
  EVT_SIZE(MainFrame::OnSize)
  //EVT_TIMER(-1,MainFrame::OnTimerEvent)
  EVT_TIMER(TIMER_ID,MainFrame::OnTimerEvent)
END_EVENT_TABLE()

MainFrame::MainFrame(
  const wxString& title, 
  const wxPoint& pos, 
  const wxSize& size
  //,GriffinController * p_pumastatectrl
  , I_CPUcontroller * p_cpucontroller
  //, CPUcoreData * p_cpucoredata 
  , Model * p_model
  )
  : wxFrame((wxFrame *)NULL, -1, title, pos, size
    //http://docs.wxwidgets.org/2.6/wx_wxwindow.html#wxwindow:
    //"Use this style to force a complete redraw of the window whenever it is resized instead of redrawing just the part of the window affected by resizing. Note that this was the behaviour by default before 2.5.1 release and that if you experience redraw problems with code which previously used to work you may want to try this. Currently this style applies on GTK+ 2 and Windows only, and full repainting is always done on other platforms."
    //kurz: Stil ist notwendig, um das ganze Diagramm neu zu zeichnen
    , wxFULL_REPAINT_ON_RESIZE //| wxCLIP_CHILDREN
        //Necessary for showing a title bar
        | wxDEFAULT_FRAME_STYLE
    )
  //, mp_cpucoredata(p_cpucoredata)
  , mp_cpucoredata( & p_model->m_cpucoredata )
  , mp_model ( p_model )
//, m_bConfirmedYet(true)
  , mp_freqandvoltagesettingdlg(NULL)
  //, mp_pumastatectrl(p_pumastatectrl)
  , mp_cpucontroller ( p_cpucontroller) 
  //Necessary for the timer to run:
  , m_timer(this)
  , mp_wxbitmap(NULL)
  , m_fPreviousCPUusage(0.0f)
  , m_ullHighestDiff(0)
  , m_ullHighestPerformanceEventCounter2Diff(0)
  , m_ullPreviousCPUusage(0)
  , m_ullPreviousPerformanceEventCounter2(0)
  , b_NotFirstTime(false)
  , m_vbAnotherWindowIsActive(false)
  //, m_clocksnothaltedcpucoreusagegetter(0 ,p_pumastatectrl)
  //, m_wxbufferedpaintdcStatic( this ) 
  , mp_wxbufferedpaintdcStatic( NULL) 
  , mp_wxbitmapStatic (NULL)
  , m_dwTimerIntervalInMilliseconds (1000)
  , mp_calculationthread( NULL )
  , m_bRangeBeginningFromMinVoltage ( true )
{
  LOG("begin of main frame creation\n")

  //mp_wxbitmapStatic = new 
  //;
    //http://www.informit.com/articles/article.aspx?p=405047:
  //[...]call SetBackgroundStyle with wxBG_STYLE_CUSTOM to hint to some 
  //systems not to clear the background automatically
  SetBackgroundStyle(wxBG_STYLE_CUSTOM );
  m_bConfirmedYet = true ;
  mp_wxmenubar = new wxMenuBar;

  //mp_pumastatectrl = p_pumastatectrl ;
  wxMenu * p_wxmenuFile = new wxMenu;
  //wxMenu * p_wxmenuCore1 = new wxMenu;
  //wxMenu * p_wxmenuCore0 = new wxMenu;
  wxMenu * p_wxmenuNorthBridge = new wxMenu;
  p_wxmenuFile->Append( ID_About, _T("&About...") );
  p_wxmenuFile->Append( ID_SaveAsDefaultPstates, 
    _T("Save &Performance States...") );
  p_wxmenuFile->AppendSeparator();
  //p_wxmenuFile->Append( ID_Service, _T("Run As Service") );
  p_wxmenuFile->Append( ID_Quit, _T("E&xit") );
//#ifdef COMPILE_WITH_OTHER_DVFS_ACCESS
#ifdef COMPILE_WITH_SERVICE_CONTROL
  CreateServiceMenuItems() ;
#endif
  CreateAndInitMenuItemPointers() ;

  //p_wxmenuCore0->Append( ID_FindLowestOperatingVoltage, 
  //  _T("find lowest operating voltage") );

  //p_wxmenuBothCores->Append( ID_FindLowestOperatingVoltage, _T("find lowest operting voltage") );

//#ifdef PRIVATE_RELEASE //hide the other possibilities
//  p_wxmenuCore0->Append( ID_IncreaseVoltageForCurrentPstate, 
//    _T("increase voltage for current p-state (stabilize machine)") );
//#endif //#ifdef PRIVATE_RELEASE //hide the other possibilities

  //wxMenuBar * mp_wxmenubar = new wxMenuBar;
  //p_wxmenuBar->Append( p_wxmenuFile, _T("&File") );
  //m_wxmenubar.Append( p_wxmenuFile, _T("&File") );
  mp_wxmenubar->Append( p_wxmenuFile, _T("&File") );
  //p_wxmenuBar->Append( p_wxmenuCore0, _T("for core &0") );
  //p_wxmenuBar->Append( p_wxmenuCore1, _T("for core &1") );

  //UpdatePowerSettings(wxPOWER_UNKNOWN, wxBATTERY_UNKNOWN_STATE);

  m_arp_freqandvoltagesettingdlg = new FreqAndVoltageSettingDlg * [
    mp_cpucoredata->m_byNumberOfCPUCores];
  if(m_arp_freqandvoltagesettingdlg)
    //important: init pointers with NULL
    memset(m_arp_freqandvoltagesettingdlg, NULL, 
      sizeof(m_arp_freqandvoltagesettingdlg[0]) *
      mp_cpucoredata->m_byNumberOfCPUCores);
  //TRACE("sizeof: %u\n", sizeof(m_arp_freqandvoltagesettingdlg));
#ifdef _DEBUG
  int i = sizeof(m_arp_freqandvoltagesettingdlg) ;
#endif
  CreateDynamicMenus();
//#ifdef COMPILE_WITH_VISTA_POWERPROFILE_ACCESS
  wxMenu * p_wxmenuExtras = new wxMenu;
//#ifdef COMPILE_WITH_SERVICE_CONTROL
#ifdef COMPILE_WITH_OTHER_DVFS_ACCESS
  mp_wxmenuitemOtherDVFS = p_wxmenuExtras->Append(
    ID_MinAndMaxCPUcoreFreqInPercentOfMaxFreq, 
    //_T("&CPU % min and max.") 
    _T("enable or disable OS's dynamic frequency scaling")
    );
  //If one can not change the power scheme (Windows) etc.
  if( ! mp_cpucontroller->mp_dynfreqscalingaccess->
      ChangeOtherDVFSaccessPossible() )
  {
    mp_wxmenuitemOtherDVFS->Enable(false);
    mp_wxmenuitemOtherDVFS->SetHelp ( wxT("Start e.g. as administrator to gain access") ) ;
    //mp_wxmenuitemOtherDVFS->SetItemLabel (wxT("dd") ) ;
  }
#endif //#ifdef COMPILE_WITH_SERVICE_CONTROL
  p_wxmenuExtras->Append(
    ID_UpdateViewInterval, 
    //_T("&CPU % min and max.") 
    _T("set update view interval")
    );
//#endif //#ifdef COMPILE_WITH_VISTA_POWERPROFILE_ACCESS
//#ifdef _TEST_PENTIUM_M
#ifdef _WINDOWS
  //wxMenu * p_wxmenuExtras = new wxMenu;
#ifdef COMPILE_WITH_MSR_EXAMINATION
  p_wxmenuExtras->Append(ID_MSR, _T("&MSR...") );
#endif
  mp_wxmenubar->Append(p_wxmenuExtras, _T("E&xtras") );
//#endif
  if( ! p_cpucontroller->mp_model->m_cpucoredata.
    m_stdsetvoltageandfreqDefault.empty() )
    mp_wxmenuitemOwnDVFS = p_wxmenuExtras->Append(
      ID_EnableOrDisableOtherDVFS
      , _T("enable own DVFS") 
      );
#endif
//#ifdef COMPILE_WITH_VISTA_POWERPROFILE_ACCESS
//    ////Connect the action, that is a class derived from class xx directly
//    ////with the menu item so that it is ensured to be the correct action
//    ////(calling the action in an event function is more error-prone)
//    ////TODO release memory
//    ////, new CalculationThread(byCoreID, HighALUloadThreadProc)
//    //, wxCommandEventHandler( MainFrame::OnOwnDynFreqScaling )
//    //) ;
//  mp_wxmenubar->Append(p_wxmenuExtras, _T("E&xtras") );
//#endif
  //mp_wxmenubar->Append( p_wxmenuNorthBridge, _T("&NorthBridge") );
  //SetMenuBar( p_wxmenuBar );
  //SetMenuBar( & m_wxmenubar );
  SetMenuBar( mp_wxmenubar );

  CreateStatusBar();
  const char bits [] = {
      0,0,0,0,
      1,1,1,1,
      0,0,0,0,
      1,1,1,1
    } ;
//  wxIcon wxicon(bits,4,4) ;
//  SetIcon(wxicon);
  Connect(wxEVT_PAINT, wxPaintEventHandler(MainFrame::OnPaint));
  //SetStatusText( _T("Welcome to wxWidgets!") );
  //m_timer.Start(1000);
  m_timer.Start(m_dwTimerIntervalInMilliseconds);
  //http://docs.wxwidgets.org/stable/wx_wxtimer.html#wxtimersetowner:
  //"Associates the timer with the given owner object. When the timer is 
  //running, the owner will receive timer events with id equal to id 
  //specified here."
  m_timer.SetOwner(this, TIMER_ID) ;
  Connect(wxEVT_SIZE, wxSizeEventHandler(MainFrame::OnSize));
#ifdef _TEST_PENTIUM_M
     //mp_model->//m_stdvectorDialog.at(nIndex)
     //    m_stdvectorUIitem.clear();
#endif //#ifdef _TEST_PENTIUM_M
//#ifdef _COMPILE_WITH_CPU_CORE_USAGE_GETTER
#ifdef _COMPILE_WITH_IWBEMSERVICES
  m_cpucoreusagegetteriwbemservices.Init() ;
#endif //_COMPILE_WITH_CPU_CORE_USAGE_GETTER
  LOG("end of main frame creation\n")
}

MainFrame::~MainFrame()
{
  //TODO Error here.the problem was with destructor of wxMenuBar.->create 
  //wxMenuBar via "new"?
  //for(BYTE byCoreID = 0 ; byCoreID < //m_byCoreNumber
  //  mp_cpucoredata->m_byNumberOfCPUCores ; ++ byCoreID )
  //{
  //  //Release memory from the heap.
  //  delete m_vecp_wxmenuCore.at(byCoreID) ;
  //}

  //Release dynamically allocated memory (inside OnInit() ) :
  delete [] m_arp_freqandvoltagesettingdlg ;
  //Release memory for array of pointers.
  delete [] m_arp_wxmenuitemPstate ;
  delete [] marp_wxmenuItemHighLoadThread ;
  if( mp_calculationthread )
    delete mp_calculationthread ;
}

//void MyFrame::AddMenu()
//{
//  //wxEventTableEntry(type, winid, idLast, fn, obj)
//  EVT_MENU(ID_LastStaticID+1,MyFrame::OnDynamicMenu);
//}
//
//void MyFrame::OnDynamicMenu(wxCommandEvent &event)
//{
//	
//}

wxString MainFrame::BuildHighLoadThreadMenuText(
  std::string str,
  BYTE byPreviousAction)
{
  wxString wxstr = byPreviousAction == ENDED ? 
    //We need a _T() macro (wide char-> L"", char->"") for EACH 
    //line to make it compitable between char and wide char.
    _T("Start") : 
    //We need a _T() macro (wide char-> L"", char->"") for EACH 
    //line to make it compitable between char and wide char.
    _T("End") ;
  ////Invert the menu item's checked state.
  //marp_wxmenuItemHighLoadThread[byCoreID]->Check(
  //    ! marp_wxmenuItemHighLoadThread[byCoreID]->IsChecked () ) ;
//  marp_wxmenuItemHighLoadThread[byCoreID]->SetText(
//      wxstr +
//      str
//      );
    return wxstr + wxString( 
      //TODO this conversion may not work
      (const wxChar*) str.c_str() ) ;
}

bool MainFrame::Confirm(const std::string & str)
{
  //::AfxMessageBox(str.c_str());

  //To not show too many dialogs that the timer would bring up.
  if( m_bConfirmedYet )
  {
    m_bConfirmedYet = false ;
    ::wxMessageBox(wxString( 
      //TODO this conversion may not work
      (const wxChar * ) str.c_str() ) 
      );
    m_bConfirmedYet = true ;
  }
  //m_bConfirmedYet = true ;
  return true;
}

bool MainFrame::Confirm(std::ostrstream & r_ostrstream
  //std::ostream & r_ostream
  )
{
  bool bReturn = true ;
  DEBUG("MyFrame::Confirm--begin\n");
  //Must set this, else text may follow after the string we want.
  //I had program crashes with the following method:
  //pch[r_ostrstream.pcount()] = '\0' ;
  //r_ostrstream.ends();
  r_ostrstream.put('\0'); //the same as "ends()" does.
  char *pch = r_ostrstream.str() ;
  //r_ostrstream.flush();
  //To not show too many dialogs that the timer would bring up.
  if( m_bConfirmedYet )
  {
    m_bConfirmedYet = false ;
    int nReturn = ::wxMessageBox( wxString( 
      //TODO this conversion may not work
      (const wxChar * ) pch ) ,
      //We need a _T() macro (wide char-> L"", char->"") for EACH 
      //line to make it compitable between char and wide char.
      wxString( _T("Message") ) , wxCANCEL | wxOK ) ;
    if( nReturn == wxCANCEL )
      bReturn = false ;
    m_bConfirmedYet = true ;
  }
  //return true;
  DEBUG("MyFrame::Confirm--end\n");
  return bReturn ;
}

void MainFrame::CreateServiceMenuItems()
{
  wxMenu * p_wxmenuService = new wxMenu;
  p_wxmenuService->Append( ID_ContinueService, _T("&Continue") );
  p_wxmenuService->Append( ID_PauseService , _T("&Pause") );
  p_wxmenuService->Append( ID_StartService , _T("&Start") );
  p_wxmenuService->Append( ID_StopService , _T("Sto&p") );
    
  mp_wxmenubar->Append( p_wxmenuService, _T("&Service") ) ;
}

//void 
wxMenuItem * MainFrame::AddDynamicallyCreatedMenu(
  wxMenu * p_wxmenu,
  WORD & r_wMenuID,
  const wxString & r_wxstr
  )
{
  wxMenuItem * p_wxmenuitemAppended = p_wxmenu->Append(r_wMenuID, r_wxstr );
  Connect( r_wMenuID ++ , //wxID_ANY,
    wxEVT_COMMAND_MENU_SELECTED ,
    wxCommandEventHandler(MainFrame::OnDynamicallyCreatedUIcontrol)
    );
  return p_wxmenuitemAppended ;
}

wxMenuItem * MainFrame::AddDynamicallyCreatedMenu(
  wxMenu * p_wxmenu,
  WORD & r_wMenuID,
  const wxString & r_wxstr
  , //void (wxEvtHandler::*wxEventFunction)(wxEvent&)
  wxObjectEventFunction wxeee
  , SpecificCPUcoreActionAttributes * p_scaa
  )
{
  m_stdmapwxuicontroldata.insert(
    std::make_pair(
      r_wMenuID ,
      //TODO release memory
      p_scaa
      )
    ) ;
  wxMenuItem * p_wxmenuitemAppended = p_wxmenu->Append(r_wMenuID, r_wxstr );
  Connect( r_wMenuID ++ , //wxID_ANY,
    wxEVT_COMMAND_MENU_SELECTED ,
    //wxCommandEventHandler(
    //wxEventFunction //)
    wxeee
    );
  return p_wxmenuitemAppended ;
}


//wxMenuItem * MainFrame::AddDynamicallyCreatedMenu(
//  wxMenu * p_wxmenu,
//  WORD & r_wMenuID,
//  const wxString & r_wxstr,
//  //Use a contrete class as parameter because this is more flexible than
//  //using a function with (a) fixed parameter type(s).
//  //Another option: use C functions: then parameter here would be:
//  //"void func(PVOID)"
//  //function A (PVOID pv)
//  //{ ActionAParamStruct * paramstr = (ActionAParamStruct)
//  // ActionAParamStruct: neccessary parameters for execution (like CPU core ID)
//  //   pv ; }
//  //
//  I_CPUcontrollerAction * icpuca
//  )
//{
//  wxMenuItem * p_wxmenuitemAppended = p_wxmenu->Append(r_wMenuID, r_wxstr );
//  m_stdmapwmenuid2i_cpucontrolleraction.insert(
//    std::make_pair (r_wMenuID,icpuca)
//    ) ;
//  Connect( r_wMenuID ++, wxID_ANY, wxEVT_COMMAND_MENU_SELECTED,
//    wxCommandEventHandler(MainFrame::OnDynamicallyCreatedUIcontrol)
//    );
//  return p_wxmenuitemAppended ;
//}

void MainFrame::CreateAndInitMenuItemPointers()
{
    BYTE byPstateMenuItemNo = mp_cpucoredata->m_byNumberOfCPUCores * 
        NUMBER_OF_PSTATES ;
    m_arp_wxmenuitemPstate = new wxMenuItem * 
      [ //mp_cpucoredata-> //GetNumberOfCPUcores() 
      //m_byNumberOfCPUCores *
      //NUMBER_OF_PSTATES 
      byPstateMenuItemNo ] ;
    for( BYTE byIndex = 0 ; byIndex < byPstateMenuItemNo ; ++ byIndex )
        m_arp_wxmenuitemPstate[byIndex] = NULL ;

    marp_wxmenuItemHighLoadThread = new wxMenuItem * [ 
        mp_cpucoredata->m_byNumberOfCPUCores ] ;
    for( BYTE byIndex = 0 ; byIndex < mp_cpucoredata->m_byNumberOfCPUCores ; 
        ++ byIndex )
        marp_wxmenuItemHighLoadThread[ byIndex ] = NULL ;
}

//void 
//Return: TRUE: success.
BYTE MainFrame::AddSetPstateMenuItem(
  wxMenu * p_wxmenuCore
  , BYTE byCoreID
  , BYTE byPstateID 
  //Must be a reference because changes to the variable should be 
  //maintained OUTside this function.
  , WORD & r_wMenuID
  )
{
    BYTE byReturnValue = FALSE ;
    //DWORD dwLow ;
    //DWORD dwHigh ;
//    PState pstate ;
//    byReturnValue = mp_pumastatectrl->GetPStateFromMSR(
//        byPstateID,
//        dwLow,
//        dwHigh,
//        pstate,
//        byCoreID
//        ) ;
//    //If getting the p-state data was successfull.
//    //If e.g. this program was not started as admin it probably was
//    //not successfull.
//    if( byReturnValue )
//      m_arp_wxmenuitemPstate[ byCoreID * NUMBER_OF_PSTATES + byPstateID ] =
//        AddDynamicallyCreatedMenu(
//          p_wxmenuCore,
//          r_wMenuID,
//          //_T("Set p-state &0")
//          GetSetPstateMenuItemLabel(
//              //byPstateNumber
//              byPstateID
//              //mp_pumastatectrl->mp_model-> pstate
//              //Gets the Voltage ID, Frequ ID for the p-state:
//              //if not set by the user, then
//              //GetPState(byPstateID)
//              , pstate
//          )
//          //, new SelectPstateAction(byCoreID, byPstateID )
//        ) ;

    return byReturnValue ;
}

//void 
//Return value: 
BYTE MainFrame::CreateDynamicMenus()
{
  BYTE byPstateID = 0 ;
  BYTE byReturnValue = 
      //Needs to be TRUE for the 1st loop condition evaluation.
      TRUE ;
  WORD wMenuID = ID_LastStaticID;
  //m_vecwxstring.push_back(wxString)
  wxMenu * p_wxmenuCore ;
  for( BYTE byCoreID = 0 ; byCoreID < //m_byCoreNumber
    mp_cpucoredata->m_byNumberOfCPUCores ; ++ byCoreID )
  {
    p_wxmenuCore = new wxMenu;
    //m_vecp_wxmenuCore.push_back(p_wxmenuCore);
    p_wxmenuCore->Append(wMenuID, _T("set frequency and voltage ") );
    //for(BYTE byDivisorID = 0 ; byDivisorID < FIRST_RESERVED_DIVISOR_ID ; 
    //  ++ byDivisorID )
    //{
    //  Connect( wMenuID ++, wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, 
    //    wxCommandEventHandler(MyFrame::OnRuntimeCreatedMenu)//, & m_vecwxstring.back() 
    //    );
    //}
    if( byCoreID == 0 ) 
      m_nLowestIDForSetVIDnFIDnDID = wMenuID ;
//    wxCPUcoreID wxcpucoreid(byCoreID)  ;
//    m_stdvectorwxuicontroldata.push_back( //wxCPUcoreID(byCoreID)
//      wxcpucoreid
//      //wxObject()
//      ) ;
    m_stdmapwxuicontroldata.insert( 
      std::make_pair( 
        wMenuID ,
        //TODO release memory
        //new wxCPUcoreID(byCoreID) 
        new SpecificCPUcoreActionAttributes(byCoreID)
        //wxcpucoreid 
        //wxObject() 
        )
      ) ;
#ifdef _DEBUG
    wxCPUcoreID * p_wxcpucoreid = (wxCPUcoreID *) //wxevent.m_callbackUserData ;
      //& 
      m_stdmapwxuicontroldata.find( wMenuID )->second ;
    //wxCPUcoreID & r_wxcpucoreid = (wxCPUcoreID &) //wxevent.m_callbackUserData ;
    //  m_stdmapwxuicontroldata.find( wMenuID )->second ;
#endif
    Connect( wMenuID ++, wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, 
      wxCommandEventHandler(
        //MainFrame::OnDynamicallyCreatedUIcontrol
        MainFrame::OnPstateDialog )//, & m_vecwxstring.back()
      //new wx
      //, & m_stdvectorwxuicontroldata.back()
      );
    p_wxmenuCore->Append(wMenuID, _T("find different p-states") );
    Connect( wMenuID ++, wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, 
      wxCommandEventHandler(
      MainFrame::OnFindDifferentPstates )
      );
    #ifdef COMPILE_WITH_CALC_THREAD
    marp_wxmenuItemHighLoadThread[byCoreID] = AddDynamicallyCreatedMenu(
      p_wxmenuCore,
      wMenuID, //_T("high load thread (for stability check)")
      BuildHighLoadThreadMenuText(
       "high FPU load thread (for stability check)"
        )
      //Connect the action, that is a class derived from class xx directly
      //with the menu item so that it is ensured to be the correct action
      //(calling the action in an event function is more error-prone)
      //TODO release memory
      //, new CalculationThread(byCoreID, FPUcalculationThreadProc)
      , wxCommandEventHandler( MainFrame::OnHighLoadThread )
      , new CalculationThread(
        byCoreID
        , FPUcalculationThreadProc
        , & ::wxGetApp() 
        , ::wxGetApp().GetCPUcontroller() 
        )
      ) ;
    //#endif //#ifdef COMPILE_WITH_CALC_THREAD
    marp_wxmenuItemHighLoadThread[byCoreID] = AddDynamicallyCreatedMenu(
      p_wxmenuCore,wMenuID, //_T("high load thread (for stability check)")
      BuildHighLoadThreadMenuText(
      std::string( "high ALU load thread (for stability check)" )
        )
      //Connect the action, that is a class derived from class xx directly
      //with the menu item so that it is ensured to be the correct action
      //(calling the action in an event function is more error-prone)
      //TODO release memory
      //, new CalculationThread(byCoreID, HighALUloadThreadProc)
      , wxCommandEventHandler( MainFrame::OnHighLoadThread )
      , new CalculationThread(
        byCoreID
        , HighALUloadThreadProc 
        , & ::wxGetApp()
        , ::wxGetApp().GetCPUcontroller()
        )
      ) ;
    #endif //    #ifdef COMPILE_WITH_CALC_THREAD

    //marp_wxmenuItemHighLoadThread[byCoreID] = AddDynamicallyCreatedMenu(
    //  p_wxmenuCore,
    //  wMenuID, //_T("high load thread (for stability check)")
    //   _T("own DVFS")
    //  //Connect the action, that is a class derived from class xx directly
    //  //with the menu item so that it is ensured to be the correct action
    //  //(calling the action in an event function is more error-prone)
    //  //TODO release memory
    //  //, new CalculationThread(byCoreID, HighALUloadThreadProc)
    //  , wxCommandEventHandler( MainFrame::OnOwnDynFreqScaling )
    //  , new SpecificCPUcoreActionAttributes(byCoreID)
    //  ) ;
    //OnOwnDynFreqScaling
    //p_wxmenuCore->Append(wMenuID, _T("Set p-state &0") );
    //Connect( wMenuID ++, wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, 
    //  wxCommandEventHandler(MyFrame::OnRuntimeCreatedMenu) 
    //  );
    //byPstateID = 0 ;

    //m_arp_wxmenuitemPstate[byCoreID * NUMBER_OF_PSTATES ] = 
    //    AddDynamicallyCreatedMenu(p_wxmenuCore,wMenuID, //_T("Set p-state &0")
    //        GetSetPstateMenuItemLabel(byPstateNumber,
    //        //mp_pumastatectrl->mp_model-> pstate
    //        GetPState(byPstateID)
    //        )
    //    ) ;
    //
    ////p_wxmenuCore->Append(wMenuID, _T("Set p-state &1") );
    ////Connect( wMenuID ++, wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, 
    ////  wxCommandEventHandler(MyFrame::OnRuntimeCreatedMenu) 
    ////  );

    //m_arp_wxmenuitemPstate[byCoreID * NUMBER_OF_PSTATES + 1] = 
    //    AddDynamicallyCreatedMenu(p_wxmenuCore,wMenuID, _T("Set p-state &1")) ;
    ////p_wxmenuCore->Append(wMenuID, _T("Set p-state &2") );
    ////Connect( wMenuID ++, wxID_ANY, wxEVT_COMMAND_MENU_SELECTED, 
    ////  wxCommandEventHandler(MyFrame::OnRuntimeCreatedMenu) 
    ////  );

    //m_arp_wxmenuitemPstate[byCoreID * NUMBER_OF_PSTATES + 2] = 
    //    AddDynamicallyCreatedMenu(p_wxmenuCore,wMenuID, _T("Set p-state &2")) ;
    m_byNumberOfSettablePstatesPerCore = NUMBER_OF_PSTATES ;
    if( byCoreID == 0 ) 
      m_byMenuIndexOf1stPstate = wMenuID - m_nLowestIDForSetVIDnFIDnDID ;
    //if( typeid (mp_cpucontroller ) == typeid(GriffinController) )
    //{
    //  for( byPstateID = 0 ; byPstateID < //3
    //    //NUMBER_OF_PSTATES
    //    m_byNumberOfSettablePstatesPerCore ; ++ byPstateID &&
    //      //if == TRUE
    //      byReturnValue
    //     )
    //    byReturnValue = AddSetPstateMenuItem(
    //      p_wxmenuCore, byCoreID, byPstateID, wMenuID ) ;
    //}
    if( byReturnValue )
    {
        //marp_wxmenuItemHighLoadThread[byCoreID] = AddDynamicallyCreatedMenu(
        //    p_wxmenuCore,wMenuID, _T("high load thread (for stability check)")) ;
        //m_wxmenubar.Append(p_wxmenuCore, _T("for core &")+ byCoreID);
        mp_wxmenubar->Append(
          p_wxmenuCore,
          wxString::Format(
          //We need a _T() macro (wide char-> L"", char->"") for EACH 
          //line to make it compitable between char and wide char.
          _T("%s%u"),_T("for core &"), //_T('0'+byCoreID)
            byCoreID )
          );
        //marp_wxmenuItemHighLoadThread[byCoreID]->Check(true) ;
        //marp_wxmenuItemHighLoadThread[byCoreID]->Enable(false);
        if( byCoreID == 0 ) 
          m_nNumberOfMenuIDsPerCPUcore = wMenuID - ID_LastStaticID ;
    }
  }
  //SetMenuBar(&m_wxmenubar);
  return byReturnValue ;
}

void MainFrame::DisableWindowsDynamicFreqScalingHint()
{
   wxMessageBox(
     //We need a _T() macro (wide char-> L"", char->"") for EACH 
     //line to make it compitable between char and wide char.
     _T("Remember: the OS' dynamic frequency scaling should be ")
     _T("deactivated the change to take effect\n")
     _T("see help/ disable by: ")
     _T("Start->control panel->energy options")
     _T("->change the current power plan")
     _T("->change extended energy options")
     _T("->set minimal and maximal CPU load to the same percentage") 
     );
}


void MainFrame::OnContinueService(wxCommandEvent & WXUNUSED(event))
{
  //ServiceBase::ContinueService( //mp_model->m_strServiceName.c_str() 
  //  "CPUcontrolService" );
#ifdef COMPILE_WITH_NAMED_WINDOWS_PIPE
    if( ! ::wxGetApp().m_ipcclient.IsConnected() )
    ::wxGetApp().m_ipcclient.Init() ;
  if( ::wxGetApp().m_ipcclient.IsConnected() )
    ::wxGetApp().m_ipcclient.SendMessage(continue_service) ;
#endif //#ifdef COMPILE_WITH_NAMED_WINDOWS_PIPE
}

void MainFrame::OnFindDifferentPstates( wxCommandEvent & WXUNUSED(event) )
{
  //wxString wxstrInput = ::wxGetTextFromUser( wxstrMessage , wxT("input"), 
  //  wxT("1000") ) ;
  ////::wxGetNumberFromUser
  ////If the user has input text (and has NOT pressed "cancel")
  //if( ! wxstrInput.empty() )
  //{
  //  unsigned long ulMs ;
  //  bool bSuccess = wxstrInput.ToULong(& ulMs) ;
  //  if ( bSuccess )
  //  {
  //    //ReadMsrEx() returned false results if used with a time and a 10 ms interval.
  //    if( ulMs < 100 )
  //      wxMessageBox( wxT("the number is too low. "
  //      "Getting the current CPU frequency returned wrong values with < 100 ms") ) ;
  //    else
  //    {
  //      m_dwTimerIntervalInMilliseconds = ulMs ;
  //      m_timer.Stop() ;
  //      m_timer.Start(ulMs) ;
  //    }
  //  }
  //  else
  //  {
  //    wxMessageBox( wxT("You did not enter a valid integer number") ) ;
  //  }
  //}
  //Must create dynamically, else the CalculationThread is destroyed after leaving
  //this block.
  //if( ! mp_calculationthread )
  //{
  //  mp_calculationthread = new CalculationThread( 
  //  0 
  //  , FindDifferentPstatesThreadProc
  //  , & ::wxGetApp() 
  //  , ::wxGetApp().GetCPUcontroller() 
  //  ) ;
  //}
  //if( mp_calculationthread )
  //  mp_calculationthread->Execute() ;
  long lMin = ::wxGetNumberFromUser(
    wxT("input mininum frequency:"), 
    wxT("prompt:"), 
    wxT("caption:"), 
    mp_cpucontroller->GetMinimumFrequencyInMHz() ,
    0 ,
    mp_cpucontroller->GetMaximumFrequencyInMHz()    
    ) ;
  //If the user enters an invalid value or cancels the dialog, the function will return -1.
  if( lMin == -1 )
    ::wxMessageBox(wxT("either invalid value or cancel") ) ;
  else
  {
    long lMax = ::wxGetNumberFromUser(
      wxT("input maxinum frequency:"), 
      wxT("prompt:"), 
      wxT("caption:"), 
      mp_cpucontroller->GetMaximumFrequencyInMHz() ,
      lMin ,
      mp_cpucontroller->GetMaximumFrequencyInMHz()
      ) ;
    if( lMax == -1 )
      ::wxMessageBox(wxT("either invalid value or cancel") ) ;
    else
    {
      std::set<VoltageAndFreq> stdsetvoltageandfreq ;
      std::set<VoltageAndFreq>::iterator iter ;
      mp_cpucontroller->GetAllPossibleFrequencies( stdsetvoltageandfreq ) ;
      //iter = stdsetvoltageandfreq.find(lMin) ;
      iter = stdsetvoltageandfreq.begin() ;
      while( iter != stdsetvoltageandfreq.end() )
      {
        if( lMin >= iter->m_wFreqInMHz )
          break ;
        iter ++ ;
      }
      

      //if( iter != stdsetvoltageandfreq.end() )
      {
        float fVolt ;
        WORD wFreq ;
        //for( WORD wFreq = lMin ; wFreq < lMax ; wFreq += 50 )
        while( iter != stdsetvoltageandfreq.end() )
        {
          //mp_cpucontroller->GetNearestHigherPossibleFreqInMHz( lMin ) ;
          mp_cpucontroller->SetFreqAndVoltageFromFreq( //wFreq, 
            iter->m_wFreqInMHz ,
            mp_model->m_cpucoredata.m_stdsetvoltageandfreqDefault
            //mp_model->m_cpucoredata.m_stdsetvoltageandfreqPossibleByCPU
            //stdsetvoltageandfreq
            , 0 ) ;
          ::wxMilliSleep(100) ;
          mp_cpucontroller->GetCurrentPstate( //wFreq, 
            wFreq ,
            fVolt
            , 0 ) ;
          if( wFreq == iter->m_wFreqInMHz )
            if( mp_model->m_cpucoredata.AddDefaultVoltageForFreq( fVolt, wFreq ) )
              RedrawEverything() ;
          ++ iter ;
        }
      }
    }
  }
  //::wxMilliSleep()
}

void MainFrame::OnPauseService(wxCommandEvent & WXUNUSED(event))
{
#ifdef COMPILE_WITH_NAMED_WINDOWS_PIPE
    if( ! ::wxGetApp().m_ipcclient.IsConnected() )
    ::wxGetApp().m_ipcclient.Init() ;
  if( ::wxGetApp().m_ipcclient.IsConnected() )
    ::wxGetApp().m_ipcclient.SendMessage(pause_service) ;
  else
    try
    {
      ServiceBase::PauseService( //mp_model->m_strServiceName.c_str() 
        //We need a _T() macro (wide char-> L"", char->"") for EACH 
        //line to make it compitable between char and wide char.
        _T("CPUcontrolService") );
    }
    catch(ConnectToSCMerror connecttoscmerror )
    {
      ::wxMessageBox( wxString(
        //We need a _T() macro (wide char-> L"", char->"") for EACH 
        //line to make it compitable between char and wide char.
        _T("connecting to service control manager failed: ") 
        ) +
        (const wxChar *) ::LocalLanguageMessageFromErrorCode( 
        connecttoscmerror.m_dwErrorCode).c_str() 
        ) ;
    }
  #endif //#ifdef COMPILE_WITH_NAMED_WINDOWS_PIPE
}

void MainFrame::OnStartService(wxCommandEvent & WXUNUSED(event))
{
  #ifdef _WINDOWS
  ServiceBase::StartService( //mp_model->m_strServiceName.c_str() 
    //We need a _T() macro (wide char-> L"", char->"") for EACH 
    //line to make it compitable between char and wide char.
    _T("CPUcontrolService") 
    );
  #endif
}

void MainFrame::OnStopService(wxCommandEvent & WXUNUSED(event))
{
  //ServiceBase::StopService( //mp_model->m_strServiceName.c_str() 
  //  "CPUcontrolService" );
}

void MainFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
  std::tstring stdtstr ;
  std::vector<std::tstring> stdvecstdtstring ;
  MainController::GetSupportedCPUs(stdvecstdtstring) ;
  for(BYTE by = 0 ; by < stdvecstdtstring.size() ; by ++ )
    stdtstr += stdvecstdtstring.at(by) + _T(" ") ;
  wxMessageBox(
    //We need a _T() macro (wide char-> L"", char->"") for EACH 
    //line to make it compitable between char and wide char.
    
      //"A tool for optimizing the usage of AMD family 17\n, "
//      _T("A tool for controlling performance states of AMD family 17\n, ")
//      _T("codename \"griffin\" (mobile) CPUs, especially for undervolting.\n")
      //_T("A tool for controlling performance states of AMD family 17\n, ")
      _T("A tool for controlling performance states of ")
      + stdtstr +
      _T( "\n, ")

      //"Build: " __DATE__ " " __TIME__ "GMT\n\n"
      BUILT_TIME
      //"AMD--smarter choice than Intel?\n\n"
      //"To ensure a stable operation:\n"
      _T("I observed system instability when when switching from power ")
      _T("supply operation to battery mode\n")
      _T("So test for this case if needed: test for the different p-states, ")
      _T("especially for the ones that are much undervolted.\n")
      _T("If the system is instable, heighten the voltage(s).\n")
      _T("Note that the OS may freeze if changing voltages, so you may encounter data loss.\n")
      _T("->save all of your work before.\n\n")
      //" -when switching from power supply operation to battery,\n"
      _T("Licence/ info: http://amd.goexchange.de / http://sw.goexchange.de")
      //"who wants to boinc Dr. Tigerlilly?\n\n"
      //"--deutsches Qualitaetserzeugnis--"
    ,
    //"About wxPumaStatecontrol"
    //_T("About GriffinControlDialog"),
    _T("About ") //_T(PROGRAM_NAME)
    + mp_cpucontroller->mp_model->m_stdtstrProgramName
    ,
    wxOK | wxICON_INFORMATION, this
    );
}

void MainFrame::OnHighLoadThread( wxCommandEvent & //WXUNUSED(wxevent) 
  wxevent )
{
  WORD wEventID = wxevent.GetId() ;
  #ifdef COMPILE_WITH_CALC_THREAD
//  HighLoadThreadAttributes p_hlta =  (HighLoadThreadAttributes * )
//    m_stdmapwxuicontroldata.find( wEventID )->second ;
  CalculationThread * p_ct = (CalculationThread *)
    m_stdmapwxuicontroldata.find( wEventID )->second ;
  if( p_ct )
    p_ct->Execute() ;
  #endif //#ifdef COMPILE_WITH_CALC_THREAD
}

void MainFrame::OnOwnDynFreqScaling( wxCommandEvent & //WXUNUSED(wxevent) 
  wxevent )
{
  WORD wEventID = wxevent.GetId() ;
  #ifdef COMPILE_WITH_NAMED_WINDOWS_PIPE
  //TODO
  if( ::wxGetApp().m_ipcclient.IsConnected() )
  {
    ::wxGetApp().m_ipcclient.SendMessage(start_DVFS) ;
    ::wxGetApp().m_ipcclient.SendMessage(start_DVFS) ;
  }
  else
  #endif //#ifdef COMPILE_WITH_NAMED_WINDOWS_PIPE
  {
    //if( mp_pumastatectrl->mp_dynfreqscalingaccess->DVFSisEnabled() ;
    //SpecificCPUcoreActionAttributes * p_atts = (CalculationThread *)
    //  m_stdmapwxuicontroldata.find( wEventID )->second ;
    PerCPUcoreAttributes * p_percpucoreattributes = & mp_cpucoredata->
      m_arp_percpucoreattributes[ //p_atts->m_byCoreID 
      0 ] ;
    //DynFreqScalingThread * p_dynfreqscalingthread
    if ( p_percpucoreattributes->mp_dynfreqscalingthread )
    {
      p_percpucoreattributes->mp_dynfreqscalingthread->Stop() ;
      //p_percpucoreattributes->mp_dynfreqscalingthread->Delete() ;
      p_percpucoreattributes->mp_dynfreqscalingthread = NULL ;
	    ////Start the timer (it should have been stopped before else the timer had redrawn 
	    ////addtionally to the scaling thread).
     // m_timer.Start() ;
      mp_wxmenuitemOwnDVFS->SetText(
        //We need a _T() macro (wide char-> L"", char->"") for EACH 
        //line to make it compitable between char and wide char.
        _T("enable Own DVFS") 
        ) ;
    }
    else
    {
        PossiblyAskForOSdynFreqScalingDisabling() ;
        if( ! //mp_pumastatectrl->mp_dynfreqscalingaccess->OtherDVFSisEnabled() 
            //mp_cpucontroller->mp_dynfreqscalingaccess->OtherDVFSisEnabled()
            mp_cpucontroller->OtherPerfCtrlMSRwriteIsActive()
          )
        {
          //TODO is the memory released?
          DynFreqScalingDlg * p_dynfreqscalingdlg = new DynFreqScalingDlg( 
            this 
            , *mp_cpucoredata 
            ) ;
          if( p_dynfreqscalingdlg )
            p_dynfreqscalingdlg->
            //Showing it "modal" is important because else the instructions below 
            //continue before the dialog ic closed.
            ShowModal();
          //p_percpucoreattributes->mp_dynfreqscalingthread 
          p_percpucoreattributes->SetCPUcontroller( mp_cpucontroller ) ;
          p_percpucoreattributes->CreateDynFreqScalingThread( 
            //& m_clocksnothaltedcpucoreusagegetter
            ::wxGetApp().mp_cpucoreusagegetter
            //wxApp
            ) ;
	        ////Stop the timer (else the timer redraws addtionally to the scaling thread).
	        //m_timer.Stop() ;
          mp_wxmenuitemOwnDVFS->SetText(
            //We need a _T() macro (wide char-> L"", char->"") for EACH 
            //line to make it compitable between char and wide char.
            _T("disable Own DVFS") 
            ) ;
        }
      }
  }
}

void MainFrame::OnPstateDialog( wxCommandEvent & //WXUNUSED(event)
  wxevent )
{
  WORD wEventID = wxevent.GetId() ;
  //wxCPUcoreID * p_wxcpucoreid = (wxCPUcoreID *) //wxevent.m_callbackUserData ;
  //  & m_stdmapwxuicontroldata.find( wxevent.GetId() )->second ;
  //wxCPUcoreID & r_wxcpucoreid = (wxCPUcoreID &) //wxevent.m_callbackUserData ;
  //  m_stdmapwxuicontroldata.find( wxevent.GetId() )->second ;
  //const wxObject & wxobj = //wxevent.m_callbackUserData ;
  //  m_stdmapwxuicontroldata.find( wxevent.GetId() )->second ;
  //wxCPUcoreID & r_wxcpucoreid2 = (wxCPUcoreID &) wxobj ;
  //const wxCPUcoreID * p_wxcpucoreid2 = (const wxCPUcoreID *) & wxobj ;
  //std::map <WORD, wxObject> :: iterator iter =
  //  m_stdmapwxuicontroldata.find( wxevent.GetId() ) ;
  //wxCPUcoreID wxcpucoreid = (wxCPUcoreID ) iter->second ;
//  wxCPUcoreID * p_wxcpucoreid = (wxCPUcoreID *) //wxevent.m_callbackUserData ;
//    //&
//    m_stdmapwxuicontroldata.find( wEventID )->second ;
  SpecificCPUcoreActionAttributes * p = (SpecificCPUcoreActionAttributes *)
    m_stdmapwxuicontroldata.find( wEventID )->second ;

  BYTE byCoreID = //p_wxcpucoreid->m_byID
    p->m_byCoreID ;
  //BYTE byCoreID = 0 ;
  if( m_arp_freqandvoltagesettingdlg[byCoreID] )
    m_arp_freqandvoltagesettingdlg[byCoreID]->Show(true);
  else
  {
    //If created as local vaiable on stack the dialog would disappear 
    //immediately.
    m_arp_freqandvoltagesettingdlg[byCoreID] = new 
      FreqAndVoltageSettingDlg( 
      this 
      //, mp_pumastatectrl
      ,  mp_cpucontroller
      , byCoreID 
      );
    if( m_arp_freqandvoltagesettingdlg[byCoreID] )
      m_arp_freqandvoltagesettingdlg[byCoreID]->Show(true);
  }
}

void MainFrame::OnQuit(wxCommandEvent& //WXUNUSED(event)
                     event)
{
  Close(TRUE);
}

void MainFrame::DrawAllPossibleOperatingPoints(
  wxDC & r_wxdcDrawOn
  )
{
  //float fCurrentVoltageInVolt ;
  //WORD wXcoordinate ;
  //WORD wYcoordinate ;
  WORD wMaxFreqInMHz = //mp_pumastatectrl->mp_model->m_cpucoredata.
    mp_cpucontroller->mp_model->m_cpucoredata.
    m_wMaxFreqInMHz ;

  WORD wCurrentFreqInMHz = //PState::GetFreqInMHz(
    //mp_pumastatectrl->mp_model->m_cpucoredata.
    //  m_byLowestEffectiveFreqID
    //, 
    //HIGHEST_EFFECTIVE_DIVISOR_ID
    //) ;
    mp_cpucontroller->mp_model->m_cpucoredata.m_wMaxFreqInMHz ;
  //DIDandFID didandfid ;
  BYTE byMinVoltageID = mp_cpucoredata->m_byMinVoltageID ;
  BYTE byMaxVoltageID = mp_cpucoredata->m_byMaxVoltageID ;
  //BYTE byCurrentVoltageID ;
  //do 
  //{
  //  for( byCurrentVoltageID = byMinVoltageID ; 
  //    byCurrentVoltageID <= byMaxVoltageID ; ++ byCurrentVoltageID )
  //  {
  //    fCurrentVoltageInVolt = PState::GetVoltageInVolt( byCurrentVoltageID ) ;
  //    wXcoordinate =
  //      m_wXcoordOfBeginOfYaxis +
  //        (float) wCurrentFreqInMHz /
  //        (float) wMaxFreqInMHz * m_wDiagramWidth ;
  //    wYcoordinate =
  //      m_wDiagramHeight - fCurrentVoltageInVolt
  //      / m_fMaxVoltage * m_wDiagramHeight ;
  //    r_wxdcDrawOn.DrawLine( 
  //      //wCurrentXcoordinateInDiagram + m_wXcoordOfBeginOfYaxis,
  //      wXcoordinate
  //      , wYcoordinate
  //      , //wCurrentXcoordinateInDiagram + m_wXcoordOfBeginOfYaxis
  //      wXcoordinate
  //      //"+ 1" because: http://docs.wxwidgets.org/stable/wx_wxdc.html#wxdcdrawline:
  //      //"Note that the point (x2, y2) is not part of the line and is 
  //      //not drawn by this function (this is consistent with the 
  //      //behaviour of many other toolkits)."
  //      + 1
  //      , //(200-fVoltage*100) 
  //      wYcoordinate
  //      //"+ 1" because: http://docs.wxwidgets.org/stable/wx_wxdc.html#wxdcdrawline:
  //      //"Note that the point (x2, y2) is not part of the line and is 
  //      //not drawn by this function (this is consistent with the 
  //      //behaviour of many other toolkits)."
  //      + 1 
  //      ) ;
  //  }
  //  didandfid = mp_pumastatectrl->
  //    GetNearestHigherPossibleFreqInMHzAsDIDnFID( wCurrentFreqInMHz + 1 ) ;
  //  wCurrentFreqInMHz = didandfid.GetFreqInMHz() ;
  //} while( wCurrentFreqInMHz != mp_cpucoredata->m_wMaxFreqInMHz ) ;
}

void MainFrame::DrawPerformanceStatesCrosses(
  wxDC & r_wxdc 
  , const std::set<VoltageAndFreq> & cr_stdsetmaxvoltageforfreq 
  , const wxColor * cp_wxcolor 
  )
{
  const wxPen wxpenCurrent = r_wxdc.GetPen() ;
#ifdef _DEBUG
  const wxColor wxcolor = wxpenCurrent.GetColour() ;
  int nPenWidth = wxpenCurrent.GetWidth() ;
#endif
  std::set<VoltageAndFreq>::const_iterator iter = 
    cr_stdsetmaxvoltageforfreq.begin() ;
  while( iter != cr_stdsetmaxvoltageforfreq.end() )
  {
    DrawVoltageFreqCross(
      r_wxdc
      , iter->m_fVoltageInVolt
      , iter->m_wFreqInMHz
      , cp_wxcolor
      ) ;
    ++ iter ;
  }
#ifdef _DEBUG
  if( nPenWidth == 3 )
    nPenWidth = 3 ;
#endif
  //Restore the pen.
  r_wxdc.SetPen( wxpenCurrent ) ;
}

void MainFrame::DrawDiagramScale(
  wxDC & wxdc ,
  //std::set<MaxVoltageForFreq>::iterator & iterstdsetmaxvoltageforfreq
  std::set<VoltageAndFreq>::iterator & iterstdsetvoltageandfreq
  )
{
  //float fMinVoltage ;
  //float fMaxMinusMinVoltage ;
  WORD wXcoordinate ;
  WORD wYcoordinate ;
  WORD wMaxFreqInMHz = //mp_pumastatectrl->GetMaximumFrequencyInMHz() ;
    //mp_pumastatectrl->mp_model->m_cpucoredata.m_wMaxFreqInMHz ;
    mp_cpucontroller->mp_model->m_cpucoredata.m_wMaxFreqInMHz ;
  std::set<VoltageAndFreq> & r_stdsetvoltageandfreq = 
    mp_cpucontroller->mp_model->m_cpucoredata.m_stdsetvoltageandfreqDefault ;
  iterstdsetvoltageandfreq = r_stdsetvoltageandfreq.begin() ;
#ifdef _DEBUG
  const wxPen & wxpenCurrent = wxdc.GetPen() ;
  const wxColor wxcolor = wxpenCurrent.GetColour() ;
  int nPenWidth = wxpenCurrent.GetWidth() ;
#endif

  //fMinVoltage = mp_cpucontroller->GetMinimumVoltageInVolt() ;
  //fMaxMinusMinVoltage = m_fMaxVoltage - fMinVoltage ;
  //p_wxpaintdc->DrawText( wxT("550") , arwxpoint[0].x, //200
  //    wDiagramHeight ) ;
  //p_wxpaintdc->DrawText( wxT("1100") , arwxpoint[1].x, //200
  //    wDiagramHeight ) ;
  //p_wxpaintdc->DrawText( wxT("2200") , arwxpoint[2].x, //200
  //    wDiagramHeight ) ;
  for( ;
      //iterstdvecmaxvoltageforfreq != mp_pumastatectrl->mp_model->
      //iterstdsetmaxvoltageforfreq != //mp_pumastatectrl->mp_model->
      iterstdsetvoltageandfreq !=
      //mp_cpucontroller->mp_model->
      //m_vecmaxvoltageforfreq.end()
      //m_setmaxvoltageforfreq.end() ; //++ iterstdvecmaxvoltageforfreq
      r_stdsetvoltageandfreq.end() ;
      //++ iterstdsetmaxvoltageforfreq
      ++ iterstdsetvoltageandfreq
      )
  {
      wXcoordinate =
        m_wXcoordOfBeginOfYaxis +
          (float) //(*iterstdvecmaxvoltageforfreq).m_wFreqInMHz /
          //(*iterstdvecmaxvoltageforfreq).m_wFreqInMHz /
          //(*iterstdsetmaxvoltageforfreq).m_wFreqInMHz /
          (*iterstdsetvoltageandfreq).m_wFreqInMHz /
          (float) wMaxFreqInMHz * m_wDiagramWidth ;
      //Draw frequency mark.
      //p_wxpaintdc->DrawText(
      //wxmemorydc.DrawText(
      wxdc.DrawText(
        //wxT("550")
        wxString::Format(
          //We need a _T() macro (wide char-> L"", char->"") for EACH 
          //line to make it compitable between char and wide char.
          _T("%u") ,
              //(*iterstdvecmaxvoltageforfreq).m_wFreqInMHz )
              //(*iterstdsetmaxvoltageforfreq).m_wFreqInMHz )
              (*iterstdsetvoltageandfreq).m_wFreqInMHz)
          ,
          //wXcoordOfBeginOfYaxis +
          //(float) (*iterstdvecmaxvoltageforfreq).m_wFreqInMHz /
          //(float) wMaxFreqInMHz * wDiagramWidth
          wXcoordinate
          , //200
          m_wDiagramHeight
          ) ;
      //Draw vertical line for current frequency mark.
      //p_wxpaintdc->DrawLine(wXcoordinate, 0, wXcoordinate, wDiagramHeight) ;
      //wxmemorydc.DrawLine(wXcoordinate, 0, wXcoordinate, wDiagramHeight) ;
      wxdc.DrawLine(wXcoordinate, 0, wXcoordinate, m_wDiagramHeight) ;

      if( m_bRangeBeginningFromMinVoltage )
        wYcoordinate =
          m_wDiagramHeight -
          ( (*iterstdsetvoltageandfreq).m_fVoltageInVolt - m_fMinVoltage )
          / m_fMaxMinusMinVoltage * m_wDiagramHeight ;
      else
        wYcoordinate =
          m_wDiagramHeight -
          //(*iterstdvecmaxvoltageforfreq).m_fVoltageInVolt
          //(*iterstdsetmaxvoltageforfreq).m_fVoltageInVolt
          (*iterstdsetvoltageandfreq).m_fVoltageInVolt
          / m_fMaxVoltage * m_wDiagramHeight ;
      //Draw voltage mark.
      //p_wxpaintdc->DrawText(
      //wxmemorydc.DrawText(
      wxdc.DrawText(
          wxString::Format(
            //We need a _T() macro (wide char-> L"", char->"") for EACH 
            //line to make it compitable between char and wide char.
            _T("%.3f") ,
              //(*iterstdvecmaxvoltageforfreq).m_fVoltageInVolt
              //(*iterstdsetmaxvoltageforfreq).m_fVoltageInVolt
              (*iterstdsetvoltageandfreq).m_fVoltageInVolt
              ),
          5
          ,
          //wDiagramHeight -
          //(*iterstdvecmaxvoltageforfreq).m_fVoltageInVolt
          /// fMaxVoltage * wDiagramHeight
          wYcoordinate
          ) ;
      //Draw horizontal line for current voltage mark.
      //p_wxpaintdc->DrawLine(wXcoordOfBeginOfYaxis, wYcoordinate,
      //wxmemorydc.DrawLine(wXcoordOfBeginOfYaxis, wYcoordinate,
      wxdc.DrawLine(m_wXcoordOfBeginOfYaxis, wYcoordinate,
          m_wDiagramWidth + m_wXcoordOfBeginOfYaxis, wYcoordinate ) ;
  }
}

void MainFrame::DrawLowestStableVoltageCurve(
  wxDC & wxdc
  , float fMaxVoltage
  )
{
  std::set<VoltageAndFreq>::const_iterator ciLower =
    mp_cpucoredata->m_setloweststablevoltageforfreq.begin() ;
    ////because of the "MaxVoltageForFreq::<" operator the set is sorted in 
    ////DESCENDING order.
    ////Points BEYOND the last element now.
    //mp_cpucoredata->m_setloweststablevoltageforfreq.end() ;
  ////Now it should point to the last element.
  //-- ciLower ;
  if( ciLower != mp_cpucoredata->m_setloweststablevoltageforfreq.end() 
    )
  {
    std::set<VoltageAndFreq>::const_iterator ciHigher =
      ciLower ;
    ciHigher ++ ;
    //ciHigher -- ;
    if( ciHigher != mp_cpucoredata->m_setloweststablevoltageforfreq.end() 
      )
    {
      float fVoltage ;
      WORD wYcoordinate ;
      WORD wMaxFreqInMHz = //mp_pumastatectrl->GetMaximumFrequencyInMHz() ;
        //mp_pumastatectrl->mp_model->m_cpucoredata.m_wMaxFreqInMHz ;
        mp_cpucontroller->mp_model->m_cpucoredata.m_wMaxFreqInMHz ;
      WORD wCurrentFreqInMHz ;

      for( WORD wCurrentXcoordinateInDiagram = //wXcoordOfBeginOfYaxis 
        //0 
        //Begin with 1 to avoid div by zero.
        1 ; 
        wCurrentXcoordinateInDiagram < //wxcoordWidth 
        m_wDiagramWidth ; ++ wCurrentXcoordinateInDiagram 
        )
      {
        wCurrentFreqInMHz = (float) wMaxFreqInMHz / 
          ( (float) m_wDiagramWidth / (float) wCurrentXcoordinateInDiagram ) ;
        ////mp_pumastatectrl->GetMinFreqToPreventOvervoltage( iter ) ;
        ////fVoltage = //1.05f - 
        //mp_pumastatectrl->GetMaxVoltageForFreq( (float) wMaxFreqInMHz /
        //   ( (float) m_wDiagramWidth / (float) wCurrentXcoordinateInDiagram )
        //   , fVoltage 
        //  );
        if( ciHigher->m_wFreqInMHz < wCurrentFreqInMHz )
        {
          std::set<VoltageAndFreq>::const_iterator ciBeyondHigher = ciHigher ;
          ++ ciBeyondHigher ;
          if( ciBeyondHigher != mp_cpucoredata->m_setloweststablevoltageforfreq.end() )
          {
            ++ ciLower ;
            ++ ciHigher ;
            //-- ciLower ;
            //-- ciHigher ; 
          }
        }
        //If current freq is in between.
        if( ciHigher != mp_cpucoredata->m_setloweststablevoltageforfreq.end()
          && ciLower->m_wFreqInMHz <= wCurrentFreqInMHz && 
          ciHigher->m_wFreqInMHz >= wCurrentFreqInMHz 
          )
        {
          //mp_pumastatectrl->GetInterpolatedVoltageFromFreq( 
          //  wCurrentFreqInMHz
          //  , *ciHigher
          //  , *ciLower
          //  , fVoltage
          //  ) ;
          mp_cpucontroller->GetInterpolatedVoltageFromFreq(
            wCurrentFreqInMHz 
            , fVoltage
            , mp_cpucoredata->m_setloweststablevoltageforfreq
            ) ;
          if( m_bRangeBeginningFromMinVoltage )
            wYcoordinate =
              m_wDiagramHeight -
              ( fVoltage - m_fMinVoltage )
              / m_fMaxMinusMinVoltage * m_wDiagramHeight ;
          else
            wYcoordinate = m_wDiagramHeight - 
              fVoltage/ //(*iterstdvecmaxvoltageforfreq).m_fVoltageInVolt 
              fMaxVoltage
              * m_wDiagramHeight ;
          //p_wxpaintdc->DrawLine( 
          wxdc.DrawLine( 
            wCurrentXcoordinateInDiagram + m_wXcoordOfBeginOfYaxis,
            //200-fVoltage*100, 
            //wDiagramHeight - 
            //fVoltage/ (*iterstdvecmaxvoltageforfreq).m_fVoltageInVolt 
            //* wDiagramHeight ,
            wYcoordinate ,
            wCurrentXcoordinateInDiagram + m_wXcoordOfBeginOfYaxis
            //"+ 1" because: http://docs.wxwidgets.org/stable/wx_wxdc.html#wxdcdrawline:
            //"Note that the point (x2, y2) is not part of the line and is 
            //not drawn by this function (this is consistent with the 
            //behaviour of many other toolkits)."
            + 1
            , //(200-fVoltage*100) 
            wYcoordinate
            //"+ 1" because: http://docs.wxwidgets.org/stable/wx_wxdc.html#wxdcdrawline:
            //"Note that the point (x2, y2) is not part of the line and is 
            //not drawn by this function (this is consistent with the 
            //behaviour of many other toolkits)."
            + 1 
            ) ;
        }
      }// for
    }
  }
}

void MainFrame::DrawOvervoltageProtectionCurve(
    //wxPaintDC * p_wxpaintdc
    wxDC & wxdc
//    , WORD wDiagramWidth
//    , WORD wDiagramHeight
    , float fMaxVoltage
//    , WORD wXcoordOfBeginOfYaxis
    )
{
    float fVoltage ;
    WORD wYcoordinate ;
    WORD wMaxFreqInMHz = //mp_pumastatectrl->GetMaximumFrequencyInMHz() ;
      //mp_pumastatectrl->mp_model->m_cpucoredata.m_wMaxFreqInMHz ;
      mp_cpucontroller->mp_model->m_cpucoredata.m_wMaxFreqInMHz ;
    for( WORD wCurrentXcoordinateInDiagram = //wXcoordOfBeginOfYaxis 
        //0 
        //Begin with 1 to avoid div by zero.
        1 ; 
        wCurrentXcoordinateInDiagram < //wxcoordWidth 
        m_wDiagramWidth ; ++ wCurrentXcoordinateInDiagram )
    {
        //mp_pumastatectrl->GetMinFreqToPreventOvervoltage( iter ) ;
        //fVoltage = //1.05f - 
          //p_cpucoredata->m_fVoltageForMaxCPUcoreFreq -
          //(float) 
          //( 
          //  //log2f( 
          //  //MSVC++ has no log2() function (in <math.h>).
          //  //So emulate it by:  log(2)x = (log(10)x)/(log(10)2)
          //  log10
          //  (
          //    //(double) 
          //    ( //2200.0f 
          //      (float) wMaxFreqInMHz
          //      / (float) 
          //      ( 
          //        wMaxFreqInMHz / ( 400.0f / (float) wCurrentXcoordinateInDiagram) 
          //      ) 
          //    )
          //  )
          //  / log10(2.0f)
          //  * 0.25f 
          //) ;
          //TODO just for gettint Pentium M working fast
          //mp_pumastatectrl->GetMaxVoltageForFreq( (float) wMaxFreqInMHz /
          //   ( (float) m_wDiagramWidth / (float) wCurrentXcoordinateInDiagram )
          //   , fVoltage 
          //  );
        mp_cpucontroller->GetInterpolatedVoltageFromFreq(
          (float) wMaxFreqInMHz /
             ( (float) m_wDiagramWidth / (float) wCurrentXcoordinateInDiagram )
          , fVoltage
          , mp_cpucoredata->m_stdsetvoltageandfreqDefault ) ;
        if( m_bRangeBeginningFromMinVoltage )
          wYcoordinate =
            m_wDiagramHeight -
            ( fVoltage - m_fMinVoltage )
            / m_fMaxMinusMinVoltage * m_wDiagramHeight ;
        else
          wYcoordinate = m_wDiagramHeight - 
            fVoltage/ //(*iterstdvecmaxvoltageforfreq).m_fVoltageInVolt 
            fMaxVoltage
            * m_wDiagramHeight ;
        //p_wxpaintdc->DrawLine( 
        wxdc.DrawLine( 
            wCurrentXcoordinateInDiagram + m_wXcoordOfBeginOfYaxis,
            //200-fVoltage*100, 
            //wDiagramHeight - 
            //fVoltage/ (*iterstdvecmaxvoltageforfreq).m_fVoltageInVolt 
            //* wDiagramHeight ,
            wYcoordinate ,
            wCurrentXcoordinateInDiagram + m_wXcoordOfBeginOfYaxis
            //"+ 1" because: http://docs.wxwidgets.org/stable/wx_wxdc.html#wxdcdrawline:
            //"Note that the point (x2, y2) is not part of the line and is 
            //not drawn by this function (this is consistent with the 
            //behaviour of many other toolkits)."
            + 1
            , //(200-fVoltage*100) 
            wYcoordinate
            //"+ 1" because: http://docs.wxwidgets.org/stable/wx_wxdc.html#wxdcdrawline:
            //"Note that the point (x2, y2) is not part of the line and is 
            //not drawn by this function (this is consistent with the 
            //behaviour of many other toolkits)."
            + 1 
            ) ;
    }
}

void MainFrame::DrawCurrentPstateInfo(
  wxDC & r_wxdc
  )
{
   //DWORD dwEDX ;
   //DWORD dwLowmostBitsCurrentLimitRegister, dwLowmostBits ;
   //ULONGLONG ull ;
//   PState pstate ;

//#ifdef _DEBUG
//   ClocksNotHaltedCPUcoreUsageGetter & getter = 
//	   
//   ClocksNotHaltedCPUcoreUsageGetterPerCoreAtts * ar_cnh_cpucore_ugpca = 
//	   ;
//#endif
   //TODO respect # of cpu cores
   for ( BYTE byCPUcoreID = 0 ; byCPUcoreID < 
     mp_cpucoredata->m_byNumberOfCPUCores ; ++ byCPUcoreID )
   {
     //mp_pumastatectrl->RdmsrEx(P_STATE_CURRENT_LIMIT_REGISTER,
     // dwLowmostBitsCurrentLimitRegister, dwEDX , 1 << byCPUcoreID ) ;
     //if( mp_pumastatectrl->RdmsrEx(COFVID_STATUS_REGISTER, 
     //  dwLowmostBits, dwEDX , 1 << byCPUcoreID )
     //  )
     //{
     WORD wFreqInMHz ;
     float fVoltageInVolt ;
     if( mp_cpucontroller->GetCurrentPstate(wFreqInMHz, fVoltageInVolt, byCPUcoreID ) )
     {
       //mp_cpucontroller->
       ::wxGetApp().mp_cpucoreusagegetter->
         GetPercentalUsageForAllCores(
         mp_cpucoredata->m_arfCPUcoreLoadInPercent) ;
       float fCPUload = mp_cpucoredata->m_arfCPUcoreLoadInPercent[ byCPUcoreID ] ;
#ifdef _DEBUG
       if ( fCPUload == 0.0 )
       {
         int i = 0 ;
       }
#endif
        //mp_pumastatectrl->GetVIDmFIDnDID(dwLowmostBits,pstate) ;
        //wxmemorydc
        r_wxdc.DrawText(
          wxString::Format(
            //We need a _T() macro (wide char-> L"", char->"") for EACH 
            //line to make it compitable between char and wide char.
            _T("Core %u: ")
              //"PstateMaxVal:%u "
              //"CurPstateLimit:%u "
            _T("current p-state: ")
              //Pentium Ms do not have a p-state ID
              //"%u "
              //"VID:%u->"
            //.4f : 4 digits after comma
            _T("%.4f Volt ")
              //"FID:%u DID:%u->
            _T("%u MHz")
            //.3f : 3 digits after comma
#ifdef _WINDOWS
			      _T(" usage in percent:%.3f")
#else
            //when compiled with MSVC and running under WinXP the executable 
            //crashes with this format string (surely because of the 1st "%")
			      _T(" usage in %:%.3f")
#endif
			      //"counter val.:%I64u"
			      ,
            (WORD) byCPUcoreID ,
            ////"6:4 PstateMaxVal: P-state maximum value."
            //(dwLowmostBitsCurrentLimitRegister >> 4 ) &
            //  BITMASK_FOR_LOWMOST_3BIT ,
            ////"2:0 CurPstateLimit: current P-state limit."
            ////dwLowmostBitsCurrentLimitRegister & BITMASK_FOR_LOWMOST_3BIT ,
            ////AMD: "18:16 CurPstate: current P-state."
            //(dwLowmostBits >> 16 ) & BITMASK_FOR_LOWMOST_3BIT ,
            //pstate.m_byVoltageID, 
            //pstate.GetVoltageInVolt() ,
            fVoltageInVolt ,
            //pstate.m_byFreqID, 
            //pstate.m_byDivisorID, 
            //pstate.GetFreqInMHz() ,
            wFreqInMHz ,
			      //mp_cpucoredata->m_arfCPUcoreLoadInPercent[ byCPUcoreID ]
            fCPUload * 100.0f
			      //, m_clocksnothaltedcpucoreusagegetter.m_ar_cnh_cpucore_ugpca[
			      //	byCPUcoreID].m_ullPreviousPerformanceEventCounter3
          )
          , 10
          , //90 
          m_wDiagramHeight - ( 20 * 
            (mp_cpucoredata->m_byNumberOfCPUCores + 1 ) ) + 
            ( byCPUcoreID * 20 )
          ) ;
       //ull = mp_pumastatectrl->GetCurrentCPUload( byCPUcoreID ) ;
       //(double) pstate.GetFreqInMHz() / 
       //  (double) mp_cpucoredata->m_wMaxFreqInMHz ;
     }
   } //for-loop
     
   //mp_pumastatectrl->RdmsrEx(P_STATE_CURRENT_LIMIT_REGISTER,
   // dwLowmostBitsCurrentLimitRegister, dwEDX , 2 ) ;
   //if( mp_pumastatectrl->RdmsrEx(COFVID_STATUS_REGISTER, dwLowmostBits, dwEDX , 2 )
   //  )
   //{
   //   PState pstate ;
   //   mp_pumastatectrl->GetVIDmFIDnDID(dwLowmostBits,pstate) ;
   //   wxmemorydc.DrawText(
   //     wxString::Format("Core 1: PstateMaxVal:%u "
   //       "CurPstateLimit:%u "
   //       "current p-state:%u VID:%u->%f Volt FID:%u DID:%u->%u MHz" ,
   //     ////"58:56 CurPstateLimit:"
   //     //"6:4 PstateMaxVal: P-state maximum value."
   //     (dwLowmostBitsCurrentLimitRegister >> 4 ) &
   //       BITMASK_FOR_LOWMOST_3BIT ,
   //     //"2:0 CurPstateLimit: current P-state limit."
   //     dwLowmostBitsCurrentLimitRegister & BITMASK_FOR_LOWMOST_3BIT ,
   //     //dwEDX >>
   //     //AMD: "18:16 CurPstate: current P-state."
   //     (dwLowmostBits >> 16 ) & BITMASK_FOR_LOWMOST_3BIT ,
   //     pstate.m_byVoltageID, pstate.GetVoltageInVolt() ,
   //     pstate.m_byFreqID, pstate.m_byDivisorID, pstate.GetFreqInMHz() 
   //     )
   //     , 10
   //     , 120
   //     ) ;
   //}
}

void MainFrame::DrawCurrentVoltageSettingsCurve(
    //wxPaintDC * p_wxpaintdc
    wxDC & wxdc
//    , WORD wDiagramWidth
//    , WORD wDiagramHeight
    , float fMaxVoltage
//    , WORD wXcoordOfBeginOfYaxis
    )
{
    float fVoltage ;
    WORD wYcoordinate ;
    WORD wMaxFreqInMHz = //mp_pumastatectrl->GetMaximumFrequencyInMHz() ;
      //mp_pumastatectrl->mp_model->m_cpucoredata.m_wMaxFreqInMHz ;
      mp_cpucontroller->mp_model->m_cpucoredata.m_wMaxFreqInMHz ;
    if( //If max. freq is assigned 
      wMaxFreqInMHz != 0 )
      for( WORD wCurrentXcoordinateInDiagram = //wXcoordOfBeginOfYaxis
          //0
          //Begin with 1 to avoid div by zero.
          1 ;
          wCurrentXcoordinateInDiagram < //wxcoordWidth
          m_wDiagramWidth ; ++ wCurrentXcoordinateInDiagram )
      {
          //mp_pumastatectrl->GetMinFreqToPreventOvervoltage( iter ) ;
          //fVoltage = //1.05f -
            //p_cpucoredata->m_fVoltageForMaxCPUcoreFreq -
            //(float)
            //(
            //  //log2f(
            //  //MSVC++ has no log2() function (in <math.h>).
            //  //So emulate it by:  log(2)x = (log(10)x)/(log(10)2)
            //  log10
            //  (
            //    //(double)
            //    ( //2200.0f
            //      (float) wMaxFreqInMHz
            //      / (float)
            //      (
            //        wMaxFreqInMHz / ( 400.0f / (float) wCurrentXcoordinateInDiagram)
            //      )
            //    )
            //  )
            //  / log10(2.0f)
            //  * 0.25f
            //) ;
        //TODO uncomment
         //if( mp_pumastatectrl->GetInterpolatedVoltageFromFreq
         //    (
         //     (float) wMaxFreqInMHz /
         //     ( (float) m_wDiagramWidth / (float) wCurrentXcoordinateInDiagram )
         //     , fVoltage
         //    )
         //   )
         //{
        if( mp_cpucontroller->GetInterpolatedVoltageFromFreq(
            (float) wMaxFreqInMHz /
               ( (float) m_wDiagramWidth / (float) wCurrentXcoordinateInDiagram )
            , fVoltage
            , mp_cpucoredata->m_stdsetvoltageandfreqWanted )
          )
        {
          if( m_bRangeBeginningFromMinVoltage )
            wYcoordinate =
              m_wDiagramHeight -
              ( fVoltage - m_fMinVoltage )
              / m_fMaxMinusMinVoltage * m_wDiagramHeight ;
          else
            wYcoordinate = m_wDiagramHeight -
              fVoltage / //(*iterstdvecmaxvoltageforfreq).m_fVoltageInVolt
              fMaxVoltage
              * m_wDiagramHeight ;
          //p_wxpaintdc->DrawLine(
          wxdc.DrawLine(
              wCurrentXcoordinateInDiagram + m_wXcoordOfBeginOfYaxis,
              //200-fVoltage*100,
              //wDiagramHeight -
              //fVoltage/ (*iterstdvecmaxvoltageforfreq).m_fVoltageInVolt
              //* wDiagramHeight ,
              wYcoordinate ,
              wCurrentXcoordinateInDiagram + m_wXcoordOfBeginOfYaxis
              //"+ 1" because: http://docs.wxwidgets.org/stable/wx_wxdc.html#wxdcdrawline:
              //"Note that the point (x2, y2) is not part of the line and is
              //not drawn by this function (this is consistent with the
              //behaviour of many other toolkits)."
              + 1
              , //(200-fVoltage*100)
              wYcoordinate
              //"+ 1" because: http://docs.wxwidgets.org/stable/wx_wxdc.html#wxdcdrawline:
              //"Note that the point (x2, y2) is not part of the line and is
              //not drawn by this function (this is consistent with the
              //behaviour of many other toolkits)."
              + 1
              ) ;
        }
      }//for-loop
}

void MainFrame::DrawVoltageFreqCross(
  wxDC & r_wxdc
  , float fVoltageInVolt
  , WORD wFreqInMHz
  , const wxColor * cp_wxcolor 
  )
{
  WORD wXcoordinate = 
    m_wXcoordOfBeginOfYaxis +
    (float) //(*iterstdvecmaxvoltageforfreq).m_wFreqInMHz / 
    //(*iterstdvecmaxvoltageforfreq).m_wFreqInMHz / 
    wFreqInMHz / 
    (float) mp_cpucoredata->m_wMaxFreqInMHz * m_wDiagramWidth ;
  WORD wYcoordinate ;
  if( m_bRangeBeginningFromMinVoltage )
    wYcoordinate =
      m_wDiagramHeight -
      ( fVoltageInVolt - m_fMinVoltage )
      / m_fMaxMinusMinVoltage * m_wDiagramHeight ;
  else
    wYcoordinate =
      m_wDiagramHeight - 
      //(*iterstdvecmaxvoltageforfreq).m_fVoltageInVolt 
      fVoltageInVolt
      / m_fMaxVoltage * m_wDiagramHeight ;

  //wxPen pen(*wxBLUE, 3); // pen of width 3
  wxPen pen(*cp_wxcolor, 3); // pen of width 3
  r_wxdc.SetPen(pen);
  //Draw Cursor:
  r_wxdc.DrawLine( 
    wXcoordinate - 3, wYcoordinate , 
    wXcoordinate + 4, wYcoordinate  
    ) ;
  r_wxdc.DrawLine( 
    wXcoordinate , wYcoordinate - 3, 
    wXcoordinate , wYcoordinate + 4 
    ) ;
}

//http://www.informit.com/articles/article.aspx?p=405047:
//[...]another thing you can do to make drawing smoother (particularly 
//when resizing) is to paint the background in your paint handler, and 
//not in an erase background handler. All the painting will then be 
//done in your buffered paint handler, so you don't see the background 
//being erased before the paint handler is called. Add an empty erase 
//background handler[...]
// ->Empty implementation, to prevent flicker
void MainFrame::OnEraseBackground(wxEraseEvent& event)
{
}

void MainFrame::OnPaint(wxPaintEvent & event)
{
//// Create a memory DC
//  wxMemoryDC temp_dc;
//  temp_dc.SelectObject(test_bitmap);
//
//  // We can now draw into the memory DC...
//  // Copy from this DC to another DC.
//  old_dc.Blit(250, 50, BITMAP_WIDTH, BITMAP_HEIGHT, temp_dc, 0, 0);

  wxPaintDC * p_wxpaintdc = new wxPaintDC(this);
    //dc->SetBackground(*wxBLUE_BRUSH);
    wxPoint arwxpoint[3] ;
    arwxpoint[0] = wxPoint(100, 
        //Because the y-origin is top left.
        200-80 ) ;
    arwxpoint[1] = wxPoint(200, 
        //Because the y-origin is top left.
        200-95) ;
    arwxpoint[2] = wxPoint(400, 
        //Because the y-origin is top left.
        200-110) ;

    //p_wxpaintdc->DrawSpline(3, arwxpoint ) ;
  //for( std::vector<MaxVoltageForFreq>::iterator iter = 
  //  //m_model.m_vecmaxvoltageforfreq.begin() ; 
  //  
  //  //The first max voltage for freq pair ought to be the 
  //  //highest/ maximum freq and its default voltage.
  //  mp_pumastatectrl->mp_model->m_vecmaxvoltageforfreq.begin() ; 
  //  iter != //m_model.m_vecmaxvoltageforfreq.end() ;
  //    mp_pumastatectrl->mp_model->m_vecmaxvoltageforfreq.end() ;
  //  ++ iter )
  //{
  //}
    //float fVoltage ;
    CPUcoreData * p_cpucoredata = & //mp_pumastatectrl->mp_model->m_cpucoredata ;
      mp_cpucontroller->mp_model->m_cpucoredata ;
    WORD wMaxFreqInMHz = //mp_pumastatectrl->GetMaximumFrequencyInMHz() ;
      //mp_pumastatectrl->mp_model->m_cpucoredata.m_wMaxFreqInMHz ;
      mp_cpucontroller->mp_model->m_cpucoredata.m_wMaxFreqInMHz ;
    //std::vector<MaxVoltageForFreq>::iterator iterstdvecmaxvoltageforfreq = 
    //std::set<MaxVoltageForFreq>::iterator iterstdsetmaxvoltageforfreq = 
    //  mp_pumastatectrl->mp_model->//m_vecmaxvoltageforfreq
    //  m_setmaxvoltageforfreq.begin() ;
    
    //wxMemoryDC wxmemorydc(wxbitmap) ;
    wxBufferedPaintDC wxmemorydc( this //, wxbitmap
      , *mp_wxbitmap
      ) ;

    //http://www.informit.com/articles/article.aspx?p=405047:
    // Shifts the device origin so we don't have to worry
    // about the current scroll position ourselves
    PrepareDC(wxmemorydc);

    //http://docs.wxwidgets.org/stable/wx_wxmemorydc.html#wxmemorydc:
    //"Use the IsOk member to test whether the constructor was 
    //successful in creating a usable device context."
    if( wxmemorydc.IsOk() )
    {
      wxCoord wxcoordCanvasWidth ;
      wxCoord wxcoordCanvasHeight ;
      p_wxpaintdc->GetSize( & wxcoordCanvasWidth , & wxcoordCanvasHeight ) ;
      //m_wDiagramHeight = wxcoordCanvasHeight 
      //    //Space for drawing scale and scale values below the x-axis.
      //    - 50 ;
      ////WORD wXcoordOfBeginOfYaxis = 50 ;
      //m_wXcoordOfBeginOfYaxis = 50 ;
      ////WORD wDiagramWidth = wxcoordCanvasWidth - wXcoordOfBeginOfYaxis - 30 ;
      //m_wDiagramWidth = wxcoordCanvasWidth - m_wXcoordOfBeginOfYaxis - 30 ;
      //WORD wYcoordinate ;
      //WORD wXcoordinate ;
      //Clears the device context using the current background brush. 
      //(else black background?)
      wxmemorydc.Clear();
      //TODO maybe "dc.Blit(dcDiagramScaleAndCurves)" is faster than DrawBitmap.
      //wxmemorydc.Blit(0, 0, wxcoordCanvasWidth , wxcoordCanvasHeight, 
      //  & m_wxbufferedpaintdcStatic, 0, 0 );
      //if( mp_wxbufferedpaintdcStatic )
        wxmemorydc.Blit(0, 0, wxcoordCanvasWidth , wxcoordCanvasHeight, 
          //mp_wxbufferedpaintdcStatic
          & m_wxmemorydcStatic , 0, 0 );
      //wxmemorydc.DrawBitmap( //mp_wxbitmapDiagramScaleAndCurves 
      //  *mp_wxbitmapStatic , 0, 0) ;
      //if( //iterstdvecmaxvoltageforfreq != mp_pumastatectrl->mp_model->
      //  iterstdsetmaxvoltageforfreq != mp_pumastatectrl->mp_model->
      //  //m_vecmaxvoltageforfreq.end() 
      //  m_setmaxvoltageforfreq.end()
      //  )
      //{
      //    m_fMaxVoltage = //(*iterstdvecmaxvoltageforfreq).m_fVoltageInVolt ;
      //      //P-state 0 usually has the highest voltage.
      //      (*iterstdsetmaxvoltageforfreq).m_fVoltageInVolt ;
      //    //http://docs.wxwidgets.org/stable/wx_wxdc.html#wxdcclear:
      //    //"Clears the device context using the current background brush."
      //    //p_wxpaintdc->Clear();
      //    //mp_wxbitmap = new wxBitmap( 
      //    //    wxcoordCanvasWidth, 
      //    //    wxcoordCanvasHeight , 
      //    //    //http://docs.wxwidgets.org/stable/wx_wxbitmap.html#wxbitmapctor:
      //    //    //"A depth of -1 indicates the depth of the current screen or visual."
      //    //    -1) ;

      //    //    p_wxpaintdc->Blit(
      //    //        wXcoordOfBeginOfYaxis, 
      //    //        0, 
      //    //        wDiagramWidth, 
      //    //        wDiagramHeight, 
      //    //        &wxmemorydc, 
      //    //        0, 
      //    //        0);
      //    //}
      //    DrawDiagramScale( wxmemorydc, iterstdsetmaxvoltageforfreq) ;
      //}
  //    DrawOvervoltageProtectionCurve(
  //        //p_wxpaintdc,
  //        wxmemorydc ,
  //        //wDiagramWidth,
  ////                m_wDiagramWidth,
  //        //wDiagramHeight,
  ////                m_wDiagramHeight,
  //        m_fMaxVoltage
  ////                m_wXcoordOfBeginOfYaxis
  //        ) ;
  //    DrawCurrentVoltageSettingsCurve(
  //        wxmemorydc ,
  //        m_fMaxVoltage
  //        ) ;

        //p_wxpaintdc->Blit(
        //    0, 
        //    0, 
        //    wxcoordCanvasWidth, 
        //    wxcoordCanvasHeight, 
        //    &wxmemorydc, 
        //    0, 
        //    0);
        //float f
       DWORD dwLowmostBits;
//#ifndef _TEST_PENTIUM_M
       DrawCurrentPstateInfo(wxmemorydc) ;
//#endif //#ifndef _TEST_PENTIUM_M
       ULONGLONG ull ;
       DWORD dwHighmostBits //, dwLowmostBits 
         ;
       //mp_pumastatectrl->RdmsrEx(
       //   MSR_TIME_STAMP_COUNTER_REGISTER,
       //   dwLowmostBits,// bit  0-31 (register "EAX")
	      //  dwHighmostBits, 
       //   1 
       //   ) ;

        ull = dwHighmostBits ;
        ull <<= 32 ;
        ull |= dwLowmostBits ;

        ULONGLONG ullTimeStampCounterValueDiff = ull - 
          m_ullPreviousTimeStampCounterValue;
        if( b_NotFirstTime )
        {
          wxmemorydc.DrawText( //wxString::Format(
            //"%f"
            //We need a _T() macro (wide char-> L"", char->"") for EACH 
            //line to make it compitable between char and wide char.
            _T("TimeStampC value diff:")
            , 0
            , 90
            ) ;
          wxmemorydc.DrawText( 
            wxString::Format(
              //"%f"
              //We need a _T() macro (wide char-> L"", char->"") for EACH 
              //line to make it compitable between char and wide char.
              //print at least 10 chars
              _T("%10I64u") ,
              //ull - m_ullPreviousTimeStampCounterValue 
              ullTimeStampCounterValueDiff
              )
            , 150
            , 90
            ) ;
        }
        m_ullPreviousTimeStampCounterValue = ull ;
            //CPU_USAGE cpu_usage ;
           
//            wxmemorydc.DrawText(
//              wxString::Format(
//    //            "%64u"
//                //"%f"
//                //We need a _T() macro (wide char-> L"", char->"") for EACH
//                //line to make it compatible between char and wide char.
//                _T("core 0 cpu_usage: ")
//                _T("%d")
//                //"%f"
//                ,
//                //ull
//                //GetCpuUsage( 0 , & cpu_usage )
//                //wxGetApp().mp_wxDynLinkedCPUcoreUsageGetter->mp_wxDynLinkedCPUcoreUsageGetter
//                //mp_model->mp_cpucoredata
//                (int)mp_cpucoredata->m_arfCPUcoreLoadInPercent[0]
//                //mp_pumastatectrl
//                //m_cpucoreusagentqsi.GetCpuUsageNT(0 , & cpu_usage )
//              )
//              , 50
//              , 50
//              ) ;

       //ull = mp_pumastatectrl->GetCurrentCPUload(1) ;
    //    double d = ull ;

    //    wxmemorydc.DrawText( wxString::Format(
    //      //"%f"
    //      "%I64u",
    //      //f
    //      //d
    //      ull )
    //        , 50
    //        , 20
    //        ) ;
    //    double dDiff ;
        ULONGLONG ullDiff ;
    //    if( //If value overflow.
    //      ull < m_ullPreviousCPUusage )
    //      //dDiff =
    //      ullDiff = //0 
    //        //If the value rangs WAS 0..255:
    //        //    if current value = 10 and previos value = 250: 
    //        //  oveflow occured. So the correct value is 255-250+10=5+10=15
    //        _UI64_MAX - m_ullPreviousCPUusage + ull ;
    //    else
          //ULONGLONG ullPerformanceEventCounter2 ;
          //ULONGLONG ullPerformanceEventCounter3 ;
            DWORD dwAffinityMask = 1 ; //1= only CPU core 0
#ifdef _TEST_GRIFFIN
          //These values must be fetched even for the 1st:
          //else the difference between 2 values is wrong/ too high
          mp_pumastatectrl->ReadPerformanceEventCounterRegister(
            0 ,
            ull ,
            dwAffinityMask ) ;
          mp_pumastatectrl->ReadPerformanceEventCounterRegister(
            1 ,
            ullPerformanceEventCounter2 ,
            dwAffinityMask ) ;
          mp_pumastatectrl->ReadPerformanceEventCounterRegister(
            2 ,
            ullPerformanceEventCounter3 ,
            dwAffinityMask ) ;
          if ( //at the 1st time prev valu is 0--so the difference 
            //to the current valiue can be too high
            b_NotFirstTime )
          {
            //BYTE byCPUcoreID = 0 ;
            DWORD dwHigh, dwLow ; 
            //mp_pumastatectrl->//mp_controller->
            //  RdmsrEx(
            //  //PERF_CTR_0,
            //  PERFORMANCE_EVENT_COUNTER_0_REGISTER ,
            //  //PERFORMANCE_EVENT_COUNTER_1_REGISTER ,
            //  dwLow,
            //  dwHigh,
            //  //1=core 0
            //  //1
            //  //byCPUcoreID 
            //  dwAffinityMask
            //  ) ;
            //mp_pumastatectrl->AccuratelyStartPerformanceCounting( 
            //  0, 
            //  PERFORMANCE_EVENT_SELECT_RETIRED_INSTRUCTIONS 
            //  ) ;
            //mp_pumastatectrl->AccuratelyStartPerformanceCounting( 
            //  1, 
            //  CPU_CLOCKS_NOT_HALTED 
            //  ) ;
            //        //dDiff
            //ullDiff = ull - m_ullPreviousCPUusage ;
            ullDiff = PERFORMANCE_COUNTER_VALUE_DIFF( ull , 
              m_ullPreviousCPUusage ) ;
            ULONGLONG ullPerformanceEventCounter2Diff = 
              PERFORMANCE_COUNTER_VALUE_DIFF( ullPerformanceEventCounter2 , 
              m_ullPreviousPerformanceEventCounter2 ) ;
            ULONGLONG ullPerformanceEventCounter3Diff = 
              PERFORMANCE_COUNTER_VALUE_DIFF( ullPerformanceEventCounter3 , 
              m_ullPreviousPerformanceEventCounter3 ) ;
            if( ullDiff > m_ullHighestDiff )
              m_ullHighestDiff = ullDiff ;
            if( ullPerformanceEventCounter2Diff > 
              m_ullHighestPerformanceEventCounter2Diff 
              )
              m_ullHighestPerformanceEventCounter2Diff = 
                ullPerformanceEventCounter2Diff ;
            wxmemorydc.DrawText(
                //"%f"
                "clocks not halted diff:"
              , 0
              , 110
              ) ;
            wxmemorydc.DrawText(
              wxString::Format(
                //print at least 10 chars
                "%10I64u"
                ,
                //ullPerformanceEventCounter2Diff
                ullPerformanceEventCounter3Diff
                )
              , 150
              , 110
              ) ;
              double dClocksNotHaltedDiffDivTCSdiff =
                (double) ullPerformanceEventCounter3Diff /
                (double) ullTimeStampCounterValueDiff ;
            wxmemorydc.DrawText(
              wxString::Format(
                //print at least 10 chars
                "clocks not halted diff / TimeStampCounter diff: %0.2f"
                ,
                //ullPerformanceEventCounter2Diff
                dClocksNotHaltedDiffDivTCSdiff
                )
              , 0
              , 130
              ) ;
            wxmemorydc.DrawText(
              wxString::Format(
                //"%f"
                //"ret instr diff / clocks not halted diff: %f"
                "FPU load = " //"FPU usage: "
                //"\"( clocks not halted diff\"(%I64u) - "
                //"\"cycles with empty FPU diff\"(%I64u) )(%I64u) / "  //"max. usage:"
                "cycles at least 1 FPU op(%I64u) / "
                //"clocks not halted diff(%I64u) = %f"
                "TSC diff diff(%I64u) = %f"
                ,
                //ullPerformanceEventCounter3Diff ,
                ullPerformanceEventCounter2Diff ,
                //( ullPerformanceEventCounter3Diff - 
                //  ullPerformanceEventCounter2Diff ) ,
                //m_ullHighestPerformanceEventCounter2Diff,
                //ullPerformanceEventCounter3Diff ,
                ullTimeStampCounterValueDiff ,
                (double) //ullDiff 
                //( ullPerformanceEventCounter3Diff - 
                  ullPerformanceEventCounter2Diff //)
                / (double) 
                //ullPerformanceEventCounter2Diff
                //m_ullHighestPerformanceEventCounter2Diff
                //ullPerformanceEventCounter3Diff
                ullTimeStampCounterValueDiff
              )
              , 50
              , 150
              ) ;
            //        dDiff = ullDiff ;
            wxmemorydc.DrawText(
              wxString::Format(
    //            "%64u"
                //"%f"
                "perf ctr diff: %I64u"
                ,
                //f
                //d - //m_fPreviousCPUusage
                //ull - m_ullPreviousCPUusage
                //dDiff
                //ull
                ullDiff
              )
              , 50
              , 170
              ) ;
            wxmemorydc.DrawText(
              wxString::Format(
    //            "%64u"
                //"%f"
                "perf ctr: "
                "%I64u"
                //"%u"
                ,
                //f
                //d - //m_fPreviousCPUusage
                //ull - m_ullPreviousCPUusage
                //dDiff
                ull
                //dwLow
                //ullDiff
              )
              , 50
              , 30
              ) ;    //        DWORD dwValue ;

    //        //fVoltageOfOvVoltProtVnf_pairHigherEqualWantedFreq
    //        //p_maxvoltageforfreqFreqAbove->m_fVoltageInVolt -
    //          = 1.062 -
    //          mp_pumastatectrl->log_dualis
    //            (//wFreqOfOvVoltProtVnf_pairHigherEqualWantedFreq
    //                (double) //p_maxvoltageforfreqFreqAbove->m_wFreqInMHz /
    //                2200 /
    //                (double) //wFreqToGetMaxVoltageFor
    //                2000
    //            )
    //              *
    //                //fVoltageDiffBetwOvVoltProtVnf_pairs
    //                ( //p_maxvoltageforfreqFreqAbove->m_fVoltageInVolt -
    //                1.062 -
    //                //p_maxvoltageforfreqFreqBelow->m_fVoltageInVolt
    //                0.85
    //            )
    //          ;

            ////dPercentalFreq = (double) m_wFreqInMHz / 
            ////  (double) mp_cpucoredata->m_wMaxFreqInMHz ;
            //////If e.g. running at 1/4 of max. freq the maxium performance
            //////counter value difference can just be 1/4 of the maximum 
            ////// possible (=when runnung at highest frequency ) performance 
            //////  counter value difference!
            ////dMaximumPerfCounterDiffForCurrentFreq = dPercentalFreq 
            ////  * (double) m_ullHighestDiff ;
            double dCPUloadInPercent = (double) ullDiff  /
              //dMaximumPerfCounterDiffForCurrentFreq ;
              (double) 
              //m_ullHighestDiff ;
              //ullPerformanceEventCounter3Diff ;
              ullTimeStampCounterValueDiff ;
            wxmemorydc.DrawText(
              wxString::Format("ALU load: %0.2f" , dCPUloadInPercent )
                , 270
                , 90
                ) ;
            //wxmemorydc.DrawText(
            //  wxString::Format("max. diff: %I64u" , m_ullHighestDiff )
            //    , 270
            //    , 90
            //    ) ;
            //wxmemorydc.DrawText(
            //  wxString::Format(//"diff"
            //    "retired microops/s : %I64u" , ullDiff )
            //    , 270
            //    , 110
            //    ) ;

            //if( //If thes window has the focus 
            //  IsActive() )
            //{
              if ( m_fPreviousCPUusage != 0.0f )
              {
                if ( dClocksNotHaltedDiffDivTCSdiff > 
                  m_fPreviousCPUusage * 0.75 //* 1.5f 
                  )
                {
                  m_fPreviousCPUusage = dClocksNotHaltedDiffDivTCSdiff ;
                  //raise the freq.
                  dClocksNotHaltedDiffDivTCSdiff *= 1.5 ;
                }
                else
                  m_fPreviousCPUusage = dClocksNotHaltedDiffDivTCSdiff ;
              }
              else
                m_fPreviousCPUusage = dClocksNotHaltedDiffDivTCSdiff ;

              //wxmemorydc.DrawText(
              //    wxString::Format("%0.2f" , fFreqInMHz )
              //    , 170
              //    , 70
              //    ) ;

                //wxmemorydc.DrawText(
                //  wxString::Format("Freq by load:"
                //  //"%f"
                //  "%u"
                //  "voltage: %0.4f" , 
                //  //fFreqInMHz
                //  //wFreqInMHz
                //  pstate.GetFreqInMHz(), 
                //  //fVoltageInVolt 
                //  pstate.GetVoltageInVolt() 
                //  )
                    //, 200
                    //, 200
                    //) ;
             //}
             //else
             //   wxmemorydc.DrawText(
             //   wxString::Format("curr freq:%f NO current voltage %0.4f" , 
             //     fFreqInMHz, fVoltageInVolt )
             //       , 200
             //       , 200
             //       ) ;

            if( //If this window has the focus 
              //IsActive() 
              ! m_vbAnotherWindowIsActive
              )
            {
              //m_wFreqInMHzOfCurrentActiveCoreSettings = wFreqInMHz ;
              //m_fVoltageInVoltOfCurrentActiveCoreSettings = fVoltageInVolt ;
            }
          }
          else // b_NotFirstTime == false 
            b_NotFirstTime = true ;
        //m_ullPreviousCPUusage = ull ;
        //m_ullPreviousPerformanceEventCounter2 = 
        //  ullPerformanceEventCounter2 ;
        //m_ullPreviousPerformanceEventCounter3 =
        //  ullPerformanceEventCounter3 ;
#endif //_TEST_GRIFFIN
      //Just for testing:
      //wXcoordinate = wYcoordinate = 50 ;
      if( //m_bDrawFreqAndVoltagePointForCurrCoreSettings 
        m_vbAnotherWindowIsActive 
        )
      {
        DrawVoltageFreqCross(
          wxmemorydc
          , m_fVoltageInVoltOfCurrentActiveCoreSettings
          , m_wFreqInMHzOfCurrentActiveCoreSettings
          , wxBLUE
          ) ;
      }
      else
      {
        float fVoltageInVolt ;
        WORD wFreqInMHz ;
        for( BYTE byCoreID = 0 ; byCoreID < mp_cpucoredata->
          GetNumberOfCPUcores() ; ++ byCoreID )
        {
          mp_cpucontroller->GetCurrentPstate(wFreqInMHz, fVoltageInVolt, byCoreID);
          PerCPUcoreAttributes & r_percpucoreattributes = mp_cpucoredata->
            m_arp_percpucoreattributes[ byCoreID] ;
          DrawVoltageFreqCross(
            wxmemorydc
            //, r_percpucoreattributes.m_fVoltageInVoltCalculatedFromCPUload
            , fVoltageInVolt
            //, r_percpucoreattributes.m_wFreqInMHzCalculatedFromCPUload 
            , wFreqInMHz
            , wxBLUE
            ) ;
        }
      }
      //m_bDrawFreqAndVoltagePointForCurrCoreSettings =
      //    ! m_bDrawFreqAndVoltagePointForCurrCoreSettings ;
//#ifndef _TEST_PENTIUM_M
//        //DWORD dwValue ;
//        float fTempInDegCelsius ;
//        if( mp_pumastatectrl->GetCurrentTempInDegCelsius(fTempInDegCelsius)
//          )
//          wxmemorydc.DrawText(
//            wxString::Format(_T("CPU temp.:%0.2f degrees C") ,
//              fTempInDegCelsius )
//              , 50
//              , 70
//              ) ;
//#endif //#ifndef _TEST_PENTIUM_M
#ifdef _COMPILE_WITH_CPU_CORE_USAGE_GETTER__
       float arf[2] ;
       //m_cpucoreusagegetter.GetPercentalUsageForAllCores(arf) ;
       m_cpucoreusagegetteriwbemservices.GetPercentalUsageForAllCores(arf) ;
          wxmemorydc.DrawText(
          wxString::Format("%f" , arf[0] )
              , 200
              , 70
              ) ;
      double dPassedNanos ;
      SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION ar_sysprocperfinfo [1] ;
      m_cpucoreusagegetter.GetSysProcPerfInfo(
        ar_sysprocperfinfo ,
        dPassedNanos
        ) ;
      LARGE_INTEGER liKernelMinusIdle ; 
      liKernelMinusIdle.QuadPart = 
        ar_sysprocperfinfo[0].KernelTime.QuadPart 
        - ar_sysprocperfinfo[0].IdleTime.QuadPart ;
      LARGE_INTEGER liOverall ; 
      liOverall.QuadPart = 
        //liKernelMinusIdle.QuadPart + 
        ar_sysprocperfinfo[0].KernelTime.QuadPart +
        ar_sysprocperfinfo[0].IdleTime.QuadPart + 
        ar_sysprocperfinfo[0].UserTime.QuadPart ;

          wxmemorydc.DrawText(
            wxString::Format("idle: %I64u kernel:%I64u user:%I64u "
              "kernel-idle:%I64u overall:%I64u" 
              ,
              ar_sysprocperfinfo[0].IdleTime.QuadPart , 
              ar_sysprocperfinfo[0].KernelTime.QuadPart, 
              ar_sysprocperfinfo[0].UserTime.QuadPart,
              liKernelMinusIdle.QuadPart ,
              liOverall.QuadPart
              )
              , 0
              , 170
              ) ;
          wxmemorydc.DrawText(
            wxString::Format(
            "idle:%f kernel:%f kernel-idle:%f user:%f dPassedNanos:%f",

              (double) ar_sysprocperfinfo[0].IdleTime.QuadPart / 
                (double) liOverall.QuadPart ,
              (double) ar_sysprocperfinfo[0].KernelTime.QuadPart / 
                (double) liOverall.QuadPart ,
              (double) liKernelMinusIdle.QuadPart / 
                (double) liOverall.QuadPart ,
              (double) ar_sysprocperfinfo[0].UserTime.QuadPart / 
                (double) liOverall.QuadPart ,
                dPassedNanos
              )
              , 0
              , 190
              ) ;
#endif //#ifdef _COMPILE_WITH_CPU_CORE_USAGE_GETTER
//        m_fPreviousCPUusage = d ;

        //m_ullPreviousCPUusage = ull ;

        //}
    }//if( wxmemorydc.IsOk() )

    delete p_wxpaintdc; 
}

//order of submenus/ menu items of "core x" menus.
enum 
{
  Settings = 0,
  highLoadThread
  , highFPUloadThread
  //setp_state0 ,
  //setp_state1 ,
  //setp_state2 ,
};

void MainFrame::Notify() //overrides wxTimer::Notify()
{
  m_bDrawFreqAndVoltagePointForCurrCoreSettings =
	! m_bDrawFreqAndVoltagePointForCurrCoreSettings ;
    Refresh() ;
}

void MainFrame::PossiblyAskForOSdynFreqScalingDisabling()
{
  if( //! //mp_pumastatectrl->m_bFrequencyScalingByOSDisabled 
    //mp_pumastatectrl->mp_dynfreqscalingaccess->OtherDVFSisEnabled() 
    //mp_cpucontroller->mp_dynfreqscalingaccess->OtherDVFSisEnabled()
    mp_cpucontroller->OtherPerfCtrlMSRwriteIsActive()
    )
    if (::wxMessageBox(
        //We need a _T() macro (wide char-> L"", char->"") for EACH 
        //line to make it compitable between char and wide char.
        _T("The OS's dynamic frequency scaling must be disabled ")
        _T("in order that the p-state isn' changed by the OS afterwards.")
        _T("If the OS's dynamic frequency isn't disabled, should it be done now?")
        , 
        //We need a _T() macro (wide char-> L"", char->"") for EACH 
        //line to make it compitable between char and wide char.
        _T("Question")
        , wxYES_NO | wxICON_QUESTION )
        == wxYES
       )
      //mp_pumastatectrl->DisableFrequencyScalingByOS();
      mp_cpucontroller->DisableFrequencyScalingByOS();
}

//#ifdef _TEST_PENTIUM_M
#ifdef COMPILE_WITH_MSR_EXAMINATION
  void MainFrame::OnMSR(wxCommandEvent& WXUNUSED(event))
  {
     //MSRdata msrdata(0x199);
     //RegisterData registerdataFID("FID");
     //RegisterData registerdataVolt("VID");
     ////example: registerdata kann be made up of e.g. msr reggister[5:0], reggister[10:8]
     //registerdataFID.add(8,4) ;
     //registerdataVolt.add(0,8) ;
     ////registerdataVolt.setTable("VID_to_Volt") ;
     ////registerdataMHz.setTable("FID_to_MHz") ;
     //msrdata.add(registerdataFID) ;
     //msrdata.add(registerdataVolt) ;
     //std::string stdstrFIDtoMHzTable [][2] = {
     // { "MHz","FID" } ,
     // { "600","6" } ,
     // { "800","8" } ,
     // { "900","9" } ,
     // { "1000","10" } ,
     // { "1100","11" } ,
     // { "1200","12" } ,
     // { "1300","13" } ,
     // { "1400","14" } ,
     // { "1600","16" } ,
     // { "1800","18" } 
     // } ;
     //msrdata.setTable(
     //  //2-dim. arrays can't be passed as variable length for each dimension.
     //  //->nach B. Stroustrup chapter C.7.3
     //  //&stdstrFIDtoMHzTable[0][0]
     //  stdstrFIDtoMHzTable[0],2,4);
     //std::string stdstrVIDtoVoltTable [][2] = {
     // { "Volt","VID" } ,
     // { "0.860","10" } ,
     // { "0.924","14" } ,
     // { "0.988","18" } ,
     // { "1.036","21" } ,
     // { "1.068","23" } ,
     // { "1.1","25" } ,
     // { "1.132","27" } ,
     // { "1.164","29" } ,
     // { "1.196","31" } ,
     // { "1.244","33" } ,
     // { "1.276","36" } ,
     // { "1.308","38" } ,
     // { "1.340","40" } 
     // } ;
     //msrdata.setTable(
     //  //2-dim. arrays can't be passed as variable length for each dimension.
     //  //->nach B. Stroustrup chapter C.7.3
     //  //&stdstrFIDtoMHzTable[0][0]
     //  stdstrVIDtoVoltTable[0],2,14);
     //mp_model->m_stdvectorUIitem.push_back(msrdata ) ;
     wxDynamicDialog * p_wxdlg = new wxDynamicDialog(
       this , 
       //msrdata 
       //*mp_pumastatectrl->mp_model,
       *mp_cpucontroller->mp_model,
       //mp_pumastatectrl
       mp_cpucontroller
       );
     //p_wxdlg->ShowModal() ;
     p_wxdlg->Show(true) ;
  }
#endif

void MainFrame::OnDynamicallyCreatedUIcontrol(wxCommandEvent & wxevent)
{
  int nMenuEventID = wxevent.GetId() ;
//#ifdef COMPILE_WITH_VISTA_POWERPROFILE_ACCESS
#ifdef COMPILE_WITH_OTHER_DVFS_ACCESS
  if( nMenuEventID == ID_MinAndMaxCPUcoreFreqInPercentOfMaxFreq )
  {
    //wxMessageDialog();
    //wxMessageBox("hh") ;
    //wxString strCurrentValue = wxT("50") ;
    //wxString sNewValue = wxGetTextFromUser(wxT("Enter min CPU %for CURRENT power profile (scheme)"), 
//#ifndef _DEBUG
    //  wxT("title"), strCurrentValue);
    if( //mp_pumastatectrl->m_bFrequencyScalingByOSDisabled 
      //mp_pumastatectrl->mp_dynfreqscalingaccess->OtherDVFSisEnabled() 
      ::wxGetApp().mp_dynfreqscalingaccess->OtherDVFSisEnabled() 
      )
//#endif
      //mp_pumastatectrl->DisableFrequencyScalingByOS() ;
      ::wxGetApp().mp_dynfreqscalingaccess->DisableFrequencyScalingByOS() ;
//#ifndef _DEBUG
    else
      //mp_pumastatectrl->EnableFrequencyScalingByOS() ;
      ::wxGetApp().mp_dynfreqscalingaccess->EnableFrequencyScalingByOS() ;
//#endif //#ifdef _DEBUG
    mp_wxmenuitemOtherDVFS->SetText(//_T("")
       wxString::Format(
       //We need a _T() macro (wide char-> L"", char->"") for EACH 
       //line to make it compitable between char and wide char.
       _T("%sable DVFS") , 
       //mp_pumastatectrl->m_bFrequencyScalingByOSDisabled 
       //mp_pumastatectrl->mp_dynfreqscalingaccess->OtherDVFSisEnabled() 
       ::wxGetApp().mp_dynfreqscalingaccess->OtherDVFSisEnabled() ? 
       //We need a _T() macro (wide char-> L"", char->"") for EACH 
       //line to make it compitable between char and wide char.
       _T("dis") : 
      //We need a _T() macro (wide char-> L"", char->"") for EACH 
      //line to make it compitable between char and wide char.
      _T("en")
        )
      ) ;
    return ;
  }
#endif //#ifdef COMPILE_WITH_OTHER_DVFS_ACCESS
//#endif
  BYTE byCoreID = ( nMenuEventID - m_nLowestIDForSetVIDnFIDnDID ) / 
    m_nNumberOfMenuIDsPerCPUcore ;
  //EventIDToDivisorIDandFrequencyID(nMenuEventID);
  if( //Array was successfully allocated.
    m_arp_freqandvoltagesettingdlg 
    //&&
    ////menu item "set frequency and voltage" for any CPU core menu was selected.
    //( nMenuEventID - m_nLowestIDForSetVIDnFIDnDID ) % 
    //m_nNumberOfMenuIDsPerCPUcore == 0 
    )
  {
    //FreqAndVoltageSettingDlg freqandvoltagesettingdlg(this);
    //freqandvoltagesettingdlg.Show(true);
    //if( mp_freqandvoltagesettingdlg )
    //{
    //  //if(mp_freqandvoltagesettingdlg )
    //    mp_freqandvoltagesettingdlg->Show(true);
    //}
    //else
    //{
    //  //If created as local vaiable on stack the dialog would disappear 
    //  //immediately.
    //  mp_freqandvoltagesettingdlg = new FreqAndVoltageSettingDlg(this);
    //  if(mp_freqandvoltagesettingdlg )
    //    mp_freqandvoltagesettingdlg->Show(true);
    //}
    BYTE bySubmenuIndexWithinPerCoreMenu = ( nMenuEventID - 
      m_nLowestIDForSetVIDnFIDnDID ) % m_nNumberOfMenuIDsPerCPUcore ;

    switch( bySubmenuIndexWithinPerCoreMenu )
    {
      case //0:
        Settings :
        if( m_arp_freqandvoltagesettingdlg[byCoreID] )
          m_arp_freqandvoltagesettingdlg[byCoreID]->Show(true);
        else
        {
          //If created as local vaiable on stack the dialog would disappear 
          //immediately.
          //TODO uncomment
          //m_arp_freqandvoltagesettingdlg[byCoreID] = new 
          //  FreqAndVoltageSettingDlg(this,mp_pumastatectrl,byCoreID);
          if( m_arp_freqandvoltagesettingdlg[byCoreID] )
            m_arp_freqandvoltagesettingdlg[byCoreID]->Show(true);
        }
      break;
//      case setp_state0:
//        PossiblyAskForOSdynFreqScalingDisabling();
//        mp_pumastatectrl->SetPstate(0,
//          //1 = 1bin
//          1 <<
//          //=core ID:
//            //difference between current menu ID first menu ID for
//            //core settings
//            ( nMenuEventID - m_nLowestIDForSetVIDnFIDnDID )
//            / m_nNumberOfMenuIDsPerCPUcore);
//        break;
//      case setp_state1:
//        PossiblyAskForOSdynFreqScalingDisabling();
//        mp_pumastatectrl->SetPstate(1,
//          //1 = 1bin
//          1 <<
//          //=core ID:
//            //difference between current menu ID first menu ID for
//            //core settings
//            ( nMenuEventID - m_nLowestIDForSetVIDnFIDnDID )
//            / m_nNumberOfMenuIDsPerCPUcore);
//        break;
//      case setp_state2:
//        PossiblyAskForOSdynFreqScalingDisabling();
//        mp_pumastatectrl->SetPstate(2,
//          //1 = 1bin
//          1 <<
//          //=core ID:
//            //difference between current menu ID first menu ID for
//            //core settings
//            ( nMenuEventID - m_nLowestIDForSetVIDnFIDnDID )
//            / m_nNumberOfMenuIDsPerCPUcore);
//        break;
      case highLoadThread:
        {
        //m_byNumberOfSettablePstatesPerCore + 1:
        BYTE byCoreID = 
          //=core ID:
            //difference between current menu ID first menu ID for
            //core settings
            ( nMenuEventID - m_nLowestIDForSetVIDnFIDnDID ) 
            / m_nNumberOfMenuIDsPerCPUcore ;
        //mp_pumastatectrl->mp_calculationthread->StartCalculationThread(
        //  //=core ID:
        //    //difference between current menu ID first menu ID for
        //    //core settings
        //    ( nMenuEventID - m_nLowestIDForSetVIDnFIDnDID ) 
        //    / m_nNumberOfMenuIDsPerCPUcore
        //  );
        //TODO uncomment
        //BYTE byAction = mp_pumastatectrl->StartOrStopCalculationThread(
        //    byCoreID
        //  );
        //BuildHighLoadThreadMenuText(
        //  " high load thread (e.g. for stability check)" , byAction ) ;
        }
        break;
      default:
        m_stdmapwmenuid2i_cpucontrolleraction.find(nMenuEventID)->
           second->Execute() ;
        //if( bySubmenuIndexWithinPerCoreMenu == m_byNumberOfSettablePstatesPerCore + 1)
        //{
        //BYTE byCoreID = 
        //  //=core ID:
        //    //difference between current menu ID first menu ID for
        //    //core settings
        //    ( nMenuEventID - m_nLowestIDForSetVIDnFIDnDID ) 
        //    / m_nNumberOfMenuIDsPerCPUcore ;
        ////mp_pumastatectrl->mp_calculationthread->StartCalculationThread(
        ////  //=core ID:
        ////    //difference between current menu ID first menu ID for
        ////    //core settings
        ////    ( nMenuEventID - m_nLowestIDForSetVIDnFIDnDID ) 
        ////    / m_nNumberOfMenuIDsPerCPUcore
        ////  );
        //BYTE byAction = mp_pumastatectrl->StartOrStopCalculationThread(
        //    byCoreID
        //  );
        //wxString wxstr = byAction == ENDED ? "Start" : "End" ;
        //////Invert the menu item's checked state.
        ////marp_wxmenuItemHighLoadThread[byCoreID]->Check(
        ////    ! marp_wxmenuItemHighLoadThread[byCoreID]->IsChecked () ) ;
        //marp_wxmenuItemHighLoadThread[byCoreID]->SetText( 
        //    wxstr + 
        //    " high load thread (e.g. for stability check)"
        //    );
        //}
        if( bySubmenuIndexWithinPerCoreMenu //> 0 
           >= m_byMenuIndexOf1stPstate &&
           bySubmenuIndexWithinPerCoreMenu < //bySubmenuIndexWithinPerCoreMenu 
           m_byMenuIndexOf1stPstate + m_byNumberOfSettablePstatesPerCore )
        {
          BYTE byCoreID =
          //=core ID:
              //difference between current menu ID first menu ID for
              //core settings
              ( nMenuEventID - m_nLowestIDForSetVIDnFIDnDID )
              / m_nNumberOfMenuIDsPerCPUcore ;
          BYTE byCoreBitMask = 
            //1 = 1bin
            1 << byCoreID ;
          PossiblyAskForOSdynFreqScalingDisabling();
          //TODO uncomment
          //mp_pumastatectrl->SetPstate(
          //  bySubmenuIndexWithinPerCoreMenu - m_byMenuIndexOf1stPstate ,
          //  //byCoreBitMask
          //  byCoreID
          //  );
        }
        break;
    }
  }
}

//#ifdef _WINDOWS //For installing as a service.
#ifdef COMPILE_WITH_SERVICE_CAPABILITY
  void MainFrame::OnStartService(wxCommandEvent& WXUNUSED(event))
  {
    SERVICE_TABLE_ENTRY   DispatchTable[] = 
    { 
      { 
        //"Pointer to a null-terminated string that specifies the name of a service to be run in this service process. 
        //If the service is installed with the SERVICE_WIN32_OWN_PROCESS service type, this member is ignored, but cannot be NULL. This member can be an empty string ("").
        //If the service is installed with the SERVICE_WIN32_SHARE_PROCESS service type, this member specifies the name of the service that uses the ServiceMain function pointed to by the lpServiceProc member."
        "MyService", 
        //Pointer to a ServiceMain function. 
        MyServiceStart      }, 
      //NULL values mark the end.
      { NULL,              NULL          } 
    }; 

    if (!::StartServiceCtrlDispatcher( DispatchTable))
      //If the function fails, the return value is zero. 
    {
      DWORD dwErrorCode = ::GetLastError() ;
      DEBUG(" [MY_SERVICE] StartServiceCtrlDispatcher error number: %lu\n", 
        dwErrorCode );
      switch(dwErrorCode)
      {
      case ERROR_FAILED_SERVICE_CONTROLLER_CONNECT:
        DEBUG("Typically, this error indicates that the program is being run as a console application rather than as a service. "
"If the program will be run as a console application for debugging purposes, structure it such that service-specific code is not called when this error is returned.\n");
        break;
      case ERROR_INVALID_DATA:
        DEBUG("The specified dispatch table contains entries that are not in the proper format.\n");
        break;
      case ERROR_SERVICE_ALREADY_RUNNING:
        DEBUG("The process has already called \n");
        break;
      default:
        DEBUG("error code was not set by the service control manager directly\n");
      }
      ::wxMessageBox("error starting service");
    }
    else
      ::wxMessageBox("service succ. started ");
  }
#endif //#ifdef COMPILE_WITH_SERVICE_CAPABILITY

void MainFrame::OnFindLowestOperatingVoltage(wxCommandEvent& WXUNUSED(event))
{
  //TODO uncomment
  //mp_pumastatectrl->FindLowestOperatingVoltage(
  //  mp_pumastatectrl->GetCurrentPstate(1),0);
}

#ifdef PRIVATE_RELEASE //hide the other possibilities
void MainFrame::OnIncreaseVoltageForCurrentPstate(wxCommandEvent& WXUNUSED(event))
{
  mp_pumastatectrl->IncreaseVoltageForCurrentPstate();
}
#endif //#ifdef PRIVATE_RELEASE //hide the other possibilities

#ifdef wxHAS_POWER_EVENTS
  void MainFrame::OnResume(wxPowerEvent& WXUNUSED(event))
  {
#ifndef _EMULATE_TURION_X2_ULTRA_ZM82
    //mp_pumastatectrl->ApplyAllPStates() ;
    mp_cpucontroller->ResumeFromS3orS4() ;
    //After a resume (from standby) the counter value for CPU core 1 did not change.
    //So re-initialize it now.
    //m_clocksnothaltedcpucoreusagegetter.Init() ;
    ::wxGetApp().mp_cpucoreusagegetter->Init() ;
#endif //#ifndef _EMULATE_TURION_X2_ULTRA_ZM82
    //wxLogMessage(_T("System resumed from suspend."));
  }
#endif // wxHAS_POWER_EVENTS

void MainFrame::OnSaveAsDefaultPStates(wxCommandEvent & WXUNUSED(event))
{
  std::string strCPUtypeRelativeDirPath ;
  std::string strPstateSettingsFileName ;
  if( wxGetApp().m_maincontroller.GetPstatesDirPath( 
    strCPUtypeRelativeDirPath ) 
    && wxGetApp().m_maincontroller.GetPstateSettingsFileName( 
    strPstateSettingsFileName )
    )
  {
    LOGN( "for file dialog: relative dir path=" << strCPUtypeRelativeDirPath )
    wxString wxstrFilePath = ::wxFileSelector( 
      wxT("Select file path") 
      , strCPUtypeRelativeDirPath.c_str() 
      , strPstateSettingsFileName.c_str() 
      , wxT("xml")
      , wxT("*.*")
      , wxFD_SAVE
      ) ;
    if ( ! wxstrFilePath.empty() )
    {
        // work with the file
        //...
      //readXMLfileDOM( wxstrFilePath.c_str() ) ;
      mergeXMLfileDOM( wxstrFilePath.c_str() , * mp_model , 
        strPstateSettingsFileName ) ;
    }
    //mp_configfileaccess->
  }
}

void MainFrame::OnSize( wxSizeEvent & //WXUNUSED(
                     sizeevent//)
                     )
{
  RedrawEverything() ;
}

void MainFrame::OnTimerEvent(wxTimerEvent &event)
{
  //Do something
  bool bNewVoltageAndFreqPair = false ;
  float fVoltageInVolt ;
  WORD wFreqInMHz ;
  std::pair <std::set<VoltageAndFreq>::iterator, bool> stdpairstdsetvoltageandfreq ;
  for ( BYTE byCPUcoreID = 0 ; byCPUcoreID < 
    mp_cpucoredata->m_byNumberOfCPUCores ; ++ byCPUcoreID )
  {
    if( mp_cpucontroller->GetCurrentPstate(wFreqInMHz, fVoltageInVolt, byCPUcoreID ) )
    {
#ifdef _DEBUG
      if( wFreqInMHz > 1800 )
        wFreqInMHz = wFreqInMHz ;
#endif
      //stdpairstdsetvoltageandfreq = mp_model->m_cpucoredata.
      //  m_stdsetvoltageandfreqDefault.insert( 
      //  VoltageAndFreq ( fVoltageInVolt , wFreqInMHz ) 
      //  ) ;
      bNewVoltageAndFreqPair = mp_model->m_cpucoredata.AddDefaultVoltageForFreq(
        fVoltageInVolt , wFreqInMHz ) ;
      ////New p-state inserted.
      //if( stdpairstdsetvoltageandfreq.second )
      //  bNewVoltageAndFreqPair = true ;
    }
  }
  if( bNewVoltageAndFreqPair )
  {
    std::set<VoltageAndFreq>::reverse_iterator reviter = 
      mp_model->m_cpucoredata.m_stdsetvoltageandfreqDefault.rbegin() ;
    //Need to set the max freq. Else (all) the operating points are drawn at x-coord. "0".
    mp_cpucoredata->m_wMaxFreqInMHz = (*reviter).m_wFreqInMHz ;
    RedrawEverything() ;
  }
  else
    Refresh() ;
  //TRACE("OnTimerEvent\n") ;
}

void MainFrame::OnUpdateViewInterval(wxCommandEvent & WXUNUSED(event))
{
  wxString wxstrMessage = wxT("Input update view interval in milliseconds") ;
  wxString wxstrInput = ::wxGetTextFromUser( wxstrMessage , wxT("input"), 
    wxT("1000") ) ;
  //::wxGetNumberFromUser
  //If the user has input text (and has NOT pressed "cancel")
  if( ! wxstrInput.empty() )
  {
    unsigned long ulMs ;
    bool bSuccess = wxstrInput.ToULong(& ulMs) ;
    if ( bSuccess )
    {
      //ReadMsrEx() returned false results if used with a time and a 10 ms interval.
      if( ulMs < 100 )
        wxMessageBox( wxT("the number is too low. "
        "Getting the current CPU frequency returned wrong values with < 100 ms") ) ;
      else
      {
        m_dwTimerIntervalInMilliseconds = ulMs ;
        m_timer.Stop() ;
        m_timer.Start(ulMs) ;
      }
    }
    else
    {
      wxMessageBox( wxT("You did not enter a valid integer number") ) ;
    }
  }
}

//wxString MainFrame::GetSetPstateMenuItemLabel(
//  BYTE byPstateNumber
    //The "PState" class is AMD-specific.
//  , PState & pstate
//  )
//{
//   return wxString::Format(
//     //We need a _T() macro (wide char-> L"", char->"") for EACH
//     //line to make it compitable between char and wide char.
//     _T("set p-state %u (%u MHz @ %f Volt)"),
//       (WORD) byPstateNumber
//       , pstate.GetFreqInMHz()
//       , pstate.GetVoltageInVolt()
//     ) ;
//}

//void MainFrame::SetMenuItemLabel(
//  BYTE byCoreID
//  , BYTE byPstateNumber
//  , //const
//    PState & pstate
//  )
//{
//    //int nMenuItemID
//    //this->m_nLowestIDForSetVIDnFIDnDID
//    //this->m_nNumberOfMenuIDsPerCPUcore
//    //wxMenuItem * p_wxmenuitem = FindItemByPosition(size_t position) ;
//    m_arp_wxmenuitemPstate[byCoreID * NUMBER_OF_PSTATES + byPstateNumber]
//      ->SetText(//_T("")
//       //wxString::Format("set p-state %u (%u MHz @ %f Volt)",
//       //    (WORD) byPstateNumber
//       //    , pstate.GetFreqInMHz()
//       //    , pstate.GetVoltageInVolt()
//       // )
//       GetSetPstateMenuItemLabel(byPstateNumber,pstate)
//      ) ;
//}

void MainFrame::RedrawEverything()
{
  int i = 0 ;
  if( mp_wxbitmap )
    delete mp_wxbitmap ;
  wxRect rect = GetClientRect();
  m_fMinVoltage = mp_cpucontroller->GetMinimumVoltageInVolt() ;
  m_fMaxMinusMinVoltage = m_fMaxVoltage - m_fMinVoltage ;

  //wxCoord wxcoordCanvasWidth ;
  //wxCoord wxcoordCanvasHeight ;
  //p_wxpaintdc->GetSize( & wxcoordCanvasWidth , & wxcoordCanvasHeight ) ;
  m_wDiagramHeight = rect.height 
      //Space for drawing scale and scale values below the x-axis.
      - 50 ;
  //WORD wXcoordOfBeginOfYaxis = 50 ;
  m_wXcoordOfBeginOfYaxis = 50 ;
  //WORD wDiagramWidth = wxcoordCanvasWidth - wXcoordOfBeginOfYaxis - 30 ;
  m_wDiagramWidth = rect.width - m_wXcoordOfBeginOfYaxis - 30 ;

  mp_wxbitmap = new wxBitmap( 
    rect.width, 
    rect.height , 
    //http://docs.wxwidgets.org/stable/wx_wxbitmap.html#wxbitmapctor:
    //"A depth of -1 indicates the depth of the current screen or visual."
    -1) ;

  //if( mp_wxbufferedpaintdcStatic )
  //  delete mp_wxbufferedpaintdcStatic ;
  //create new objet so it uses internally an bitmap with the new client size.
  //mp_wxbufferedpaintdcStatic = new wxBufferedPaintDC(this) ;
  if( mp_wxbitmapStatic )
    delete mp_wxbitmapStatic ;
  mp_wxbitmapStatic = new wxBitmap( 
    rect.width, 
    rect.height , 
    //http://docs.wxwidgets.org/stable/wx_wxbitmap.html#wxbitmapctor:
    //"A depth of -1 indicates the depth of the current screen or visual."
    -1) ;
  m_wxmemorydcStatic.SelectObject(*mp_wxbitmapStatic) ;
  //Clears the device context using the current background brush. 
  //(else black background?)
  m_wxmemorydcStatic.Clear();
  //m_wxbufferedpaintdcStatic.SelectObject(mp_wxbitmapStatic) ;
//  if( mp_wxbufferedpaintdc
//  wxBufferedPaintDC mp_wxbufferedpaintdc ( this ) ;
  //Drawing the curves (calculate and draw ~ 400 points) takes some time. So do it only when
  //the client size changes and store the drawed curves into a image and DrawBitmap() or do 
  //"Blit()" with the DC drawn to and the DC that shows it in the window.
  //if( mp_wxbufferedpaintdcStatic )
  {
    std::set<VoltageAndFreq> & r_setvoltageforfreq = mp_cpucontroller->
      mp_model->m_cpucoredata.m_stdsetvoltageandfreqDefault ;
//    std::set<MaxVoltageForFreq>::iterator iterstdsetmaxvoltageforfreq = 
    std::set<VoltageAndFreq>::reverse_iterator iterstdsetvoltageandfreq = 
      //mp_pumastatectrl->mp_model->//m_vecmaxvoltageforfreq
      //m_setmaxvoltageforfreq.begin() ;
      r_setvoltageforfreq.rbegin() ;
    if( //iterstdvecmaxvoltageforfreq != mp_pumastatectrl->mp_model->
      iterstdsetvoltageandfreq != //mp_pumastatectrl->mp_model->
      //mp_cpucontroller->mp_model->
      //m_vecmaxvoltageforfreq.end() 
      //m_setmaxvoltageforfreq.end()
      r_setvoltageforfreq.rend()
      )
    {
      m_fMaxVoltage = //(*iterstdvecmaxvoltageforfreq).m_fVoltageInVolt ;
        //P-state 0 usually has the highest voltage.
        (*iterstdsetvoltageandfreq).m_fVoltageInVolt ;
      //for g++: assign value to created iterator and pass this to the arg
      std::set<VoltageAndFreq>::iterator iter = r_setvoltageforfreq.begin() ;
      DrawDiagramScale( m_wxmemorydcStatic , //iterstdsetvoltageandfreq
        //r_setvoltageforfreq.begin()
        iter
        ) ;
    }
    DrawOvervoltageProtectionCurve(
      //m_wxbufferedpaintdcStatic
      //*mp_wxbufferedpaintdcStatic
      m_wxmemorydcStatic
      , m_fMaxVoltage
      ) ;
    DrawCurrentVoltageSettingsCurve(
      //m_wxbufferedpaintdcStatic
      //*mp_wxbufferedpaintdcStatic
      m_wxmemorydcStatic
      , m_fMaxVoltage
      ) ;
    DrawLowestStableVoltageCurve(
      //m_wxbufferedpaintdcStatic
      //*mp_wxbufferedpaintdcStatic
      m_wxmemorydcStatic
      , m_fMaxVoltage
      ) ;
    DrawAllPossibleOperatingPoints( m_wxmemorydcStatic ) ;
    DrawPerformanceStatesCrosses( m_wxmemorydcStatic , 
      mp_cpucoredata->m_stdsetvoltageandfreqDefault 
      , wxGREEN ) ;
  }
}

void MainFrame::SetCPUcontroller(I_CPUcontroller * p_cpucontroller )
{
  mp_cpucontroller = p_cpucontroller ;
}

//void MainFrame::SetPumaStateController(GriffinController * p_pumastatectrl)
//{
//  mp_pumastatectrl = p_pumastatectrl ;
//  //DEBUG("main frame->SetPumaStateController--Address of AMD family 17 controller:%lx\n",
//  //  mp_pumastatectrl);
//}

void MainFrame::UpdatePowerSettings(wxPowerType powerType, wxBatteryState batteryState)
{
    wxString powerStr;
    switch ( m_powerType = powerType )
    {
        case wxPOWER_SOCKET:
            powerStr = _T("wall");
            break;

        case wxPOWER_BATTERY:
            powerStr = _T("battery");
            break;

        default:
            wxFAIL_MSG(_T("unknown wxPowerType value"));
            // fall through

        case wxPOWER_UNKNOWN:
            powerStr = _T("psychic");
            break;
    }

    wxString batteryStr;
    switch ( m_batteryState = batteryState )
    {
        case wxBATTERY_NORMAL_STATE:
            batteryStr = _T("charged");
            break;

        case wxBATTERY_LOW_STATE:
            batteryStr = _T("low");
            break;

        case wxBATTERY_CRITICAL_STATE:
            batteryStr = _T("critical");
            break;

        case wxBATTERY_SHUTDOWN_STATE:
            batteryStr = _T("empty");
            break;

        default:
            wxFAIL_MSG(_T("unknown wxBatteryState value"));
            // fall through

        case wxBATTERY_UNKNOWN_STATE:
            batteryStr = _T("unknown");
            break;
    }

    SetStatusText( wxString::Format (
      _T("System is on %s power, battery state is %s"),
      powerStr.c_str(),
      batteryStr.c_str()
        )
      );
}
