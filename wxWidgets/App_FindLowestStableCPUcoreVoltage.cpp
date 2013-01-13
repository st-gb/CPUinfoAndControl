/*
 * App_FindLowestStableCPUcoreVoltage.cpp
 *
 *  Created on: 03.10.2011
 *      Author: Stefan
 */

#include "wx/wx.h" //for wxMessageBox(...) (,etc.)
#include <wx/dynlib.h> //class wxDynamicLibrary::GetDllExt()
#include <wx/event.h>//void wxQueueEvent(wxEvtHandler * dest,wxEvent * event)
#include <wx/filename.h> //wxFileName::GetPathSeparator(...)

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

#ifdef wxUSE_BITMAPTOGGLEBUTTUN
  #include <wx/tglbtn.h> //class wxBitmapToggleButton
#else
  #include <wxWidgets/UserInterface/wx2.9compatibility/wxBitmapToggleButton.hpp>
#endif
//class FreqAndVoltageSettingDlg
#include <wxWidgets/UserInterface/FreqAndVoltageSettingDlg.hpp>
//class wxTextControlDialog
#include <wxWidgets/UserInterface/wxTextControlDialog.hpp>
//::getwxString(...)
#include <wxWidgets/Controller/character_string/wxStringHelper.hpp>
#include <wxWidgets/UserInterface/MainFrame.hpp> //class MainFrame

#include <windows.h> // QueryThreadCycleTime(...)
#ifdef _MSC_VER
  #define _WIN32_WINNT 0x0600 //for QueryThreadCycleTime(...)
#else
  typedef BOOL (WINAPI * pfnQueryThreadCycleTime) (
    HANDLE ThreadHandle,
    PULONG64 CycleTime
  );
  static pfnQueryThreadCycleTime g_pfnquerythreadcycletime;
  static ULONGLONG gs_NumCPUcyclesElapsedForCPUcore0;
  #include <Controller/CPU-related/ReadTimeStampCounter.h> //ReadTSCinOrder();
#endif

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
  LOGN( FULL_FUNC_NAME << " begin")
  FreqAndVoltageSettingDlg * p_freqandvoltagesettingdlg =
    (FreqAndVoltageSettingDlg *) p_v;
  if( p_freqandvoltagesettingdlg)
  {
    uint32_t ui32CPUcoreMask = p_freqandvoltagesettingdlg->GetCPUcoreMask();

    LOGN( FULL_FUNC_NAME << "--before setting thread affinity mask"
        //" to CPU core 0"
        )

    //GetThreadAffinityMask();
    LOGN( FULL_FUNC_NAME << "--after setting thread affinity mask"
        //" to CPU core 0"
        )

    LOGN( FULL_FUNC_NAME << "--before calling the DynLib's \""
      << START_INSTABLE_CPU_CORE_VOLTAGE_DETECTION_FCT_NAME "\" function")
    (* wxGetApp().m_pfnStartInstableCPUcoreVoltageDetection)(
      //p_freqandvoltagesettingdlg->mp_model->m_cpucoredata.m_byNumberOfCPUCores,
      ui32CPUcoreMask,
      & wxGetApp().m_external_caller);
    LOGN( FULL_FUNC_NAME << "--after calling the DynLib's \""
      << START_INSTABLE_CPU_CORE_VOLTAGE_DETECTION_FCT_NAME << "\" function")
    LOGN( FULL_FUNC_NAME << " return 0")
    return 0;
  }
  LOGN( FULL_FUNC_NAME << " return 1")
  return 1;
}

ULARGE_INTEGER GetTimeDiff(FILETIME TimeBefore,FILETIME TimeAfter)
{
  //http://msdn.microsoft.com/en-us/library/ms724284(v=vs.85).aspx:
  //"It is not recommended that you add and subtract values from the
  //FILETIME structure to obtain relative times. Instead, you should copy
  //the low- and high-order parts of the file time to a ULARGE_INTEGER
  //structure, perform 64-bit arithmetic on the QuadPart member, and copy
  //the LowPart and HighPart members into the FILETIME structure."
  ULARGE_INTEGER uliBefore = {TimeBefore.dwLowDateTime, TimeBefore.dwHighDateTime};
  ULARGE_INTEGER uliAfter = {TimeAfter.dwLowDateTime, TimeAfter.dwHighDateTime};

  ULARGE_INTEGER uli;
  uli.QuadPart = (uliAfter.QuadPart - uliBefore.QuadPart);
  return uli;
}

