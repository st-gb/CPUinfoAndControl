#include "I_CPUcontroller.hpp"
#include "UserInterface.hpp"
#include <Controller/CPUindependentHelper.h>
#include <Controller/tchar_conversion.h> //for GetCharPointer(...)
#include <Controller/stdtstr.hpp> //get...
#include <ModelData/ModelData.hpp> //class Model
#include <ModelData/CPUcoreData.hpp> //PerCPUcoreAttributes
#include <Windows_compatible_typedefs.h>

#ifdef COMPILE_WITH_XERCES
  #include "Xerces/XMLAccess.h" //for "readXMLConfig(...)"
  #include <Xerces/SAX2MainConfigHandler.hpp>
#endif

I_CPUcontroller::I_CPUcontroller()
  : mp_userinterface (NULL)
  , mp_model (NULL)
  , mp_cpuaccess (NULL)
  , mp_calculationthread (NULL)
  , mp_dynfreqscalingaccess (NULL)
  , mp_icpucoreusagegetter (NULL)
{
}

//This CPU instruction (in contrast to wrmsr) is not dangerous.
//So it can be implemented for every CPU controller in this base class.
BOOL I_CPUcontroller::CpuidEx(
  DWORD dwIndex,
  PDWORD p_dwEAX,
  PDWORD p_dwEBX,
  PDWORD p_dwECX,
  PDWORD p_dwEDX,
  DWORD_PTR affinityMask
)
{
  if( mp_cpuaccess )
  {
    return mp_cpuaccess->CpuidEx(
      dwIndex,
      p_dwEAX,
      p_dwEBX,
      p_dwECX,
      p_dwEDX,
      affinityMask
    ) ;
  }
  return FALSE ;
}


BYTE I_CPUcontroller::DisableFrequencyScalingByOS() 
{
  if( mp_dynfreqscalingaccess )
    return mp_dynfreqscalingaccess->DisableFrequencyScalingByOS() ;
  return 0 ;
}

BYTE I_CPUcontroller::EnableOwnDVFS()
{
  LOGN("enable own DVFS--ptrs:" << mp_model << mp_dynfreqscalingaccess )
  if( mp_model && mp_dynfreqscalingaccess )
  {
    LOGN("should enable DVFS?:" << mp_model->m_cpucoredata.m_bEnableDVFS )
    if( mp_model->m_cpucoredata.m_bEnableDVFS )
    {
      if( mp_dynfreqscalingaccess->OtherDVFSisEnabled() 
        )
        DisableFrequencyScalingByOS();
      PerCPUcoreAttributes * p_percpucoreattributes = 
        & mp_model->m_cpucoredata.
        m_arp_percpucoreattributes[ //p_atts->m_byCoreID 
        0 ] ;
      LOGN("freq scaling thread:" << p_percpucoreattributes->mp_dynfreqscalingthread )
      //DynFreqScalingThread * p_dynfreqscalingthread
      if ( ! p_percpucoreattributes->mp_dynfreqscalingthread )
      {
        LOGN("Other DVFS is enabled?:" << 
          mp_dynfreqscalingaccess->OtherDVFSisEnabled() 
          )
        if( ! //mp_pumastatectrl->mp_dynfreqscalingaccess->OtherDVFSisEnabled() 
            mp_dynfreqscalingaccess->OtherDVFSisEnabled() 
          )
        {
          //p_percpucoreattributes->mp_dynfreqscalingthread 
          p_percpucoreattributes->SetCPUcontroller( this ) ;
          p_percpucoreattributes->CreateDynFreqScalingThread( 
            mp_icpucoreusagegetter
            ) ;
          return 1 ;
        }
      }
    }
  }
  return 0 ;
}

