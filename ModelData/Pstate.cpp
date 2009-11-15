#include "Pstate.hpp"
#include "PStates.h" //for PStates::IsSafe(...)
#include "preprocessor_helper_macros.h" //for BITMASK...

PState::PState()
    //C++ style initialisations:
    : m_fMaxVoltageInVolt (0.0)
  {
    //255 means: not specified by configuration.
    m_byNumber=255;
    m_byFreqID=255;
    m_byVoltageID=255;
    m_byDivisorID=255;
    m_byIsSafe = true ;
  }

    PState::PState
    (
       BYTE byVID
       , BYTE byDivID//=255, 
       , BYTE byFreqID//=255, 
       , BYTE byNumber//=255
    )
  {
    //if(byVID != 255 )
      this->m_byVoltageID = byVID;
    //if(byDivID != 255 )
      this->m_byDivisorID = byDivID;
    //if(byFreqID != 255 )
      this->m_byFreqID = byFreqID;
      m_byNumber = byNumber ;
  }

void PState::AssignChangesFromPstateFromMSR(PState & r_pstateFromMSR)
{
  //Not specified by the user
  if(m_byFreqID == NOT_SPECIFIED_BY_USER)
    m_byFreqID = r_pstateFromMSR.m_byFreqID ;
  if(m_byVoltageID == NOT_SPECIFIED_BY_USER)
    m_byVoltageID = r_pstateFromMSR.m_byVoltageID ;
  if(m_byDivisorID == NOT_SPECIFIED_BY_USER)
    m_byDivisorID = r_pstateFromMSR.m_byDivisorID ;
}

void PState::AssignChangesToPstateFromMSR(PState & r_pstateFromMSR)
{
  //bool IsSafeInCombinationTo = false ;
  DEBUG("Assigning changes to p-state from MSR\n");
  if(m_byFreqID != NOT_SPECIFIED_BY_USER )
    r_pstateFromMSR.m_byFreqID = m_byFreqID ;
  if(m_byVoltageID != NOT_SPECIFIED_BY_USER )
    r_pstateFromMSR.m_byVoltageID = m_byVoltageID ;
  if(m_byDivisorID != NOT_SPECIFIED_BY_USER )
    r_pstateFromMSR.m_byDivisorID = m_byDivisorID ;
}

double PState::GetExpectedPowerDissipation(DWORD dwHigh)
{
  double dCurrent = -1.0 ;
  BYTE byIddDiv = ( dwHigh >> 
    //"41:40 IddDiv: current divisor field"
    8 ) & BITMASK_FOR_LOWMOST_2BIT ;
  BYTE byIddValue = 
    //"39:32 IddValue: current value field"
    ( dwHigh & BITMASK_FOR_LOWMOST_8BIT ) ;
  switch ( byIddDiv )
  {
  case 0:
    dCurrent = byIddValue ;
    break;
  case 1:
    dCurrent = (double) byIddValue / 10.0 ;
    break;
  case 2:
    dCurrent = (double) byIddValue / 100.0 ;
    break;
  }
  return dCurrent ;
}

  WORD PState::GetFreqInMHz()
  {
    return ( WORD) (( m_byFreqID + 8 )*100 ) / (1<<m_byDivisorID) ;
  }

  //static 
  WORD PState::GetFreqInMHz(BYTE byFreqID, BYTE byDivisorID)
  {
    return ( WORD) (( byFreqID + 8 )*100 ) / ( 1 << byDivisorID) ;
  }
  
  BYTE PState::GetDivisorID()
  {
      return m_byDivisorID ;
  }

  BYTE PState::GetVoltageID()
  {
      return m_byVoltageID ;
  }

  //static 
  BYTE PState::GetVoltageID(float fVoltageInVolt)
  {
    //voltage_in_V = 1.55 - voltageID * 0.0125 | -1.55
    //voltage_in_V - 1.55 = - voltageID * 0.0125 | : 0.0125
    //(voltage_in_V - 1.55)/0.0125 = - voltageID | * (-1)
    //-(voltage_in_V - 1.55)/0.0125 = voltage_ID
    //test: given: voltage_in_V = 1.1
    // -(-0.45 V ) /0.0125 V = voltage_ID <=> 36 = voltage_ID
    float fVoltageMinusMaxVoltage = fVoltageInVolt - 1.55f ;
    //DEBUG("voltageInVolt-1,55:%f %f VID:%f\n",fVoltageMinusMaxVoltage, 
    //  fVoltageMinusMaxVoltage * -1.0f, 
    //  (fVoltageInVolt - 1.55f) * -1.0f / 0.0125f);
    LOG( "voltageInVolt-1,55:" << fVoltageMinusMaxVoltage * -1.0f << 
        " Voltage ID:" << (fVoltageInVolt - 1.55f) * -1.0f / 0.0125f << 
        "\n" );
    return
      //without explicit cast: compiler warning
      (BYTE)
      //ceil( (fVoltageInVolt - 1.55f) * -1.0f / 0.0125f ) ;
      ceil( (fVoltageInVolt - 1.55f) / -0.0125f ) ;
  }

  //static 
  float PState::GetVoltageInVolt(BYTE byVoltageID)
  {
    PState pstate;
    pstate.m_byVoltageID = byVoltageID ;
    return pstate.GetVoltageInVolt();
  }

  float PState::GetVoltageInVolt()
  {
    return 
      //The maximum possible voltage.
      1.55f 
      - ( (float)( m_byVoltageID ) * 0.0125f ) ;
  }


bool PState::IsSafeInCombinationTo(PState & r_pstateFromMSR)
{
  //bool IsSafeInCombinationTo = false ;
  if(m_byFreqID!=255)
    r_pstateFromMSR.m_byFreqID = m_byFreqID ;
  if(m_byVoltageID!=255)
    r_pstateFromMSR.m_byVoltageID = m_byVoltageID ;
  if(m_byDivisorID!=255)
    r_pstateFromMSR.m_byDivisorID = m_byDivisorID ;
  return PStates::IsSafe(
    r_pstateFromMSR.m_byVoltageID,
    r_pstateFromMSR.m_byFreqID,
    r_pstateFromMSR.m_byDivisorID);
}

  void PState::SetDIDandFID(const DIDandFID & rcdidnfid)
  {
    m_byFreqID = rcdidnfid.m_byFreqID;
    m_byDivisorID = rcdidnfid.m_byDivisorID ;
  }

  void PState::SetDivisorID(BYTE byDivisorID)
  {
    m_byDivisorID = byDivisorID ;
  }

  void PState::SetFrequencyID(BYTE byFrequencyID )
  {
    m_byFreqID = byFrequencyID ;
  }