inline void updateUIforCountSecondsDown(
  wxCommandEvent & wxcommand_eventCountSecondsDown,
  unsigned uiSeconds,
  FreqAndVoltageSettingDlg * p_freqandvoltagesettingdlg)
{
  //user interface control updates should/ must be executed in _GUI_ thread
  //    //Adapted from http://docs.wxwidgets.org/2.9.4/classwx_thread_helper.html
  //    wxQueueEvent(p_freqandvoltagesettingdlg,
  //      new wxThreadEvent(wxEVT_COMMAND_MYTHREAD_UPDATE));

  wxcommand_eventCountSecondsDown.SetInt(uiSeconds - 1);
//  wxcommand_eventCountSecondsDown.SetClientData();
  wxPostEvent(p_freqandvoltagesettingdlg, wxcommand_eventCountSecondsDown);
  //    //http://docs.wxwidgets.org/2.8/wx_wxthreadoverview.html:
  //    wxEvtHandler::AddPendingEvent();
  //    p_freqandvoltagesettingdlg->wxEvtHandler::QueueEvent  (   wxEvent *   event )
}

float GetCPUcoreUsageForUnstableCPUcoreOperationDetectionThread(
  //FILETIME & userTimeBefore,
  ULONG64 & ul64ThreadTimeBefore,
  HANDLE handleThread,
  const DWORD dwMilliSecondsToWait)
{
  float fCPUcoreUsage;
  ULONG64 ul64ThreadTimeAfter = 0ULL;
//  if( g_pfnquerythreadcycletime )
//  {
//    BOOL b = (* g_pfnquerythreadcycletime)( handleThread, //ul64ThreadTimeAfter
//      & ul64ThreadTimeAfter);
//    if( b)
//    {
//      LOGN( FULL_FUNC_NAME << " QueryThreadCycleTime for thread "
//        << handleThread << "(ID: " //<< ::GetThreadId(handleThread)
//        << ") :"
//        << ul64ThreadTimeAfter)
//      ULONGLONG ull = ReadTSCinOrder( 1 );
//      LOGN( FULL_FUNC_NAME << " timestampcounter:" << ull)
//      ULONG64 ul64NumCPUcyclesElapsedForCPUcore0 = ull -
//        gs_NumCPUcyclesElapsedForCPUcore0;
//
//      LOGN( FULL_FUNC_NAME << " timestampcounter diff:"
//        << ul64NumCPUcyclesElapsedForCPUcore0)
//
//      ULONG64 ul64ThreadTimeDiff = ul64ThreadTimeAfter - ul64ThreadTimeBefore;
//      LOGN( FULL_FUNC_NAME << " thread cycle time diff:"
//        << ul64ThreadTimeDiff)
//
//      fCPUcoreUsage = (float) ( (double) ul64ThreadTimeDiff /
//          (double) ul64NumCPUcyclesElapsedForCPUcore0);
//      LOGN( FULL_FUNC_NAME << " CPU core usage = cycle time diff/"
//        "timestampcounter diff =" << fCPUcoreUsage)
//
//      gs_NumCPUcyclesElapsedForCPUcore0 = ull;
//    }
//    else
//      LOGN( FULL_FUNC_NAME << " QueryThreadCycleTime failed:"
//        << GetErrorMessageFromLastErrorCodeA() )
//  }
//  else
  {
    FILETIME creationTimeAfter;
    FILETIME exitTimeAfter;
    FILETIME kernelTimeAfter;
    FILETIME userTimeAfter;

    //from http://stackoverflow.com/questions/1393006/how-to-get-the-cpu-usage-per-thread-on-windows-win32
    //maybe see
    //-http://www.tech-archive.net/Archive/Development/microsoft.public.win32.programmer.kernel/2004-10/0689.html
    //-http://blog.kalmbachnet.de/?postid=28 ("Why GetThreadTimes is wrong")
    //-http://groups.google.de/groups?th=4b1324af593749b
    BOOL b = ::GetThreadTimes(
      //_In_   HANDLE hThread,
      handleThread,
      & creationTimeAfter, //_Out_  LPFILETIME lpCreationTime,
      & exitTimeAfter, // _Out_  LPFILETIME lpExitTime,
      & kernelTimeAfter, // _Out_  LPFILETIME lpKernelTime,
      & userTimeAfter // _Out_  LPFILETIME lpUserTime
//      (* FILETIME) & ul64ThreadTimeAfter
      );
    if( b )
    {
      ul64ThreadTimeAfter = (ULONG64) userTimeAfter.dwHighDateTime; // << 32 | ;
      ul64ThreadTimeAfter <<= 32;
      ul64ThreadTimeAfter |= userTimeAfter.dwLowDateTime;
      LOGN( FULL_FUNC_NAME << " user time for instable CPU "
        "core operation DLL "
        " procedure before: " //<< userTimeBefore.dwHighDateTime
        //<< " " << userTimeBefore.dwLowDateTime
        << ul64ThreadTimeBefore << " "
        << " after:"
//        << userTimeAfter.dwHighDateTime << " "
//        << userTimeAfter.dwLowDateTime
        << ul64ThreadTimeAfter
        )
      LOGN( FULL_FUNC_NAME << " kernel time for instable CPU "
        "core operation DLL "
        " procedure after:" << kernelTimeAfter.dwHighDateTime << " "
        << kernelTimeAfter.dwLowDateTime)

      //ULARGE_INTEGER uli = GetTimeDiff(userTimeBefore,userTimeAfter);
      ULARGE_INTEGER uli;
      uli.QuadPart = ul64ThreadTimeAfter - ul64ThreadTimeBefore;
      LOGN( FULL_FUNC_NAME << " user time diff for instable CPU core operation DLL "
        " procedure [100ns]: " << uli.QuadPart)
      fCPUcoreUsage = //100-nanosecond intervals
        (float) ( (double) uli.QuadPart / (double) (dwMilliSecondsToWait
        //get in 100 ns: ms (10^-3) -> 100ns (10^-7) => 10^-3 - 10^-7=10^5
        * 10000) );
      LOGN( FULL_FUNC_NAME << " CPU core usage:"//"= user time diff/"
        //"# 100 nanoseconds to wait ="
        << fCPUcoreUsage)
    }
    else
      LOGN_WARNING( " calling GetThreadTimes failed: "
        << ::GetErrorMessageFromLastErrorCodeA() )
  }
//  userTimeBefore = userTimeAfter;
  ul64ThreadTimeBefore = ul64ThreadTimeAfter;
  return fCPUcoreUsage;
}