// returns: true: p-state with freq >= wanted freq and 
//    p-state with freq <= wanted freq was found and
//    voltage could be interpolated 
BYTE I_CPUcontroller::GetInterpolatedVoltageFromFreq(
  WORD wFreqInMHzToGetVoltageFrom
  , float & r_fVoltageInVolt 
  , std::set<VoltageAndFreq> & r_stdsetvoltageandfreq
  )
{
  //WORD wFreqInMHzFromNearFreqAboveWantedFreq =
    //mp_model->m_pstates.m_arp_pstate[0]->GetFreqInMHz() ;
  //PState * p_pstateGreaterEqual = mp_model->m_pstates.
  //  GetPstateWithNearestFreqGreaterEqual( wFreqInMHzToGetVoltageFrom , 4) ;
  ////mp_model->
  //PState * p_pstateLowerEqual = mp_model->m_pstates.
  //  GetPstateWithNearestFreqLowerEqual( wFreqInMHzToGetVoltageFrom, 4 ) ;
  std::set<VoltageAndFreq>::const_iterator ci_stdsetvoltageandfreq = 
    r_stdsetvoltageandfreq.begin() ;
  std::set<VoltageAndFreq>::const_iterator 
    ci_stdsetvoltageandfreqNearestLowerEqual = r_stdsetvoltageandfreq.end() ;
  std::set<VoltageAndFreq>::const_iterator 
    ci_stdsetvoltageandfreqNearestHigherEqual = r_stdsetvoltageandfreq.end() ;
  while( ci_stdsetvoltageandfreq != r_stdsetvoltageandfreq.end() )
  {
    if( ci_stdsetvoltageandfreq->m_wFreqInMHz <= wFreqInMHzToGetVoltageFrom )
      ci_stdsetvoltageandfreqNearestLowerEqual = ci_stdsetvoltageandfreq ;
    if( ci_stdsetvoltageandfreq->m_wFreqInMHz >= wFreqInMHzToGetVoltageFrom )
    {
      ci_stdsetvoltageandfreqNearestHigherEqual = ci_stdsetvoltageandfreq ;
      //The entries are sorted ascending by frequency. So break to avoid 
      //assigning a higher value (that would not be the nearest higher freq
      //any more).
      break ;
    }
    ++ ci_stdsetvoltageandfreq ;
  }
  if( //mp_model->m_pstates.m_arp_pstate[0] &&
    //mp_model->m_pstates.m_arp_pstate[0]->GetFreqInMHz() >
    //wFreqInMHzToGetVoltageFrom 
    //p_pstateGreaterEqual && p_pstateLowerEqual
    ci_stdsetvoltageandfreqNearestLowerEqual != r_stdsetvoltageandfreq.end()
    && ci_stdsetvoltageandfreqNearestHigherEqual != 
      r_stdsetvoltageandfreq.end()
    )
  {
    if( //This is the case if wFreqInMHzToGetVoltageFrom has the 
      //same freq as one of the p-states.
      //This case must be catched, else wrong values by the 
      //log_x() function (  log_1(1)  is calculated then ) .
      //p_pstateGreaterEqual == p_pstateLowerEqual 
      ci_stdsetvoltageandfreqNearestLowerEqual == 
      ci_stdsetvoltageandfreqNearestHigherEqual
      )
    {
      //r_fVoltageInVolt = p_pstateGreaterEqual->GetVoltageInVolt() ;
      r_fVoltageInVolt = ci_stdsetvoltageandfreqNearestLowerEqual->
        m_fVoltageInVolt ;
      return true ;
    }
    else
    {
      WORD wFreqInMHzFromNearFreqAboveWantedFreq =
        //p_pstateGreaterEqual->GetFreqInMHz() ;
        ci_stdsetvoltageandfreqNearestHigherEqual->m_wFreqInMHz ;
      WORD wFreqInMHzFromNearFreqBelowWantedFreq =
        //p_pstateLowerEqual->GetFreqInMHz() ;
        ci_stdsetvoltageandfreqNearestLowerEqual->m_wFreqInMHz ;
    //if( mp_model->m_pstates.m_arp_pstate[1] &&
    //  mp_model->m_pstates.m_arp_pstate[1]->GetFreqInMHz() <
    //  wFreqInMHzToGetVoltageFrom
    //  )
    //{
      float fVoltageInVoltFromNearFreqAboveWantedFreq ;
      float fVoltageInVoltFromNearFreqBelowWantedFreq ;
      fVoltageInVoltFromNearFreqAboveWantedFreq =
        //mp_model->m_pstates.m_arp_pstate[0]->GetVoltageInVolt() ;
        //p_pstateGreaterEqual->GetVoltageInVolt() ;
        ci_stdsetvoltageandfreqNearestHigherEqual->
          m_fVoltageInVolt ;
      fVoltageInVoltFromNearFreqBelowWantedFreq =
        //mp_model->m_pstates.m_arp_pstate[1]->GetVoltageInVolt() ;
        //p_pstateLowerEqual->GetVoltageInVolt() ;
        ci_stdsetvoltageandfreqNearestLowerEqual->
          m_fVoltageInVolt ;
      //example: 1.0625 V - 0.85V = 0,2125 V
      float fVoltageFromFreqAboveAndBelowDiff = 
        fVoltageInVoltFromNearFreqAboveWantedFreq -
        fVoltageInVoltFromNearFreqBelowWantedFreq ;
      WORD wFreqInMHzFromNearFreqsWantedFreqDiff =
        wFreqInMHzFromNearFreqAboveWantedFreq -
       wFreqInMHzFromNearFreqBelowWantedFreq ;
      float fIncrease = 
        fVoltageFromFreqAboveAndBelowDiff /
          wFreqInMHzFromNearFreqsWantedFreqDiff ;
      WORD wWantedFreqMinusFreqBelow = 
        (wFreqInMHzToGetVoltageFrom - 
        wFreqInMHzFromNearFreqBelowWantedFreq
         ) ;

      //example: 2200 MHz / 1050 MHz ~= 2,0952380952
      double dFreqInMHzFromNearFreqAboveDivFreqInMHzToGetVoltageFrom =
          (double) wFreqInMHzFromNearFreqAboveWantedFreq /
          (double) wFreqInMHzToGetVoltageFrom ;

      r_fVoltageInVolt =
        ////fVoltageOfOvVoltProtVnf_pairHigherEqualWantedFreq
        //// 1,0625 - 0,22676176661993910555202664192874
        ////  = 0,835738233380060894447973358072
        //fVoltageInVoltFromNearFreqAboveWantedFreq -
        //  //log10(2,0952380952380952380952380952381) =
        //  //  0,32123338175226816317043359884426
        //  //log10(2) = 0,30102999566398119521373889472449
        //  // => log2(2,0952380952380952380952380952381) = 
        //  //  0,32123338175226816317043359884426 / 
        //  //  0,30102999566398119521373889472449 
        //  //  = 1,067114195858536967303654785547
        //  // 1,067114195858536967303654785547 * 0,2125 =
        //  //   0,22676176661993910555202664192874
        //  //log_dualis
        //  log_x
        //  (
        //    (double) wFreqInMHzFromNearFreqAboveWantedFreq /
        //    (double) wFreqInMHzFromNearFreqBelowWantedFreq
        //    ,
        //  //wFreqOfOvVoltProtVnf_pairHigherEqualWantedFreq
        //    //example: 2200 MHz / 1050 MHz ~= 2.0952380952
        //    //example: 2200 MHz / 550 MHz = 4
        //    (double) wFreqInMHzFromNearFreqAboveWantedFreq /
        //    (double) wFreqInMHzToGetVoltageFrom
        //  )
        //  *
        //  //fVoltageDiffBetwOvVoltProtVnf_pairs
        //  (
        //    //example: 1.0625 V - 0.85V = 0,2125 V
        //    //example: 1.0625 V - 0.75V = 0.3125 V
        //    fVoltageInVoltFromNearFreqAboveWantedFreq -
        //    fVoltageInVoltFromNearFreqBelowWantedFreq
        //  )
        fVoltageInVoltFromNearFreqBelowWantedFreq +
        fIncrease * (float) wWantedFreqMinusFreqBelow
        ;
      return true ;
    }
  }
  return false ;
}

