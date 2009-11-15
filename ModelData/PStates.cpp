//#ifdef _WINDOWS
	//If not included: compiler error "C1010" with MSVC++.
	#include "stdafx.h"
//#endif //#ifndef _WINDOWS
#include "PStates.h"
//#include "../Controller/PumaStateCtrl.h"
#include <Controller/AMD/Griffin/GriffinController.hpp>

//WORD GetMaximumFrequency()
//{
//  
//}

//@return 0.0 = error / false input value
float GetDefaultVoltageForFrequency(WORD wFrequInMHz)
{
  float fVoltageInVolt = 0.0f;
  //TODO get Maximum Operating Frequency (=MOF; see AMD BKDG for 11h CPUs "chapter 2.4.2, MOF"
  WORD wAFourthFrequency = 550 ;
  //float fMaxFreqDivWantedFreq = wMaxFrequency / wFrequInMHz ;
  float fWantedFreqDivAFourthFreq = (float) wFrequInMHz / wAFourthFrequency;
  //The the freq is higher than a fourth of the maximal freq.
  //if( fMaxFreqDivWantedFreq <= 4.0f)
  if(fWantedFreqDivAFourthFreq >= 1.0f )
    //ex.: max. freq. 2200
    //wanted freq: 550 MHz->0,8V
    //wanted freq: 1100 MHz->0,8+0.075*2=0,95V
    //wanted freq: 2200 MHz->0,8+0.075*2=1,1V
    //fVoltageInVolt = 0.8f + fMaxFreqDivWantedFreq * 0.125f ;
    fVoltageInVolt = 0.8f + fWantedFreqDivAFourthFreq * 0.075f ;
  //return wMaxFreqDivWantedFreq ;
  return fVoltageInVolt ;
}

