//PreCompHeaders -> faster build
#include <wx/wxprec.h>
//This file needed to have the file extension ".cpp", else there where
//errors in Visual Studio for the include of <string>.
#include "CalculationThreadProc.h"
#include <Controller/ICPUcoreUsageGetter.hpp> //class ICPUcoreUsageGetter
#include <Controller/I_CPUcontroller.hpp> //class 
#include <ModelData/ModelData.hpp> //class Model
#include "UserInterface.hpp"
#include <stdlib.h> //rand()
//wxWidgets does not provide the needed "set CPU affinity". So I must
//distinguish.
#ifdef _WINDOWS
  #include <Windows/CalculationThread.hpp>
#else 
  #ifdef _LINUX
  #include <Linux/CalculationThread.hpp>
  #endif
#endif
#include <Windows_compatible_typedefs.h>

DWORD 
#ifdef _MSC_VER
  //WINAPI ("stdcall") is needed for Windows API's "::CreateThread(...)"
  WINAPI 
#endif //#ifdef _MSC_VER
 CalculationThreadProc(LPVOID lpParameter)
{
  //Windows_API::
  CalculationThread * pcalculationthread = (CalculationThread*) lpParameter ;
  pcalculationthread->m_vbContinue = true ;
  double d = 5.0 ;
  int i = 1 ;
  //PumaStateCtrl * p_pumastatectrl = (PumaStateCtrl * ) lpParameter ;
  if( pcalculationthread )
  //if( p_pumastatectrl )
    while(//1
      pcalculationthread->m_vbContinue)
    {
      if( 
        //float op.
        //5.0/1.0 != 5.0 
        d/-1.0 != -d 
        //integer op.
        //|| 1-3 != -2 || 
        || i-3 != -2 || 
        //bit move op.
        //1 << 4 != 16 
        i << 4 != 16 
        )
      {
        DEBUG("calc. error (surely because of undervolting) occured.\n");
        //sync access to 
        //CSingleLock(&pcalculationthread->m_ccritsection,TRUE);

        //sync. access so that not from 2 or more threads is written 
        //simultaneously to MSR.
        //::EnterCriticalSection(
        //  //LPCRITICAL_SECTION lpCriticalSection
        //  &pcalculationthread->m_criticalsection
        //  //&p_pumastatectrl->m_criticalsectionMSRaccess
        //  );
        //pcalculationthread->m_bCalcError = true ;
        //Better set the voltage hogh from THIS thread because
        //on the other thread a user input may be there at the same
        //time but it is better to set the voltage high immediately to 
        //prevent a computer freeze.
        //mp_controller->
        //p_pumastatectrl->IncreaseVoltageForCurrentPstate();
        //p_pumastatectrl->GetUserInterface()->Confirm(
        //pcalculationthread->mp_pumastatectrl->IncreaseVoltageForCurrentPstate(
        pcalculationthread->mp_cpucontroller->IncreaseVoltageForCurrentPstate(
          pcalculationthread->m_byCoreID );
        //pcalculationthread->mp_pumastatectrl->GetUserInterface()->Confirm(
        pcalculationthread->mp_cpucontroller->GetUserInterface()->Confirm(
          "calc. error (surely because of undervolting) occured\n"
          "Set the voltage higher to stabilize the machine");
        DEBUG("Set the voltage higher to stabilize the machine\n");
        //::LeaveCriticalSection(
        //  &pcalculationthread->m_criticalsection
        //  //&p_pumastatectrl->m_criticalsectionMSRaccess
        //  );
      }
    }
    LOG("Ended high load thread\n");
    return 0;
}

// function to check whether a given number is prime or not
bool isPrime(//a prime is a natural number-> unsigned value
             unsigned long dwNumber )
{
	unsigned long  i;

	for (i = 2 ; i < dwNumber/2 ; i++) // divide the number with all the numbers less than its half
	{
		if (dwNumber%i == 0) // number divided
			return false;
	}

	return true;
}