//BYTE I_CPUcontroller::GetInterpolatedVoltageFromFreq(
//  WORD wFreqInMHzToGetVoltageFrom
//  , float & r_fVoltageInVolt 
//  , std::set<VoltageAndFreq> & r_stdsetvoltageandfreq
//  )
//{
//  //WORD wFreqInMHzFromNearFreqAboveWantedFreq =
//    //mp_model->m_pstates.m_arp_pstate[0]->GetFreqInMHz() ;
//  //PState * p_pstateGreaterEqual = mp_model->m_pstates.
//  //  GetPstateWithNearestFreqGreaterEqual( wFreqInMHzToGetVoltageFrom , 4) ;
//  ////mp_model->
//  //PState * p_pstateLowerEqual = mp_model->m_pstates.
//  //  GetPstateWithNearestFreqLowerEqual( wFreqInMHzToGetVoltageFrom, 4 ) ;
//  std::set<VoltageAndFreq>::const_iterator ci_stdsetvoltageandfreq = 
//    r_stdsetvoltageandfreq.begin() ;
//  std::set<VoltageAndFreq>::const_iterator 
//    ci_stdsetvoltageandfreqNearestLowerEqual = r_stdsetvoltageandfreq.
//    //return element with value <= wFreqInMHzToGetVoltageFrom
//    lower_bound (VoltageAndFreq(1.0, wFreqInMHzToGetVoltageFrom) 
//    ) ;
//  std::set<VoltageAndFreq>::const_iterator 
//    ci_stdsetvoltageandfreqNearestHigher = r_stdsetvoltageandfreq.
//    //return element with value > wFreqInMHzToGetVoltageFrom
//    upper_bound(VoltageAndFreq(1.0, wFreqInMHzToGetVoltageFrom) 
//    ) ;
//
//  std::set<VoltageAndFreq>::const_reverse_iterator 
//    c_rev_iter_stdsetvoltageandfreqNearestLowerEqual =
//    ci_stdsetvoltageandfreqNearestHigher ;
//  ++ c_rev_iter_stdsetvoltageandfreqNearestLowerEqual ;
//
//  if( //ci_stdsetvoltageandfreqNearestLowerEqual != 
//      //r_stdsetvoltageandfreq.end()
//    c_rev_iter_stdsetvoltageandfreqNearestLowerEqual !=
//      r_stdsetvoltageandfreq.rend()
//    )
//  {
//    if( ci_stdsetvoltageandfreqNearestHigher != 
//        r_stdsetvoltageandfreq.end()
//      )
//    {
//      if( //This is the case if wFreqInMHzToGetVoltageFrom has the 
//        //same freq as one of the p-states.
//        //This case must be catched, else wrong values by the 
//        //log_x() function (  log_1(1)  is calculated then ) .
//        //p_pstateGreaterEqual == p_pstateLowerEqual 
//        //ci_stdsetvoltageandfreqNearestLowerEqual == 
//        c_rev_iter_stdsetvoltageandfreqNearestLowerEqual ==
//        ci_stdsetvoltageandfreqNearestHigher
//        )
//      {
//        //r_fVoltageInVolt = p_pstateGreaterEqual->GetVoltageInVolt() ;
//        r_fVoltageInVolt = //ci_stdsetvoltageandfreqNearestLowerEqual->
//          c_rev_iter_stdsetvoltageandfreqNearestLowerEqual->
//          m_fVoltageInVolt ;
//        return true ;
//      }
//      else
//      {
//        WORD wFreqInMHzFromNearFreqAboveWantedFreq =
//          //p_pstateGreaterEqual->GetFreqInMHz() ;
//          ci_stdsetvoltageandfreqNearestHigher->m_wFreqInMHz ;
//        WORD wFreqInMHzFromNearFreqBelowWantedFreq =
//          //p_pstateLowerEqual->GetFreqInMHz() ;
//          //ci_stdsetvoltageandfreqNearestLowerEqual->m_wFreqInMHz ;
//          c_rev_iter_stdsetvoltageandfreqNearestLowerEqual->m_wFreqInMHz ;
//      //if( mp_model->m_pstates.m_arp_pstate[1] &&
//      //  mp_model->m_pstates.m_arp_pstate[1]->GetFreqInMHz() <
//      //  wFreqInMHzToGetVoltageFrom
//      //  )
//      //{
//        float fVoltageInVoltFromNearFreqAboveWantedFreq ;
//        float fVoltageInVoltFromNearFreqBelowWantedFreq ;
//        fVoltageInVoltFromNearFreqAboveWantedFreq =
//          //mp_model->m_pstates.m_arp_pstate[0]->GetVoltageInVolt() ;
//          //p_pstateGreaterEqual->GetVoltageInVolt() ;
//          ci_stdsetvoltageandfreqNearestHigher->
//            m_fVoltageInVolt ;
//        fVoltageInVoltFromNearFreqBelowWantedFreq =
//          //mp_model->m_pstates.m_arp_pstate[1]->GetVoltageInVolt() ;
//          //p_pstateLowerEqual->GetVoltageInVolt() ;
//          //ci_stdsetvoltageandfreqNearestLowerEqual->
//          c_rev_iter_stdsetvoltageandfreqNearestLowerEqual->
//            m_fVoltageInVolt ;
//        //example: 1.0625 V - 0.85V = 0,2125 V
//        float fVoltageFromFreqAboveAndBelowDiff = 
//          fVoltageInVoltFromNearFreqAboveWantedFreq -
//          fVoltageInVoltFromNearFreqBelowWantedFreq ;
//        WORD wFreqInMHzFromNearFreqsWantedFreqDiff =
//          wFreqInMHzFromNearFreqAboveWantedFreq -
//         wFreqInMHzFromNearFreqBelowWantedFreq ;
//        float fIncrease = 
//          fVoltageFromFreqAboveAndBelowDiff /
//            wFreqInMHzFromNearFreqsWantedFreqDiff ;
//        WORD wWantedFreqMinusFreqBelow = 
//          (wFreqInMHzToGetVoltageFrom - 
//          wFreqInMHzFromNearFreqBelowWantedFreq
//           ) ;
//
//        //example: 2200 MHz / 1050 MHz ~= 2,0952380952
//        double dFreqInMHzFromNearFreqAboveDivFreqInMHzToGetVoltageFrom =
//            (double) wFreqInMHzFromNearFreqAboveWantedFreq /
//            (double) wFreqInMHzToGetVoltageFrom ;
//
//        r_fVoltageInVolt =
//          fVoltageInVoltFromNearFreqBelowWantedFreq +
//          fIncrease * (float) wWantedFreqMinusFreqBelow
//          ;
//        return true ;
//      }
//    }
//    else
//      return ci_stdsetvoltageandfreqNearestLowerEqual->m_fVoltageInVolt ;
//  }
//  return false ;
//}