ULONG64 GetThreadStartTime(HANDLE hThread)
{
  ULONG64 ul64 = 0ULL;
//  DWORD dwMajorVersion;
//  DWORD dwMinorVersion;
  //see http://msdn.microsoft.com/en-us/library/ms684943(v=vs.85).aspx
  HINSTANCE hinstanceKernel32DLL =
    //If the function fails, the return value is NULL.
    ::LoadLibraryA( "Kernel32.dll" //LPCSTR
      );
  std::string strFuncName = "QueryThreadCycleTime" ;
  g_pfnquerythreadcycletime = (pfnQueryThreadCycleTime)
    ::GetProcAddress(
    hinstanceKernel32DLL, strFuncName.c_str() );
  ::CloseHandle(hinstanceKernel32DLL);

//  GetWindowsVersion(dwMajorVersion, dwMinorVersion);
//  if( dwMajorVersion < 6 ) // < Win Vista <=> win XP etc.
  if( ! g_pfnquerythreadcycletime)
  {
    FILETIME creationTime;
    FILETIME exitTime;
    FILETIME kernelTime;
    FILETIME userTimeBefore;
    //from http://stackoverflow.com/questions/1393006/how-to-get-the-cpu-usage-per-thread-on-windows-win32
    //maybe see
    //-http://www.tech-archive.net/Archive/Development/microsoft.public.win32.programmer.kernel/2004-10/0689.html
    //-http://blog.kalmbachnet.de/?postid=28 ("Why GetThreadTimes is wrong")
    //-http://groups.google.de/groups?th=4b1324af593749b
    if( GetThreadTimes(
        hThread, //_In_   HANDLE hThread,
        & creationTime, //_Out_  LPFILETIME lpCreationTime,
        & exitTime, // _Out_  LPFILETIME lpExitTime,
        & kernelTime, // _Out_  LPFILETIME lpKernelTime,
        & userTimeBefore // _Out_  LPFILETIME lpUserTime);
        )
      )
    {
      //ul64 = (ULONG64) userTimeBefore;
      ul64 = userTimeBefore.dwHighDateTime;
      ul64 <<= 32;
      ul64 |= userTimeBefore.dwLowDateTime;
    }
    else
    {
      LOGN_WARNING( " calling GetThreadTimes failed: "
        << ::GetErrorMessageFromLastErrorCodeA() )
    }
  }
//  else
//  if( g_pfnquerythreadcycletime)
//  {
//    BOOL b = //::QueryThreadCycleTime(
//      (* g_pfnquerythreadcycletime)(
//      hThread, //_In_   HANDLE ThreadHandle,
//      & ul64 //_Out_  PULONG64 CycleTime
//    );
//    if( b )
//      LOGN( FULL_FUNC_NAME << " QueryThreadCycleTime:" << ul64)
//    else
//      LOGN_TYPE( " calling GetThreadTimes failed: "
//        << ::GetErrorMessageFromLastErrorCodeA(),
//        I_LogFormatter::log_message_typeWARNING)
//    gs_NumCPUcyclesElapsedForCPUcore0 = ReadTSCinOrder( 1 );
//    LOGN( FULL_FUNC_NAME << " timestampcounter for 1st measurement:"
//      << gs_NumCPUcyclesElapsedForCPUcore0)
//  }
  return ul64;
}

