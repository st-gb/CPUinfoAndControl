/*
 * App_FindLowestStableCPUcoreVoltage.cpp
 *
 *  Created on: 03.10.2011
 *      Author: Stefan
 */

#include "wx/wx.h" //for wxMessageBox(...) (,etc.)

#include "App.hpp" //START_INSTABLE_CPU_CORE_VOLTAGE_DETECTION_FCT_NAME

//GetArrayIndexForClosestGreaterOrEqual(...)
//#include <algorithms/binary_search/binary_search.h>
#include <algorithms/binary_search/binary_search.cpp>
//::GetErrorMessageFromLastErrorCodeA();
#include <Controller/GetErrorMessageFromLastErrorCode.hpp>
//unsigned long int SetThreadAffinityMask(DWORD dwThreadAffinityMask)
#include <Controller/SetThreadAffinityMask.h>
//THREAD_PROC_CALLING_CONVENTION
#include <preprocessor_macros/thread_proc_calling_convention.h>
//class FreqAndVoltageSettingDlg
#include <wxWidgets/UserInterface/FreqAndVoltageSettingDlg.hpp>
//::getwxString(...)
#include <wxWidgets/Controller/character_string/wxStringHelper.hpp>
#include <wxWidgets/UserInterface/MainFrame.hpp> //class MainFrame

void SetThreadAffinityMask()
{
#ifdef _WIN32
  DWORD dwProcessAffinityMask;
  DWORD dwSystemAffinityMask;
  DWORD dwThreadAffinityMask = 1;
  //http://msdn.microsoft.com/en-us/library/windows/desktop/ms683213%28v=vs.85%29.aspx:
  //"A process affinity mask is a bit vector in which each bit represents the
  //processors that a process is allowed to run on. A system affinity mask is
  //a bit vector in which each bit represents the processors that are
  //configured into a system."
  ::GetProcessAffinityMask(
    ::GetCurrentThread(),
    & dwProcessAffinityMask //_out  PDWORD_PTR lpProcessAffinityMask
    , & dwSystemAffinityMask // __out  PDWORD_PTR lpSystemAffinityMask
    );
#endif
  for( BYTE byCPUcoreIndex = 0; byCPUcoreIndex < sizeof(DWORD_PTR) * 8 ;
      ++ byCPUcoreIndex)
  {
    //This process may run on core "byCPUcoreIndex".
    if( ( (dwProcessAffinityMask >> byCPUcoreIndex) & 1) == 1)
    {
      dwThreadAffinityMask = (1 << byCPUcoreIndex);
      break;
    }
  }
  //Must "pin" this thread to a specific CPU core. Else it usally is being
  //executed on different CPU cores and the CPU load does not reach 100%.
  //TODO: if this _process_ does not have affinity to core 0 (e.g. when changed
  //in task manager), the call fails?!
  ::SetThreadAffinityMask(//1
      dwThreadAffinityMask);
}

DWORD THREAD_PROC_CALLING_CONVENTION
  StartInstableCPUcoreVoltageDetectionInDLL(void * p_v )
{
  FreqAndVoltageSettingDlg * p_freqandvoltagesettingdlg =
    (FreqAndVoltageSettingDlg *) p_v;
  if( p_freqandvoltagesettingdlg)
  {
    LOGN( FULL_FUNC_NAME << "--before setting thread affinity mask"
        //" to CPU core 0"
        )
    ::SetThreadAffinityMask();
    //GetThreadAffinityMask();
    LOGN( FULL_FUNC_NAME << "--after setting thread affinity mask"
        //" to CPU core 0"
        )

    uint32_t ui32CPUcoreMask = p_freqandvoltagesettingdlg->GetCPUcoreMask();

    LOGN( FULL_FUNC_NAME << "--before calling the DynLib's \""
      << START_INSTABLE_CPU_CORE_VOLTAGE_DETECTION_FCT_NAME "\" function")
    (* wxGetApp().m_pfnStartInstableCPUcoreVoltageDetection)(
      //p_freqandvoltagesettingdlg->mp_model->m_cpucoredata.m_byNumberOfCPUCores,
      ui32CPUcoreMask,
      & wxGetApp().m_external_caller);
    LOGN( FULL_FUNC_NAME << "--after calling the DynLib's \""
      << START_INSTABLE_CPU_CORE_VOLTAGE_DETECTION_FCT_NAME << "\" function")
    return 0;
  }
  return 1;
}