#ifdef _WINDOWS
long GetPrime95ResultsFileLength()
{
  //DWORD dw ;
  FILE * p_filePrime95Results = fopen("C:\\Programme\\Prime95\\results.txt","rb") ;

  if( p_filePrime95Results )
  {
    // set the file pointer to end of file
    fseek( p_filePrime95Results, 0, SEEK_END );

    // get the file size
    long Size = ftell( p_filePrime95Results );

    // close stream and release buffer
    fclose( p_filePrime95Results );
    return Size ;
  }
    HANDLE hFile = ::CreateFile( //CreateFile takes wide char or char.
      _T("C:\\Programme\\Prime95\\results.txt") ,
      GENERIC_READ | GENERIC_READ, 0, 0, OPEN_EXISTING, 0, 0 );

    if (hFile != INVALID_HANDLE_VALUE) 
    { 
        DWORD dwFileSizeLow = 0; 
        DWORD dw ;
        if ( (dwFileSizeLow = GetFileSize(hFile,&dw) ) != 0xFFFFFFFF ) 
        { 
            //printf("%i\n",dwSize);
            return dwFileSizeLow ;
        } 
    }
  return -1 ;
}
#endif //#ifdef _WINDOWS

DWORD 
#ifdef _MSC_VER
  //WINAPI ("stdcall") is needed for Windows API's "::CreateThread(...)"
  WINAPI 
#endif //#ifdef _MSC_VER
  FindDifferentPstatesThreadProc(LPVOID lpParameter)
{
  CalculationThread * pcalculationthread = (CalculationThread*) lpParameter ;
  if( pcalculationthread )
  {
    std::pair <std::set<VoltageAndFreq>::iterator, bool> stdpairstdsetvoltageandfreq ;
    std::pair <std::map<WORD,BYTE>::iterator, bool> stdpairstdmapword2byte;
    std::map<WORD,BYTE>::iterator stdpairstdmapword2byte_const_iter ;
    bool bNewVoltageAndFreqPair ;
    BYTE byTimes = 15 ;
    pcalculationthread->m_vbContinue = true ;
    std::map<WORD,BYTE> mapFreq2Count ;
    WORD wSleepTimeInMs = 1 ;
    WORD wFreqInMHz , wPrevInsertedFreqInMHz;
    float fVoltageInVolt , fLoad ;
    float * pfCPUcoreLoadInPercent = pcalculationthread->
      mp_cpucontroller->mp_model->m_cpucoredata.m_arfCPUcoreLoadInPercent ;
  #ifdef _DEBUG
    float fCPUusageReferringToMaxFreq , 
      fPreviousCPUusageReferringToMaxFreq = 1.0 ;
    //pcalculationthread->mp_cpucontroller->
    WORD wMaxFreqInMHz = 1800 ;
    wPrevInsertedFreqInMHz = wMaxFreqInMHz + 1 ;
  #endif
    //while( wSleepTimeInMs < 200 )
    do
    {
      DWORD dwNum = 10000000 ;
      //Try to get a specific (average) CPU load by a full load for 
      //some nanoseconds 
      //and sleep afterwards.
      while( dwNum -- )
      {

      }
      //if( pcalculationthread->mp_cpucontroller->
       if( pcalculationthread->mp_cpucontroller->GetCurrentPstate(
         wFreqInMHz, fVoltageInVolt, pcalculationthread->m_byCoreID ) )
       {
         //mp_cpucontroller->
         //fLoad = ::wxGetApp().mp_cpucoreusagegetter->
           pcalculationthread->mp_cpucontroller->mp_icpucoreusagegetter->
           //GetPercentalUsageForCore(
           //pcalculationthread->m_byCoreID
           GetPercentalUsageForAllCores(
             pfCPUcoreLoadInPercent) ;
         fLoad = pfCPUcoreLoadInPercent[ pcalculationthread->m_byCoreID ] ;
          fCPUusageReferringToMaxFreq = fLoad * (float) wFreqInMHz / (float) wMaxFreqInMHz ;
#ifdef _DEBUG
          fCPUusageReferringToMaxFreq = fCPUusageReferringToMaxFreq ;
#endif
         if( //fCPUusageReferringToMaxFreq < fPreviousCPUusageReferringToMaxFreq 
           //)
           //fPreviousCPUusageReferringToMaxFreq - fCPUusageReferringToMaxFreq < 0.02
         //if( 
           //&&
           wFreqInMHz >= wPrevInsertedFreqInMHz 
           && ! (-- byTimes)
           )
         {
           wSleepTimeInMs ++ ;
           byTimes = 15 ;
         }
         if( //fPreviousCPUusageReferringToMaxFreq - fCPUusageReferringToMaxFreq < 0.1
           //&& 
           wFreqInMHz < wPrevInsertedFreqInMHz )
         {
           stdpairstdmapword2byte_const_iter = mapFreq2Count.find( wFreqInMHz ) ;
           if( stdpairstdmapword2byte_const_iter != mapFreq2Count.end() )
           {
             ++ stdpairstdmapword2byte_const_iter->second ;
             if( stdpairstdmapword2byte_const_iter->second > 5 ) 
             {
               wPrevInsertedFreqInMHz = wFreqInMHz ;
              fPreviousCPUusageReferringToMaxFreq = fCPUusageReferringToMaxFreq ;
             }
           }
           else
            //stdpairstdmapword2byte = 
            mapFreq2Count.insert( std::pair<WORD,BYTE>(wFreqInMHz,1) ) ;
           //if( stdpairstdmapword2byte 
           //wPrevInsertedFreqInMHz = wFreqInMHz ;
           //fPreviousCPUusageReferringToMaxFreq = fCPUusageReferringToMaxFreq ;
         }
         //stdpairstdsetvoltageandfreq = pcalculationthread->
         //  mp_cpucontroller->mp_model->m_cpucoredata.
         //   m_stdsetvoltageandfreqDefault.insert( 
         //   VoltageAndFreq ( fVoltageInVolt , wFreqInMHz ) 
         //   ) ;
          bNewVoltageAndFreqPair = pcalculationthread->
            mp_cpucontroller->mp_model->m_cpucoredata.AddDefaultVoltageForFreq(
            fVoltageInVolt , wFreqInMHz ) ;
          //New p-state inserted.
          if( //stdpairstdsetvoltageandfreq.second 
            bNewVoltageAndFreqPair )
          {
          //  wPrevInsertedFreqInMHz = wFreqInMHz ;
          //  //bNewVoltageAndFreqPair = true ;
          // fPreviousCPUusageReferringToMaxFreq = fCPUusageReferringToMaxFreq ;
            pcalculationthread->mp_userinterface->RedrawEverything() ;
          }
         Sleep( wSleepTimeInMs ) ;
       //  if( fLoad * fCPUusageReferringToMaxFreq 
       }
    }
    while( fCPUusageReferringToMaxFreq > 0.25 ) ;
  }
  return 0 ;
}