WORD I_CPUcontroller::GetNearestHigherPossibleFreqInMHz(WORD wFreqInMhzOld)
{
  std::set<VoltageAndFreq>::const_iterator iter = 
    mp_model->m_cpucoredata.m_stdsetvoltageandfreqDefault.begin( ) ;
    //mp_model->m_cpucoredata.mp_stdsetvoltageandfreqDefault->begin( ) ;
  while( iter != 
    mp_model->m_cpucoredata.m_stdsetvoltageandfreqDefault.
    //mp_model->m_cpucoredata.mp_stdsetvoltageandfreqDefault->
    end() 
    )
  {
    if( iter->m_wFreqInMHz > wFreqInMhzOld )
    {
      return iter->m_wFreqInMHz ;
      //break ;
    }
    ++ iter ;
  }
  return 0 ;
}

WORD I_CPUcontroller::GetNearestLowerPossibleFreqInMHz(WORD wFreqInMhzOld)
{
  std::set<VoltageAndFreq>::const_iterator iter = 
    mp_model->m_cpucoredata.m_stdsetvoltageandfreqDefault.begin( ) ;
    //mp_model->m_cpucoredata.mp_stdsetvoltageandfreqDefault->begin( ) ;
  std::set<VoltageAndFreq>::const_iterator iterLower = 
    mp_model->m_cpucoredata.m_stdsetvoltageandfreqDefault.end( ) ;
    //mp_model->m_cpucoredata.mp_stdsetvoltageandfreqDefault->end( ) ;
  while( iter != mp_model->m_cpucoredata.m_stdsetvoltageandfreqDefault.
    //mp_model->m_cpucoredata.mp_stdsetvoltageandfreqDefault->
    end() 
    )
  {
    if( iter->m_wFreqInMHz >= wFreqInMhzOld )
    {
      break ;
    }
    else
      iterLower = iter ;
    ++ iter ;
  }
  if( iterLower != mp_model->m_cpucoredata.m_stdsetvoltageandfreqDefault.
    //mp_model->m_cpucoredata.mp_stdsetvoltageandfreqDefault->
    end() 
    )
    return iterLower->m_wFreqInMHz ;
  return 0 ;
}