//Determine if voltage is not too high.
//Too high voltages may damage the CPU!
bool PStates::IsSafe(BYTE byVID, BYTE byFrequencyID, BYTE byDivisorID)
{
  bool bIsSafe = false ;
  //DEBUG("PStates::IsSafe(byVID:%u, byFrequencyID:%u, byDivisorID:%u)--begin\n",
  //  byVID,byFrequencyID,byDivisorID);
  LOG("Setting p-state is safe?: Voltage ID:" << 
      //Convert to unsigned integer, else it is output as character.
      (WORD) byVID 
      << ", Frequency ID:" << 
      //Convert to unsigned integer, else it is output as character.
      (WORD) byFrequencyID 
      << " Divisor ID:" << 
      //Convert to unsigned integer, else it is output as character.
      (WORD) byDivisorID 
      << ")--begin\n" );
  //Safe states are: 
  //-550MHz, voltage<=0.8V, 550/0,8=687,5
  //-1100MHz, voltage<=0.95V, 1157,89 MHz/V
  //-2200MHz, voltage<=1.1V, 2000 MHz/V
  //f(frequency)=frequency/687,5+ frequency/550*0,15=max_voltage ??
  //:f(550)=550/687,5
  WORD wFrequInMHz = GriffinController::getFrequencyInMHz(byFrequencyID,byDivisorID);
  //DEBUG("Resulting freq in MHz is:%u, VID is:%u\n",wFrequInMHz,(WORD)byVID);
  LOG("Resulting frequency in MHz is:" << wFrequInMHz << ", Voltage ID is:" << 
      //Convert to unsigned integer, else it is output as character.
      (WORD) byVID << "\n" );
  //switch(wFrequInMHz)
  //{
  //case 550:
  //  //VID=60: 0,8V (default)
  //  //VID=63: 0,7625V
  //  if(byVID>59)
  //    bIsSafe = true ;
  //  break;
  //case 1100:
  //  //VID=48: 0,95V (default)
  //  //VID=60: 0,8V
  //  if(byVID>47)
  //    bIsSafe = true ;
  //  break;
  //case 2200:
  //  //VID=36: 1,1V (default)
  //  //VID=40: 1.050V
  //  if(byVID>35)
  //    bIsSafe = true ;
  //  break;
  //}//switch()
  float fDefaultVoltageForWantedFreq = GetDefaultVoltageForFrequency(wFrequInMHz);
  if( 
    //The maximum voltage.
    1.55f 
    - ( (float)( byVID ) * 0.0125f ) <= fDefaultVoltageForWantedFreq )
    bIsSafe = true ;
  //DEBUG("Setting P-state is %ssafe\n", bIsSafe ? "" : "NOT " );
  LOG( "Setting p-state is " << ( bIsSafe ? "" : "NOT " ) << "safe\n");
  return bIsSafe ;
}

  void PStates::SetPStateVID(BYTE byPStateID, BYTE byVID)
  {
    //m_byNumber = byPStateID ;
    if( byVID > 64 )
#ifdef COMPILE_WITH_DEBUG
    {
      //DEBUG("Loading config--current p-state's VID was %u > 64 -> set to 64\n",
      //  (WORD) byVID);
      LOG( "Loading config--current p-state's VID was " << (WORD) byVID << 
          " > 64 -> set to 64\n" );
#endif //COMPILE_WITH_DEBUG
      //For the overvoltage protection it is important zu bercksichtigen
      //that voltages below 0.75 V are not driven by the voltage regulator.
      byVID = 64 ;
#ifdef COMPILE_WITH_DEBUG
    }
    else
      //DEBUG("Loading config--current p-state's VID: %u\n", (WORD) byVID);
      LOG( "Loading config--current p-state's VID: " << (WORD) byVID << "\n" );
#endif //COMPILE_WITH_DEBUG
    if( //The variable value is always >= 0 if it has an unsigned type.
      //byPStateID >=0 &&
      byPStateID < NUMBER_OF_PSTATES )
    {
      if( ! m_arp_pstate[byPStateID] )
        m_arp_pstate[byPStateID] = new PState();
      //If already allocated or allocating memory succeeded.
      if( m_arp_pstate[byPStateID] )
      {
        m_arp_pstate[byPStateID]->m_byVoltageID = byVID ;
        m_arp_pstate[byPStateID]->m_byNumber = byPStateID ;
      }
    }
    else
    {
      DEBUG("Loading config--p-state not within [0...7]->not assigning to RAM\n");
    }
  }

  void PStates::SetPStateDID( BYTE byPStateID, BYTE byDID)
  {
    LOG( "Setting Divisor ID to: " << 
        //Convert to integer in order to print as number.
        (WORD) byDID << "\n" )
    //m_byNumber = byPStateID ;
    if( //The variable value is always >= 0 if it has an unsigned type.
      //byPStateID >=0 &&
      byPStateID < NUMBER_OF_PSTATES )
    {
      if( ! m_arp_pstate[byPStateID] )
        m_arp_pstate[byPStateID] = new PState();
      //If already allocated or allocating memory succeeded.
      if( m_arp_pstate[byPStateID] )
      {
        m_arp_pstate[byPStateID]->m_byDivisorID = byDID ;
        m_arp_pstate[byPStateID]->m_byNumber = byPStateID ;
      }
    }
  }

  void PStates::SetPStateFID(BYTE byPStateID, BYTE byFID)
  {
    LOG( "Setting Frequency ID to: " << 
        //Convert to integer in order to print as number.
        (WORD) byFID << "\n" )
    //m_byNumber = byPStateID ;
    if( //The variable value is always >= 0 if it has an unsigned type.
      //byPStateID >=0 &&
      byPStateID < NUMBER_OF_PSTATES )
    {
      if( ! m_arp_pstate[byPStateID] )
        m_arp_pstate[byPStateID] = new PState();
      //If already allocated or allocating memory succeeded.
      if( m_arp_pstate[byPStateID] )
      {
        m_arp_pstate[byPStateID]->m_byFreqID = byFID ;
        m_arp_pstate[byPStateID]->m_byNumber = byPStateID ;
      }
    }
  }