DWORD THREAD_PROC_CALLING_CONVENTION FindLowestStableVoltage(void * p_v )
{
  LOGN( FULL_FUNC_NAME << "--begin")

  FreqAndVoltageSettingDlg * p_freqandvoltagesettingdlg =
    (FreqAndVoltageSettingDlg *) p_v;
  if( p_freqandvoltagesettingdlg)
  {
    x86IandC::thread_type x86iandc_threadFindLowestStableVoltage;

    x86iandc_threadFindLowestStableVoltage.start(
        StartInstableCPUcoreVoltageDetectionInDLL ,
        p_freqandvoltagesettingdlg ) ;

    float fVoltageInVolt = p_freqandvoltagesettingdlg->
      GetVoltageInVoltFromSliderValue() ;
    p_freqandvoltagesettingdlg->DisableOSesDVFSandServiceDVFS();

    const std::set<float> & c_r_stdset_floatAvailableVoltagesInVolt =
      p_freqandvoltagesettingdlg->mp_model->m_cpucoredata.
      m_stdset_floatAvailableVoltagesInVolt;
//    std::set<float>::
//      //Voltages are sorted ascending, but we want to get decrease the voltage:
//      //so use a "const_reverse_iterator"
//      const_reverse_iterator c_rev_iter =
//      c_r_stdset_floatAvailableVoltagesInVolt.find(fVoltageInVolt);

    WORD wArrayIndex = ::GetArrayIndexForClosestGreaterOrEqual(
      p_freqandvoltagesettingdlg->mp_model->m_cpucoredata.
        m_arfAvailableVoltagesInVolt,
      c_r_stdset_floatAvailableVoltagesInVolt.size(),
      fVoltageInVolt);
//    while( c_rev_iter != c_r_stdset_floatAvailableVoltagesInVolt.rend() )
//    {
//      p_freqandvoltagesettingdlg->PossiblyWriteVoltageAndMultiplier_Inline(
////        fVoltageInVolt
//        * c_rev_iter
//        ) ;
//    }
//    p_freqandvoltagesettingdlg->m_p_wxbuttonFindLowestStableVoltage->SetLabel(
//      wxT("stop finding the lowest stable voltage") );

//    p_freqandvoltagesettingdlg->
//      ChangeToStopFindingLowestStableCPUcoreVoltageButton();

    DWORD dwMilliSecondsToWait = 60000;

//    float fVoltageInVolt;
//    //multipliers can also be floats: e.g. 5.5 for AMD Griffin.
//    float fMultiplier;
//    float fReferenceClockInMHz;
//    WORD wCoreID;
//    p_freqandvoltagesettingdlg->mp_cpucontroller->GetCurrentVoltageAndFrequency(
//      fVoltageInVolt,
//      fMultiplier, fReferenceClockInMHz, 0);
    float fMultiplierFromSliderValue = p_freqandvoltagesettingdlg->
      GetMultiplierFromSliderValue();

    for( //-- wArrayIndex
      ; wArrayIndex != MAXWORD &&
      ! wxGetApp().m_vbExitFindLowestStableVoltage; -- wArrayIndex )
    {
        fVoltageInVolt = p_freqandvoltagesettingdlg->mp_model->m_cpucoredata.
          m_arfAvailableVoltagesInVolt[wArrayIndex];

        LOGN( FULL_FUNC_NAME << "--before setting voltage[Volt] to: "
            << fVoltageInVolt << " , multi to " << fMultiplierFromSliderValue)
        p_freqandvoltagesettingdlg->mp_cpucontroller->
          SetCurrentVoltageAndMultiplier(
            fVoltageInVolt, //fMultiplier
            fMultiplierFromSliderValue, 0);
        //::Sleep(60000);

//        //http://msdn.microsoft.com/en-us/library/ms687025%28v=VS.85%29.aspx:
//        ::WaitForMultipleObjects(
//          2 //__in  DWORD nCount
//          , //__in  const HANDLE *lpHandles,
//          ,BOOL
//          //"The time-out interval, in milliseconds."
//          , 60000 //__in  DWORD dwMilliseconds
//          );
        dwMilliSecondsToWait = 1000;
        for( unsigned uiSeconds = p_freqandvoltagesettingdlg->mp_model->
            m_instablecpucorevoltagedetection.
            m_uiNumberOfSecondsToWaitUntilVoltageIsReduced + 1;
            uiSeconds != 0 &&
          ! wxGetApp().m_vbExitFindLowestStableVoltage; -- uiSeconds)
        {
          p_freqandvoltagesettingdlg->
            m_p_wxstatictextSecondsUntilNextVoltageDecrease->SetLabel(
//            m_p_wxtextctrlSecondsUntilNextVoltageDecrease->SetLabel(
              wxString::Format( wxT("%us"), //(WORD)
                uiSeconds - 1)
              );
          LOGN( FULL_FUNC_NAME << "--waiting max. " << dwMilliSecondsToWait
            << " milliseconds")
          ::WaitForSingleObject( wxGetApp().m_conditionFindLowestStableVoltage.
            m_handleEvent, dwMilliSecondsToWait);
          LOGN( FULL_FUNC_NAME << "--after waiting for either thread's end or "
              "timeout ")
          wxGetApp().m_conditionFindLowestStableVoltage.ResetEvent();
        }

//        // "Each wxCondition object is associated with a (single) wxMutex object.
//        // The mutex object MUST be locked before calling Wait()"
//        wxGetApp().m_wxmutexFindLowestStableVoltage.Lock();
//        wxGetApp().m_wxconditionFindLowestStableVoltage.
//          //"exactly as Wait() except that it may also return if the specified
//          // timeout elapses even if the condition hasn't been signalled: in this
//          // case, the return value is false, otherwise (i.e. in case of a normal
//          // return) it is true
//          WaitTimeout(
//          // "the timeout parameter specifies an interval that needs to be waited for
//          // in milliseconds"
//          60000);
    }
    p_freqandvoltagesettingdlg->SetStartFindingLowestStableVoltageButton();
//    p_freqandvoltagesettingdlg->m_p_wxbuttonFindLowestStableVoltage->SetLabel(
//      wxT("find lowest stable voltage") );

//    wxBitmap wxbitmapFindLowestStableCPUcoreVoltage = wxBitmap(
//      find_lowest_stable_CPU_core_voltage16x16_xpm ) ;
//    p_freqandvoltagesettingdlg->
//      m_p_wxbitmapbuttonFindLowestStableCPUcoreVoltage->SetImageLabel(
//        wxbitmapFindLowestStableCPUcoreVoltage);
//    p_freqandvoltagesettingdlg->
//      m_p_wxbitmapbuttonFindLowestStableCPUcoreVoltage->SetToolTip(
//      wxT("start finding the lowest stable CPU core voltage") );

    LOGN( FULL_FUNC_NAME << "--return 0")
    return 0;
  }
  LOGN( FULL_FUNC_NAME << "--return 1")
  return 1;
}