WORD I_CPUcontroller::GetNumberOfCPUcores()
{
  if( mp_cpuaccess )
  {
    DWORD dwEAX ;
    DWORD dwEBX ;
    DWORD dwECX ;
    DWORD dwEDX ;
    //Check if register 8000_0008h is available
    if( mp_cpuaccess->CpuidEx( 
        0x80000000
        , & dwEAX,
        & dwEBX,
        & dwECX,
        & dwEDX,
        1 // CPU (core) affinityMask
        )
      )
    {
      if( //http://www.sandpile.org/ia32/cpuid.htm:
        //"EAX=xxxx_xxxxh maximum supported extended level"
        dwEAX >= 0x80000008
        )
      {
        //http://www.sandpile.org/ia32/cpuid.htm:
        //extended level 8000_0008h
        //ECX: 7...0 cores per die - 1 
        if( mp_cpuaccess->CpuidEx( 
            0x80000008
            , & dwEAX,
            & dwEBX,
            & dwECX,
            & dwEDX,
            1 // CPU (core) affinityMask
            )
          )
          return (dwECX && 127 ) + 1 ; //127=1111111bin
      }
      //e.g. Pentium Ms maximum supported extended level is
      //0x80000004, so it does not have register 8000_0008h
      else
        return 1 ;
    }
    else
      return 1 ;
  }
  return 0 ;
}