DWORD 
#ifdef _MSC_VER
  //WINAPI ("stdcall") is needed for Windows API's "::CreateThread(...)"
  WINAPI 
#endif //#ifdef _MSC_VER
  FPUcalculationThreadProc(LPVOID lpParameter)
{
  //Windows_API::
  CalculationThread * pcalculationthread = (CalculationThread*) lpParameter ;
  pcalculationthread->m_vbContinue = true ;
  BYTE arbyAllBitsSet[sizeof(double)] ;
  memset(arbyAllBitsSet,255,sizeof(double) ) ;
  //a double is 8 bytes big
  double d = 5.0 ;
  double dAllBitsSet ;
  //memcpy( dAllBitsSet, arbyAllBitsSet sizeof(double) ) ;
  memset( & dAllBitsSet,255,sizeof(double) ) ;
  double dExpectedValue = 1.0/3.0 ;
  int i = 1 ;
  //Use the SAME data type for comparing 2 values. Else because of precision
  //they can differ even if not instable voltage.
  //double fExpectedValue ;
  //WORD wNumbers = //1000 ;
  //  //Use at least as many bytes as the L2 cache size.
  //  2000000 ;
  DWORD dwNumbers = 
    //Use at least as many bytes as the L2 cache size.
    35000 ;
    //8192 ;

  //DWORD * ardwNumber = new DWORD[1000] ;
  double * arfNumberPrecalculated = new double[
    dwNumbers ] ;
  double * ardNumberPrecalculated = new double[
    dwNumbers ] ;
  BYTE * byData = new BYTE[
    dwNumbers ] ;
  if( //ardwNumber 
    //arfNumberPrecalculated
    ardNumberPrecalculated
    && byData
    )
  {
    DWORD dwNumberToTest = //ULONG_MAX; 
      1 ;
    DWORD dwIndex = 0 ;
    double dSum = 0.0 ;
    for ( ; dwIndex < dwNumbers ; ++ dwIndex )
    {
      ////Idea: the value should depend on all of its predecessors, so
      ////use a sum.
      dSum += ( 1.0 / 
        (double)
        //To let the 1st number be "1".
        //++ wIndex ;
        ( dwIndex + 1 ) 
        ) ;
      arfNumberPrecalculated[dwIndex] = 
        //if( isPrime( dwNumberToTest ) )
        //  ardwNumber[wIndex ++ ] = dwNumberToTest ;
        //1.0 / //(float) 
        //(double)
        ////To let the 1st number be "1".
        ////++ wIndex ;
        //( dwIndex + 1 ) ;
        dSum ;
      ardNumberPrecalculated[dwIndex] = 
        ////arfNumberPrecalculated[dwIndex] ;
        //( 1.0 / 
        //(double)
        ////To let the 1st number be "1".
        ////++ wIndex ;
        //( dwIndex + 1 ) 
        //) ;
        pow( 2.0, (double) dwIndex) ;

      byData[dwIndex] = 255 ;
    }
    dwIndex = 0 ;
  DWORD dwNumberCountMinus1 = dwNumbers - 1 ;
  dSum = 0.0 ;
  //PumaStateCtrl * p_pumastatectrl = (PumaStateCtrl * ) lpParameter ;
  long lPreviousPrime95ResultsFileLength = GetPrime95ResultsFileLength() ;
  srand(dwNumberCountMinus1) ;
  BYTE byExpVal = 255 ;
  if( pcalculationthread )
  //if( p_pumastatectrl )
    while(//1
      pcalculationthread->m_vbContinue)
    {
      //dwIndex = rand() % 100000 ;
      //dExpectedValue = 1.0 / //d ;
      //fExpectedValue = 1.0 / //d ;
      //  //(float) 
      //  (double) (dwIndex + 1);
      //dSum //+= 
      //  //( 1.0 / 
      //  //(double)
      //  ////To let the 1st number be "1".
      //  ////++ wIndex ;
      //  //( dwIndex + 1 ) 
      //  //) ;
      //  //+= ardNumberPrecalculated[dwIndex] ;
      
      //Zugriffsverletzung an DIESER Stelle, als mit MSVC im Debugger bei 800MHz und 0.7 Volt
      //im Debugger: arfNumberPrecalculated = 1.00000 , dwIndex = 1???
        arfNumberPrecalculated[dwIndex] 
        = pow( 2.0, (double) dwIndex) ;
      //ardNumberPrecalculated[dwIndex] = dSum ;
      //long lPrime95ResultsFileLength = GetPrime95ResultsFileLength() ;
      //byData[dwIndex] = 255 ;
      if( 
        //float op.
        //5.0/1.0 != 5.0 
        //d/-1.0 != -d 
        //d/ (2.0*d) != 0.5 
        //( d / ( 3.0 * d ) ) != dExpectedValue
        //1.0  / d  != dExpectedValue
        //dAllBitsSet / dAllBitsSet != 1.0
        //integer op.
        //|| 1-3 != -2 || 
        //|| i-3 != -2 || 
        //bit move op.
        //1 << 4 != 16 
        //i << 4 != 16 
        //TODO
          //! isPrime(//large number that is KNOWN to be a prime.
          //  ardwNumber[ wIndex ++ ] 
          //       )
        //lPrime95ResultsFileLength > lPreviousPrime95ResultsFileLength
        //arfNumberPrecalculated[dwIndex] //!= //dExpectedValue
        /// //fExpectedValue 
        //ardNumberPrecalculated[dwIndex] != 1
        //|| 
        arfNumberPrecalculated[dwIndex]
        //dSum != 
        != ardNumberPrecalculated[dwIndex] 
        //|| byData[dwIndex] != //255
        //  byExpVal
        )
      {
        DEBUG("calc. error (surely because of undervolting) occured.\n");
        //sync access to 
        //CSingleLock(&pcalculationthread->m_ccritsection,TRUE);

        //sync. access so that not from 2 or more threads is written 
        //simultaneously to MSR.
        //::EnterCriticalSection(
        //  //LPCRITICAL_SECTION lpCriticalSection
        //  &pcalculationthread->m_criticalsection
        //  //&p_pumastatectrl->m_criticalsectionMSRaccess
        //  );
        //pcalculationthread->m_bCalcError = true ;
        //Better set the voltage hogh from THIS thread because
        //on the other thread a user input may be there at the same
        //time but it is better to set the voltage high immediately to 
        //prevent a computer freeze.
        //mp_controller->
        //p_pumastatectrl->IncreaseVoltageForCurrentPstate();
        //p_pumastatectrl->GetUserInterface()->Confirm(
        //pcalculationthread->mp_pumastatectrl->IncreaseVoltageForCurrentPstate(
        pcalculationthread->mp_cpucontroller->IncreaseVoltageForCurrentPstate(
          pcalculationthread->m_byCoreID);
        //pcalculationthread->mp_pumastatectrl->GetUserInterface()->Confirm(
        pcalculationthread->mp_cpucontroller->GetUserInterface()->Confirm(
          "calc. error (surely because of undervolting) occured\n"
          "Set the voltage higher to stabilize the machine");
        DEBUG("Set the voltage higher to stabilize the machine\n");
        //::LeaveCriticalSection(
        //  &pcalculationthread->m_criticalsection
        //  //&p_pumastatectrl->m_criticalsectionMSRaccess
        //  );
        //lPreviousPrime95ResultsFileLength = lPrime95ResultsFileLength ;
      }
      //d *= -1.0 ;
      //d /= -5.0 ;
      //d += 1.0 ;
      if( dwIndex == dwNumberCountMinus1 )
      {
        dwIndex = 0 ;
        dSum = 0.0 ;
        byExpVal = ~byExpVal ;
      }
      else
        ++ dwIndex ;
      //byData[dwIndex] = //255 ;
      //  byExpVal ;
      //Sleep(500) ;
    }
    LOG("Ended high load thread\n");
  }
  delete [] ardNumberPrecalculated ;
  delete [] arfNumberPrecalculated ;
  delete [] byData ;
  return 0;
}