BYTE wxX86InfoAndControlApp::InitUnstableVoltageDetectionDynLibAccess()
{
  LOGN( FULL_FUNC_NAME << " --begin")
//  LPTSTR lptstrUnstableVoltageDetectionDynLib = _T(//"Prime95.DLL"
//    //"UnstableVoltageDetection.dll"
//      "InstableCPUcoreVoltageDetection.dll"
//      );
  if(m_hmoduleUnstableVoltageDetectionDynLib == NULL)
    m_hmoduleUnstableVoltageDetectionDynLib = //::LoadLibrary(
      //lptstrUnstableVoltageDetectionDynLib
    ::LoadLibraryW( //m_std_wstrInstableCPUcoreVoltageDynLibPath.c_str()
      m_model.m_instablecpucorevoltagedetection.m_std_wstrDynLibPath.c_str() );
  std::string std_strUnstableVoltageDetectionDynLib = GetStdString_Inline(
//    GetStdTstring_Inline(lptstrUnstableVoltageDetectionDynLib)
//    m_std_wstrInstableCPUcoreVoltageDynLibPath
    m_model.m_instablecpucorevoltagedetection.m_std_wstrDynLibPath
    );
  if( m_hmoduleUnstableVoltageDetectionDynLib != NULL)
  {
    LOGN(FULL_FUNC_NAME << "--Successfully loaded \"" <<
      std_strUnstableVoltageDetectionDynLib << "\"")

    m_pfnStartInstableCPUcoreVoltageDetection =
      (StartInstableVoltageDetectionFunctionPointer)
      ::GetProcAddress(
        m_hmoduleUnstableVoltageDetectionDynLib, //_T(
        //"TortureTest"
        START_INSTABLE_CPU_CORE_VOLTAGE_DETECTION_FCT_NAME
        //)
        );
    if( ! m_pfnStartInstableCPUcoreVoltageDetection)
    {
      std::wstring std_wstrMessage = L"assigning function \"";
      std_wstrMessage +=
          _T(START_INSTABLE_CPU_CORE_VOLTAGE_DETECTION_FCT_NAME);
      std_wstrMessage += L"\" failed.";
      MessageWithTimeStamp( std_wstrMessage);
    }

    LOGN( FULL_FUNC_NAME <<  "--\"" <<
      START_INSTABLE_CPU_CORE_VOLTAGE_DETECTION_FCT_NAME
      << "\" function pointer: "
      << (void *) m_pfnStartInstableCPUcoreVoltageDetection)
    m_pfnStopInstableCPUcoreVoltageDetection =
      (StopInstableVoltageDetectionFunctionPointer) ::GetProcAddress(
        m_hmoduleUnstableVoltageDetectionDynLib, //_T(
//        "StopTortureTest"//)
        //"StopInstableVoltageDetection"
        STOP_INSTABLE_CPU_CORE_VOLTAGE_DETECTION_FCT_NAME
        );
    if( ! m_pfnStopInstableCPUcoreVoltageDetection)
    {
      std::wstring std_wstrMessage = L"assigning function \"";
      std_wstrMessage +=
          _T(STOP_INSTABLE_CPU_CORE_VOLTAGE_DETECTION_FCT_NAME);
      std_wstrMessage += L"\" failed.";
      MessageWithTimeStamp( std_wstrMessage);
    }
    LOGN(FULL_FUNC_NAME << "--\"" <<
      STOP_INSTABLE_CPU_CORE_VOLTAGE_DETECTION_FCT_NAME
      << "\" function pointer: "
      << (void *) m_pfnStopInstableCPUcoreVoltageDetection )
    if( m_pfnStartInstableCPUcoreVoltageDetection &&
        m_pfnStopInstableCPUcoreVoltageDetection
      )
    {
      wxString wxstrDynLibFilePath = ::getwxString(
//        m_std_wstrInstableCPUcoreVoltageDynLibPath
        m_model.m_instablecpucorevoltagedetection.m_std_wstrDynLibPath );
      mp_frame->m_p_wxmenuitemUnloadDetectInstableCPUcoreVoltageDynLib->
        SetHelp(wxstrDynLibFilePath);
      return 0;
    }
  }
  else
  {
    LOGN(FULL_FUNC_NAME << "--Failed to load \"" //"Prime95 DLL""
      << std_strUnstableVoltageDetectionDynLib
      << "\"" )
    std::wstring std_wstrMessage = L"Loading unstable voltage detection "
      "dynamic library \"" ;
    std_wstrMessage += //lptstrUnstableVoltageDetectionDynLib;
//      m_std_wstrInstableCPUcoreVoltageDynLibPath;
      m_model.m_instablecpucorevoltagedetection.m_std_wstrDynLibPath;
    std_wstrMessage += L"\" failed: ";
    std::string std_strErrorMessageFromLastErrorCode =
      ::GetErrorMessageFromLastErrorCodeA();
    std_wstrMessage += GetStdWstring(
      std_strErrorMessageFromLastErrorCode);
    MessageWithTimeStamp( std_wstrMessage);
  }
  return 1;
}