void CountSecondsDown(FreqAndVoltageSettingDlg * p_freqandvoltagesettingdlg,
  DWORD dwMilliSecondsToWait,
  const x86IandC::thread_type & x86iandc_threadFindLowestStableVoltageInDLL)
{
  LOGN( FULL_FUNC_NAME << " begin")

#ifdef _WIN32
    LOGN( FULL_FUNC_NAME << "--thread handle: "
      << x86iandc_threadFindLowestStableVoltageInDLL.m_handleThread)
  ULONG64 ul64ThreadTimeBefore = GetThreadStartTime(
    x86iandc_threadFindLowestStableVoltageInDLL.m_handleThread);
#endif
    //    //http://wiki.wxwidgets.org/Custom_Events:

    //    wxEvent wxcommand_eventCountSecondsDown(uiSeconds - 1);
        wxCommandEvent wxcommand_eventCountSecondsDown(
    //      MyExcitingEvent, //wxEventType commandType = wxEVT_NULL,
    //      wxEVT_NULL,
          wxEVT_COMMAND_COUNT_SECONDS_DOWN_UPDATE
    //      uiSeconds - 1 //int winid = 0
          );
  wxcommand_eventCountSecondsDown.SetClientData(& p_freqandvoltagesettingdlg->
    m_instablecpuoperationdetectiondata);
  const float fMinCPUcoreUsage = wxGetApp().m_model.
      m_instablecpucorevoltagedetection.m_fMinCPUcoreUsage;
  for( unsigned uiSeconds = p_freqandvoltagesettingdlg->mp_model->
      m_instablecpucorevoltagedetection.
      m_uiNumberOfSecondsToWaitUntilVoltageIsReduced //+ 1
      ; uiSeconds != 0 &&
    //Is set to true when "VoltageTooLow" was called.
    ! wxGetApp().m_vbExitFindLowestStableVoltage;// -- uiSeconds
      )
  {
    LOGN( FULL_FUNC_NAME << "--waiting max. " << dwMilliSecondsToWait
      << " milliseconds")
    if( wxGetApp().m_conditionFindLowestStableVoltage.WaitTimeOut(
        dwMilliSecondsToWait) == I_Condition::signaled
        )
      //If it got signaled (-> finding lowest stable voltage should be canceled).
    {
      LOGN( FULL_FUNC_NAME << "--condition got signalled" )
//      break;
      uiSeconds = 0;
      wxGetApp().m_conditionFindLowestStableVoltage.ResetEvent();
    }
    else
      LOGN( FULL_FUNC_NAME << "--timed out" )
    LOGN( FULL_FUNC_NAME << "--after waiting for either thread's end or "
        "timeout ")
    if( uiSeconds ) // "VoltageTooLow" not called
    {
#ifdef _WIN32
      float fCPUcoreUsage = GetCPUcoreUsageForUnstableCPUcoreOperationDetectionThread(
        //userTimeBefore,
        ul64ThreadTimeBefore,
        x86iandc_threadFindLowestStableVoltageInDLL.m_handleThread,
        dwMilliSecondsToWait);
//      LOGN( FULL_FUNC_NAME << " CPU core usage for instable CPU core operation "
//          "detection DLL thread:" << fCPUcoreUsage * 100.0f )
      p_freqandvoltagesettingdlg->m_instablecpuoperationdetectiondata.seconds
        = uiSeconds;
      p_freqandvoltagesettingdlg->m_instablecpuoperationdetectiondata.
        CPUusageInPercent = fCPUcoreUsage;
      updateUIforCountSecondsDown(wxcommand_eventCountSecondsDown,
        uiSeconds,
        p_freqandvoltagesettingdlg);
      if( fCPUcoreUsage < //0.95
          fMinCPUcoreUsage)
      {
//        LOGN( FULL_FUNC_NAME << "-- < " << fMinCPUcoreUsage * 100.0f << " "
//          << "% user time for unstable CPU core operation detection procedure")
      }
      else
      {
        -- uiSeconds;
      }
#endif
    }
    //else
    if( wxGetApp().m_bVoltageWasTooLowCalled )
    {
      LOGN( FULL_FUNC_NAME << " waiting for \"VoltageTooLow()\" to be finished")
      //Wait for "::VoltageTooLow()" to be ended (after messagebox was shown).
      wxGetApp().m_conditionFindLowestStableVoltage.Wait();
      LOGN( FULL_FUNC_NAME << " after waiting for \"VoltageTooLow()\" to be finished")
    }
  }
  LOGN( FULL_FUNC_NAME << " end")
}