BYTE I_CPUcontroller::GetPstateSafefy(
  WORD wFreqInMHz
  , float fVoltageInVolt 
  )
{
  bool bPstateIsSafe = false ;
  float fMaxVoltageInVolt ;
  if( GetInterpolatedVoltageFromFreq(
      wFreqInMHz
      , fMaxVoltageInVolt 
      , mp_model->m_cpucoredata.m_stdsetvoltageandfreqDefault
      //, * mp_model->m_cpucoredata.mp_stdsetvoltageandfreqDefault
      )
    )
  {
    int i = 0 ;
    if( fVoltageInVolt <= fMaxVoltageInVolt )
      bPstateIsSafe = //true ;
        SETTING_VOLTAGE_IS_SAFE ;
  }
  return bPstateIsSafe ;
}

//Any WRITE operation to the performance control register by any other software 
//(OS, BIOS, RMclock, GNOME power manager,...) is problematic.
BYTE I_CPUcontroller::OtherPerfCtrlMSRwriteIsActive()
{
  if( mp_dynfreqscalingaccess )
    return mp_dynfreqscalingaccess->OtherDVFSisEnabled() ;
  return 0 ;
}

void I_CPUcontroller::SetCmdLineArgs(
  int argc, 
  //TCHAR * argv[]
#ifdef FORCE_WCHAR_T
  wchar_t ** argv
#else
  TCHAR ** argv
#endif
  )
{
  m_byNumberOfCmdLineArgs = argc ;
  m_arartcharCmdLineArg = argv ;
}

void I_CPUcontroller::SetUserInterface( 
  //By using a pointer instead of a reference one can pass NULL 
  //to show that there is no object.
  UserInterface * p_userinterface)
{
  mp_userinterface = p_userinterface ;
}

void I_CPUcontroller::SetModelData( 
  //By using a pointer instead of a reference one can pass NULL 
  //to show that there is no object.
  Model * p_model )
{
  mp_model = p_model ;
  p_model->SetNumberOfCPUCores( GetNumberOfCPUcores() ) ;
}

void I_CPUcontroller::SetCPUaccess(I_CPUaccess * p_cpuaccess)
{
  mp_cpuaccess = p_cpuaccess ;
}

void I_CPUcontroller::SetCalculationThread(
  ICalculationThread * p_calculationthread )
{
  mp_calculationthread = p_calculationthread ;
}

void I_CPUcontroller::SetOtherDVFSaccess(
  IDynFreqScalingAccess * p_dynfreqscalingaccess)
{
  mp_dynfreqscalingaccess = p_dynfreqscalingaccess ;
}