void wxX86InfoAndControlApp::StartInstableCPUcoreVoltageDetection(
    const FreqAndVoltageSettingDlg * c_p_freqandvoltagesettingdlg)
{
  //wxGetApp().
    InitUnstableVoltageDetectionDynLibAccess();

  if( //wxGetApp().
      m_hmoduleUnstableVoltageDetectionDynLib )
  {
    if( //wxGetApp().
        m_pfnStartInstableCPUcoreVoltageDetection && //wxGetApp().
        m_pfnStopInstableCPUcoreVoltageDetection)
    {
      DWORD dwExitCode;
      ::GetExitCodeThread( //wxGetApp().
        m_x86iandc_threadFindLowestStableVoltage.
        m_handleThread, & dwExitCode ) ;
       if( //m_p_wxbuttonFindLowestStableVoltage->GetLabel() ==
         //wxT("stop finding the lowest stable voltage")
      //        ::WaitForSingleObject( wxGetApp().
      //          m_x86iandc_threadFindLowestStableVoltage.m_handleThread,
      //          //wxGetApp().m_x86iandc_threadFindLowestStableVoltage
      //          0)
      //          ==
           dwExitCode == STILL_ACTIVE
         )
         ::wxMessageBox( wxT("Finding the lowest stable voltage has already "
           "been started.") );
       else
       {
        LOGN( FULL_FUNC_NAME << "--should start the find lowest stable "
            "voltage thread")
      //wxGetApp().
        m_vbExitFindLowestStableVoltage = false;

      //wxGetApp().
        m_x86iandc_threadFindLowestStableVoltage.start(
          FindLowestStableVoltage , (void *) c_p_freqandvoltagesettingdlg ) ;
       }
    }
    else
    {
      ::wxMessageBox( wxT("no start and/or stop function pointers for "
          "volt detect DLL access") );
    }
  }
  else
  {
    ::wxMessageBox( wxT("unstable volt detect DLL access not initialized") );
  }
}