DWORD 
#ifdef _MSC_VER
  //WINAPI ("stdcall") is needed for Windows API's "::CreateThread(...)"
  WINAPI 
#endif //#ifdef _MSC_VER
HighALUloadThreadProc(LPVOID lpParameter)
{
  CalculationThread * pcalculationthread = (CalculationThread*) lpParameter ;
  pcalculationthread->m_vbContinue = true ;
  double d = 5.0 ;
  int i = 1 ;
  DWORD dwAllBitsSet = 
    //For stability check: use a number where all bits are set. So every 
    //transistor storing a bit needs voltage.
    (DWORD) -1 ;
  //PumaStateCtrl * p_pumastatectrl = (PumaStateCtrl * ) lpParameter ;
  if( pcalculationthread )
  //if( p_pumastatectrl )
    while(//1
      pcalculationthread->m_vbContinue )
    {
      if(
        //integer op.
        //1-3 != -2
        //i << 4 != 16 
        dwAllBitsSet / 1 != dwAllBitsSet
        )
      {
        DEBUG("calc. error (surely because of undervolting) occured.\n");
        //sync access to
        //CSingleLock(&pcalculationthread->m_ccritsection,TRUE);

        //sync. access so that not from 2 or more threads is written
        //simultaneously to MSR.
        //::EnterCriticalSection(
        //  //LPCRITICAL_SECTION lpCriticalSection
        //  &pcalculationthread->m_criticalsection
        //  //&p_pumastatectrl->m_criticalsectionMSRaccess
        //  );
        //pcalculationthread->m_bCalcError = true ;
        //Better set the voltage hogh from THIS thread because
        //on the other thread a user input may be there at the same
        //time but it is better to set the voltage high immediately to
        //prevent a computer freeze.
        //mp_controller->
        //p_pumastatectrl->IncreaseVoltageForCurrentPstate();
        //p_pumastatectrl->GetUserInterface()->Confirm(
        //pcalculationthread->mp_pumastatectrl->IncreaseVoltageForCurrentPstate(
        pcalculationthread->mp_cpucontroller->IncreaseVoltageForCurrentPstate(
          pcalculationthread->m_byCoreID );        
        //pcalculationthread->mp_pumastatectrl->GetUserInterface()->Confirm(
        pcalculationthread->mp_cpucontroller->GetUserInterface()->Confirm(
          "calc. error (surely because of undervolting) occured\n"
          "Set the voltage higher to stabilize the machine");
        DEBUG("Set the voltage higher to stabilize the machine\n");
        //::LeaveCriticalSection(
        //  &pcalculationthread->m_criticalsection
        //  //&p_pumastatectrl->m_criticalsectionMSRaccess
        //  );
      }
      -- dwAllBitsSet ;
      dwAllBitsSet = ~ dwAllBitsSet ;
    }
    LOG("Ended high load thread\n");
    return 0;
}