BYTE GetNumberOfSelectedCPUcores(uint32_t ui32CPUcoreMask)
{
  BYTE byNumberOfSelectedCPUcores = 0;
  for(BYTE byCPUcoreIndex = 0; byCPUcoreIndex < 32; ++ byCPUcoreIndex)
    byNumberOfSelectedCPUcores += ( (ui32CPUcoreMask >> byCPUcoreIndex) & 1);
  return byNumberOfSelectedCPUcores;
}

const x86IandC::thread_type &
  StartInstableCPUcoreVoltageDetectionInDynLibInSeparateThread(
  FreqAndVoltageSettingDlg * p_freqandvoltagesettingdlg)
{
  LOGN( FULL_FUNC_NAME << " begin" )
//  if( //1threadForEveryCore
//      true)
//  {
//    uint32_t ui32CPUcoreMask = p_freqandvoltagesettingdlg->GetCPUcoreMask();
//    const BYTE byNumCores = GetNumberOfSelectedCPUcores(ui32CPUcoreMask);
//    x86IandC::thread_type x86iandc_threadFindLowestStableVoltage[byNumCores];
//    for( BYTE selectedCoreIndex = 0; selectedCoreIndex < byNumCores ;
//        ++ selectedCoreIndex)
//    {
//      x86iandc_threadFindLowestStableVoltage[selectedCoreIndex].start(
//        StartInstableCPUcoreVoltageDetectionInDLL ,
//        p_freqandvoltagesettingdlg ) ;
//  }
//  else
//  {
//    x86IandC::thread_type x86iandc_threadFindLowestStableVoltage;
    wxGetApp().m_x86iandc_threadFindLowestStableCPUcoreOperationInDLL.start(
      StartInstableCPUcoreVoltageDetectionInDLL ,
      p_freqandvoltagesettingdlg,
      I_Thread::maximum_priority //thread priority
      ) ;
//    StartInstableCPUcoreVoltageDetectionInDLL(p_freqandvoltagesettingdlg);
//  }
  LOGN( FULL_FUNC_NAME << "--thread handle: "
    << wxGetApp().m_x86iandc_threadFindLowestStableCPUcoreOperationInDLL.
    m_handleThread)

  LOGN( FULL_FUNC_NAME << " end")
  return //x86iandc_threadFindLowestStableVoltage;
    wxGetApp().m_x86iandc_threadFindLowestStableCPUcoreOperationInDLL;
}

#define NO_NEGATIVE_OVERFLOW(number, max_number) ( number != max_number )

float DecreaseVoltageStepByStep(
  FreqAndVoltageSettingDlg * p_freqandvoltagesettingdlg,
  WORD wCPUcoreVoltageArrayIndex,
  const float fCPUcoreMultiplier,
  const x86IandC::thread_type & x86iandc_threadFindLowestStableVoltageInDLL
  )
{
  float fVoltageInVolt = //0.0f;
    p_freqandvoltagesettingdlg->mp_model->
    m_cpucoredata.m_arfAvailableVoltagesInVolt[wCPUcoreVoltageArrayIndex];
  LOGN( FULL_FUNC_NAME << "wCPUcoreVoltageArrayIndex:"
    << wCPUcoreVoltageArrayIndex << " (=" << fVoltageInVolt << "V),"
    << "CPU core multiplier:" << fCPUcoreMultiplier << " begin")
  DWORD dwMilliSecondsToWait = 1000;
  for( //-- wCPUcoreVoltageArrayIndex
    ; //wCPUcoreVoltageArrayIndex != MAXWORD &&
      NO_NEGATIVE_OVERFLOW(wCPUcoreVoltageArrayIndex, MAXWORD) &&
    //Is set to true when "VoltageTooLow" was called.
    ! wxGetApp().m_vbExitFindLowestStableVoltage;
    -- wCPUcoreVoltageArrayIndex )
  {
    LOGN(FULL_FUNC_NAME << " index:" << wCPUcoreVoltageArrayIndex )
    fVoltageInVolt = p_freqandvoltagesettingdlg->mp_model->
      m_cpucoredata.m_arfAvailableVoltagesInVolt[wCPUcoreVoltageArrayIndex];
    p_freqandvoltagesettingdlg->mp_model->m_instablecpucorevoltagedetection.
      m_lastSetCPUcoreVoltageInVolt = fVoltageInVolt;

    //The reference clock can be changed via special tools etc., so get it
    //every time.
    const float r_fReferenceClockInMHz =
      //p_freqandvoltagesettingdlg->mp_cpucontroller->
      wxGetApp().mp_cpucontroller->m_fReferenceClockInMHz;

    if( //p_freqandvoltagesettingdlg->mp_model->m_userinterfaceattributes.
        //m_bPreventVoltageBelowLowestStableVoltage &&
        p_freqandvoltagesettingdlg->
  //            mp_wxcheckboxPreventVoltageBelowLowestStableVoltage->IsChecked()
          m_p_wxbitmaptogglebuttonPreventVoltageBelowLowestStableVoltage->
          GetValue()
        &&
        //p_freqandvoltagesettingdlg->mp_cpucontroller->
        wxGetApp().mp_cpucontroller->
        //CheckWhetherVoltageIsBelowLowestStableVoltage(
        CheckWhetherCPUcoreVoltageIsBelowHighestInstableVoltage(
          fVoltageInVolt,
          r_fReferenceClockInMHz * fCPUcoreMultiplier) ==
          I_CPUcontroller::VoltageIsOutsideSafeRange
      )
    {
      LOGN( FULL_FUNC_NAME << "--voltage " << fVoltageInVolt
        << " is below "
          //"lowest stable "
          "highest instable "
          "CPU core voltage "
          " for CPU core multiplier " << fCPUcoreMultiplier)
      wxGetApp().StabilizeVoltageAndRepaintMainFrame(
        fVoltageInVolt,
        fCPUcoreMultiplier,
        r_fReferenceClockInMHz);
      wxGetApp().StopInstableCPUcoreVoltageDetection();
      //In order to set the multi as lowest multi for instable CPU operation in
      // AutoConfigureVoltageSettings_ThreadFunc(...) function.
      wxGetApp().m_bVoltageWasTooLowCalled = true;
      break;
    }
    LOGN( FULL_FUNC_NAME << "--before setting voltage[Volt] to:"
        << fVoltageInVolt << " , multi to " << fCPUcoreMultiplier)
    p_freqandvoltagesettingdlg->mp_cpucontroller->
      SetCurrentVoltageAndMultiplier(
        fVoltageInVolt, //fMultiplier
        fCPUcoreMultiplier, 0);

    CountSecondsDown(p_freqandvoltagesettingdlg, dwMilliSecondsToWait,
      x86iandc_threadFindLowestStableVoltageInDLL);
  }
  LOGN( FULL_FUNC_NAME << " return " << fVoltageInVolt)
  return fVoltageInVolt;
}