void wxX86InfoAndControlApp::StopInstableCPUcoreVoltageDetection()
{
  if( wxGetApp().m_hmoduleUnstableVoltageDetectionDynLib )
  {
    if( //wxGetApp().m_pfnStartInstableCPUcoreVoltageDetection && wxGetApp().
      m_pfnStopInstableCPUcoreVoltageDetection)
    {
      DWORD dwExitCode;
     ::GetExitCodeThread(wxGetApp().m_x86iandc_threadFindLowestStableVoltage.
       m_handleThread, & dwExitCode ) ;
     if( //m_p_wxbuttonFindLowestStableVoltage->GetLabel() ==
       //wxT("stop finding the lowest stable voltage")
    //        ::WaitForSingleObject( wxGetApp().
    //          m_x86iandc_threadFindLowestStableVoltage.m_handleThread,
    //          //wxGetApp().m_x86iandc_threadFindLowestStableVoltage
    //          0)
    //          ==
         dwExitCode == STILL_ACTIVE
       )
     {
      LOGN( FULL_FUNC_NAME << "--should stop the find lowest stable voltage thread")

      LOGN( FULL_FUNC_NAME << "--before  calling \""
          << STOP_INSTABLE_CPU_CORE_VOLTAGE_DETECTION_FCT_NAME << "\"" )
      (* //wxGetApp().
        m_pfnStopInstableCPUcoreVoltageDetection)();
      LOGN( FULL_FUNC_NAME << "--after calling \""
        << STOP_INSTABLE_CPU_CORE_VOLTAGE_DETECTION_FCT_NAME << "\"" )

      //wxGetApp().
        ExitFindLowestStableVoltageThread();
     }
     else
       ::wxMessageBox( wxT("Can't stop because: finding the lowest stable "
         "voltage has NOT been started.") );
    }
    else
      ::wxMessageBox( wxT("\"stop\" function pointers for "
          "volt detect DLL access not valid.") );
  }
  else
    ::wxMessageBox( wxT("unstable volt detect DLL access not initialized") );
}

void wxX86InfoAndControlApp::UnloadDetectInstableCPUcoreVoltageDynLib()
{
  LOGN( FULL_FUNC_NAME << "--begin")
  if( m_hmoduleUnstableVoltageDetectionDynLib != NULL)
  {
    ::FreeLibrary(m_hmoduleUnstableVoltageDetectionDynLib);
    //For loading the dyn lib in "InitUnstableVoltageDetectionDynLibAccess()".
    m_hmoduleUnstableVoltageDetectionDynLib = NULL;
  }
}