bool I_CPUcontroller::CmdLineParamsContain(
  TCHAR * ptcharOption
  , std::string & strValue
  )
{
  bool bcmdLineParamsContain = false ;
  int nIndex = 1 ;
  signed short wPos = 0 ;
  DEBUG("cmdLineParamsContain begin\n");
  for ( ;nIndex < m_byNumberOfCmdLineArgs ; ++ nIndex )
  {
    std::string strCmdArg( GetCharPointer( m_arartcharCmdLineArg[ nIndex ] ) );
    wPos = (WORD) strCmdArg.find( std::string( GetCharPointer( 
      ptcharOption ) ) +
      //TCHAR("=")
      std::string("=") ) ;
    if( wPos != std::string::npos && wPos == 0 )
    {
//#ifdef WIN32
#ifdef MS_COMPILER
      strValue = strCmdArg.substr(_tcslen(ptcharOption)
        //Start after "="
        +1);
#else
      strValue = strCmdArg.substr( strlen( GetCharPointer( ptcharOption ) )
        //Start after "="
        + 1 );
#endif //#ifdef WIN32
      bcmdLineParamsContain = true ;
      break ;
    }
  }
  DEBUG("cmdLineParamsContain end\n");
  return bcmdLineParamsContain ;
}

//this method may be overwritten for CPU-specific configuration
BYTE I_CPUcontroller::HandleCmdLineArgs()
{
  std::string strValue ;
  if( CmdLineParamsContain(_T("-config"),strValue) )
  {
#ifdef COMPILE_WITH_XERCES
    SAX2MainConfigHandler saxhandler(//pstates
      *mp_model ,
      mp_userinterface ,
      this
      );
    if( 
      readXMLConfig(//"config.xml"

      //mp_configurationHandler->LoadConfiguration(

      //strValue.c_str(),pstates) )
      //strValue.c_str(),pstatectrl.m_pstates) )
      strValue.c_str(), //m_pstates
      //m_model,
      *mp_model,
      mp_userinterface ,
      //this
      saxhandler
      )
      )
    {
      //if( pstates.AreSafe() )
      //{
      //  ChangePStates();
      //}
      //else
      //  printf("Unsafe values/voltage for p-state->not applying values\");
      //pstatectrl.ApplyAllPStates(pstates);
      //pstatectrl.ApplyAllPStates();

      //If the file is NOT assigned/ opened yet.
      //if( fileDebug == NULL )
      if( //An empty string means: do NOT write to the log file.
          ! mp_model->m_stdstrLogFilePath.empty()
          &&
          ! //g_logger.m_ofstream.is_open() 
          g_logger.IsOpen() 
        )
      {
        //Convert std::string to wstring or remain std::string.
        std::tstring stdtstr = Getstdtstring(mp_model->m_stdstrLogFilePath) ;
          //g_logger = new Logger(mp_model->m_stdstrLogFilePath);
          g_logger.OpenFile( stdtstr ) ;
      }
#ifdef _EMULATE_TURION_X2_ULTRA_ZM82
      byReturn = SUCCESS ;
#else
    //if( //! mp_pstatectrl->m_model.m_bSkipCPUtypeCheck && 
    //  ! mp_model->m_bSkipCPUtypeCheck && 
    //  //! IsSupportedCPUModel() 
    //  )
    //{
    //  mp_userinterface->Confirm("This CPU model is not supported by this program."
    //    "Use \"skip_cpu_type_check\" (see help) if you think it makes "
    //    "sense.\n->exiting");
    //  return byReturn ;
    //}

      //byReturn = ApplyAllPStates();

#endif //#ifdef _EMULATE_TURION_X2_ULTRA_ZM82
    }
    else
      mp_userinterface->Confirm("Error reading the XML-configuration file");
#else //COMPILE_WITH_XERCES
    //byReturn = SUCCESS ;
#endif //COMPILE_WITH_XERCES
  }
  return 1 ;
}

//If inline: linker error (maybe this method is NOT available
//in derived classes then).
//inline 
bool // TRUE: success, FALSE: failure
 I_CPUcontroller::RdmsrEx(
  DWORD dwIndex,		// MSR index
  DWORD & dwLowmostBits,			// bit  0-31 (register "EAX")
  DWORD & dwHighmostBits,			// bit 32-63 (register "EDX")
  DWORD_PTR dwAffinityMask	// Thread Affinity Mask
  )
{
  if( mp_cpuaccess )
    return mp_cpuaccess->RdmsrEx(
      dwIndex,//&dwEAX
      & dwLowmostBits,//&dwEDX
      & dwHighmostBits,
      dwAffinityMask
      );
  return false ;
}