DWORD THREAD_PROC_CALLING_CONVENTION FindLowestStableVoltage_ThreadProc(
  void * p_v )
{
  LOGN( FULL_FUNC_NAME << "--begin")
//  //Reset to initial value in order to differentiate between whether the
//  //voltage was too low later.
//  wxGetApp().m_bVoltageWasTooLowCalled = false;
  FreqAndVoltageSettingDlg * p_freqandvoltagesettingdlg =
    (FreqAndVoltageSettingDlg *) p_v;
  if( p_freqandvoltagesettingdlg)
  {
    const x86IandC::thread_type & x86iandc_threadFindLowestStableVoltageInDLL =
    StartInstableCPUcoreVoltageDetectionInDynLibInSeparateThread(
      p_freqandvoltagesettingdlg);

//    float fVoltageInVolt = p_freqandvoltagesettingdlg->
//      GetVoltageInVoltFromSliderValue() ;

    wxString wxstrMessageFromService;
    //TODO exec in GUI thread.
//    p_freqandvoltagesettingdlg->DisableOSesDVFSandServiceDVFS(
//      wxstrMessageFromService);

    const std::set<float> & c_r_stdset_floatAvailableVoltagesInVolt =
      p_freqandvoltagesettingdlg->mp_model->m_cpucoredata.
      m_stdset_floatAvailableVoltagesInVolt;

    WORD wCPUcoreVoltageArrayIndex = ::GetArrayIndexForClosestGreaterOrEqual(
      p_freqandvoltagesettingdlg->mp_model->m_cpucoredata.
        m_arfAvailableVoltagesInVolt,
      c_r_stdset_floatAvailableVoltagesInVolt.size(),
//      fVoltageInVolt
      p_freqandvoltagesettingdlg->mp_model->m_instablecpucorevoltagedetection.
      m_lastSetCPUcoreVoltageInVolt
      );
    const float fMultiplier = p_freqandvoltagesettingdlg->mp_model->
      m_instablecpucorevoltagedetection.m_lastSetCPUcoreMultiplier;

//    p_freqandvoltagesettingdlg->m_p_wxbuttonFindLowestStableVoltage->SetLabel(
//      wxT("stop finding the lowest stable voltage") );

//    p_freqandvoltagesettingdlg->
//      ChangeToStopFindingLowestStableCPUcoreVoltageButton();

//    //Multipliers can also be floats: e.g. "5.5" for AMD Griffin.
//    const float fMultiplierFromSliderValue = p_freqandvoltagesettingdlg->
//      GetMultiplierFromSliderValue();
//    p_freqandvoltagesettingdlg->mp_model->m_instablecpucorevoltagedetection.
//      m_lastSetCPUcoreMultiplier = fMultiplierFromSliderValue;

    p_freqandvoltagesettingdlg->mp_model->m_instablecpucorevoltagedetection.
      m_fReferenceClockInMHz = p_freqandvoltagesettingdlg->mp_cpucontroller->
      m_fReferenceClockInMHz;

//    const float fLastSetVoltageInVolt =
      DecreaseVoltageStepByStep(
      p_freqandvoltagesettingdlg,
      wCPUcoreVoltageArrayIndex, fMultiplier,
      x86iandc_threadFindLowestStableVoltageInDLL);

//    //When the min. reached voltage and no unstable voltage was detected it may
//    // be unstable though: the voltage where calculation errors may occur may
//    //be very close to the minimal voltage: e.g. for a Pentium M 1.8 GHz
//    //when reached the min voltage of 0.7 V at 600 MHz: no calculation error is
//    //detected this minmal voltage of 0.7V is close to the voltage (ca. 0.68V)
//    //where a calculation error is likely to happen.
//    if( ! wxGetApp().m_bVoltageWasTooLowCalled )
//    {
////      float fVoltageInVolt;
////      float fMultiplier;
////      float fReferenceClockInMHz;
////      wxGetApp().mp_cpucontroller->GetCurrentVoltageAndFrequency(
////        fVoltageInVolt,
////        fMultiplier, fReferenceClockInMHz, 0);
//
//      LOGN(FULL_FUNC_NAME << " stabilizing voltage because we reached the "
//          "lowest voltage and may be close to an unstable voltage")
//      wxGetApp().StabilizeVoltage(
////        fVoltageInVolt,
//        fLastSetVoltageInVolt,
////        fMultiplier,
//        fMultiplier,
////        fReferenceClockInMHz
//        p_freqandvoltagesettingdlg->mp_model->m_instablecpucorevoltagedetection.
//        m_fReferenceClockInMHz
//        );
//    }
//    else
//      wxGetApp().m_bVoltageWasTooLowCalled = false;

    p_freqandvoltagesettingdlg->SetStartFindingLowestStableVoltageButton();
//    wxGetApp().ExitFindLowestStableVoltageThread();
    wxGetApp().StopInstableCPUcoreVoltageDetection();

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

void wxX86InfoAndControlApp::InitUnstableCPUcoreOperationDetection()
{
  PossiblyAskForOSdynFreqScalingDisabling();
  InitUnstableVoltageDetectionDynLibAccess();
}

BYTE wxX86InfoAndControlApp::InitUnstableVoltageDetectionDynLibAccess()
{
  LOGN( FULL_FUNC_NAME << " --begin")
//  LPTSTR lptstrUnstableVoltageDetectionDynLib = _T(//"Prime95.DLL"
//    //"UnstableVoltageDetection.dll"
//      "InstableCPUcoreVoltageDetection.dll"
//      );
//  MessageWithTimeStamp();
  wxTextControlDialog wxtextcontroldialog( wxNow() +
    wxT(" loading the dynamic library ") +
    m_model.m_instablecpucorevoltagedetection.m_std_wstrDynLibPath,
    wxT("current action"), 0, 0);
  //Loading may take some seconds depending on e.g. DLL size and CPU speed so
  //inform the user by via user interface.
  wxtextcontroldialog.Show(true);
  if(m_hmoduleUnstableVoltageDetectionDynLib == NULL)
    m_hmoduleUnstableVoltageDetectionDynLib = //::LoadLibrary(
      //lptstrUnstableVoltageDetectionDynLib
    ::LoadLibraryW( //m_std_wstrInstableCPUcoreVoltageDynLibPath.c_str()
      m_model.m_instablecpucorevoltagedetection.m_std_wstrDynLibPath.c_str() );
  LOGN( FULL_FUNC_NAME << " --after loading the dyn lib")
  wxtextcontroldialog.Show(false);
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
      //TODO should only be called in GUI thread
//      mp_frame->m_p_wxmenuitemUnloadDetectInstableCPUcoreVoltageDynLib->
//        SetHelp(wxstrDynLibFilePath);
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

BYTE wxX86InfoAndControlApp::StartInstableCPUcoreVoltageDetection(
    const FreqAndVoltageSettingDlg * c_p_freqandvoltagesettingdlg)
{
  LOGN( FULL_FUNC_NAME << " begin")
  BYTE ret = 1;
  //wxGetApp().
//    InitUnstableVoltageDetectionDynLibAccess();

  if( //wxGetApp().
      m_hmoduleUnstableVoltageDetectionDynLib )
  {
    if( //wxGetApp().
        m_pfnStartInstableCPUcoreVoltageDetection && //wxGetApp().
        m_pfnStopInstableCPUcoreVoltageDetection)
    {
      if( //m_p_wxbuttonFindLowestStableVoltage->GetLabel() ==
         //wxT("stop finding the lowest stable voltage")
           m_x86iandc_threadFindLowestStableVoltage.IsRunning()
         )
      {
        LOGN( "Finding the lowest stable voltage has already "
          "been started.")
        MessageWithTimeStamp(
          wxT("Finding the lowest stable voltage has already "
           "been started.") );
        return 1;
      }
      else
      {
        LOGN( FULL_FUNC_NAME << "--should start the find lowest stable "
            "voltage thread")
      //wxGetApp().
        m_vbExitFindLowestStableVoltage = false;

        //Start finding voltage in another thread. Else it couldn't be stopped
        //when it was started in the GUI thread.
      ret =
      //wxGetApp().
        m_x86iandc_threadFindLowestStableVoltage.start(
          FindLowestStableVoltage_ThreadProc , (void *) c_p_freqandvoltagesettingdlg ) ;
       }
      if( ret == I_Thread::no_error )
      {
        ret = 0;
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
  LOGN( FULL_FUNC_NAME << " return " << (WORD) ret)
  return ret;
}

void wxX86InfoAndControlApp::StopInstableCPUcoreVoltageDetection()
{
  LOGN( FULL_FUNC_NAME << "--begin")
  if( wxGetApp().m_hmoduleUnstableVoltageDetectionDynLib )
  {
    if( //wxGetApp().m_pfnStartInstableCPUcoreVoltageDetection && wxGetApp().
      m_pfnStopInstableCPUcoreVoltageDetection)
    {
     if( //m_p_wxbuttonFindLowestStableVoltage->GetLabel() ==
       //wxT("stop finding the lowest stable voltage")
         m_x86iandc_threadFindLowestStableVoltage.IsRunning()
       )
     {
      LOGN( FULL_FUNC_NAME << "--should stop the \"find lowest stable voltage\" thread")

      LOGN( FULL_FUNC_NAME << "--before calling dyn lib's \""
          << STOP_INSTABLE_CPU_CORE_VOLTAGE_DETECTION_FCT_NAME << "\" function" )
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
#ifdef _WIN32
  if( m_hmoduleUnstableVoltageDetectionDynLib != NULL)
  {
    ::FreeLibrary(m_hmoduleUnstableVoltageDetectionDynLib);
    //For loading the dyn lib in "InitUnstableVoltageDetectionDynLibAccess()".
    m_hmoduleUnstableVoltageDetectionDynLib = NULL;
  }
#endif
}

void wxX86InfoAndControlApp::LoadDetectInstableCPUcoreVoltageDynLib(
  //wxCommandEvent & event
  )
{
  wxString wxstrDynLibExtension = wxDynamicLibrary::GetDllExt();
  wxString wxstrDynLibFilePath = ::wxFileSelector(
    wxT("Select dynamic library for detecting an unstable CPU core operation"
        //"voltage"
      )
    //"default_path"
    , //wxEmptyString

      m_wxstrDirectoryForLastSelectedInstableCPUcoreVoltageDynLib
    , wxEmptyString
    , wxDynamicLibrary::GetDllExt()
    , wxT("*") + wxstrDynLibExtension
    , wxFD_OPEN
    ) ;
  if ( ! wxstrDynLibFilePath.empty() )
  {
      m_wxstrDirectoryForLastSelectedInstableCPUcoreVoltageDynLib =
      wxstrDynLibFilePath.Left(
        //0-based index of path separator = # of chars before it.
        wxstrDynLibFilePath.rfind(
        wxFileName::GetPathSeparator() )
        );
    //Unload attached dyn lib at first.
    UnloadDetectInstableCPUcoreVoltageDynLib();
//    std::wstring std_wstrInstableCPUcoreVoltageDynLibPath = ::GetStdWstring(
//      wxstrDynLibFilePath);
    //m_std_wstrInstableCPUcoreVoltageDynLibPath =
      m_model.m_instablecpucorevoltagedetection.m_std_wstrDynLibPath =
      ::GetStdWstring(wxstrDynLibFilePath);

    if( //LoadDetectInstableCPUcoreVoltageDynLib(
      //std_wstrInstableCPUcoreVoltageDynLibPath);
      InitUnstableVoltageDetectionDynLibAccess() == 0)
//      m_p_wxmenuitemUnloadDetectInstableCPUcoreVoltageDynLib->SetHelp(
//        wxstrDynLibFilePath);
    { //By using a block/ braces: avoid g++ warning "Suspicious semicolon"
      ;
    }
  }
}
