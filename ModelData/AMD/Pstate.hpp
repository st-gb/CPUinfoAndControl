/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
#pragma once //include guard
#include <global.h> //for BYTE, WORD
#ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
  #include <windef.h> //for BYTE
#endif
//#include "DIDandFID.hpp" //for class DIDandFID

//pre-declare.
class DIDandFID ;

class PState
{
//private:
public:
  BYTE m_byFreqID ;
  BYTE m_byIsSafe ;
  BYTE m_byNumber ;
  BYTE m_byVoltageID ;
  BYTE m_byDivisorID ;
  float m_fMaxVoltageInVolt ;
//public:
  PState() ;
  PState(BYTE byVID, BYTE byDivID=255, BYTE byFreqID=255, BYTE byNumber=255) ;
  PState( float fVoltageInVolt, WORD wFreqInMHz ) ;

  void AssignChangesFromPstateFromMSR(PState & r_pstateFromMSR) ;

  void AssignChangesToPstateFromMSR(PState & r_pstateFromMSR);

  BYTE GetDivisorID() ;
  static double GetExpectedPowerDissipation(DWORD dwHigh) ;
    
  static float GetVoltageInVolt(BYTE byVoltageID) ;
  float GetVoltageInVolt() ;
  BYTE GetVoltageID() ;
  static BYTE GetVoltageID(float fVoltageInVolt) ;
  WORD GetFreqInMHz() ;
  static WORD GetFreqInMHz(BYTE byFreqID, BYTE byDivisorID) ;

  bool IsSafeInCombinationTo(PState & r_pstateFromMSR);

  void SetDIDandFID(const DIDandFID & rcdidnfid) ;
  void SetDivisorID(BYTE byDivisorID) ;
  void SetFrequencyID(BYTE byFrequencyID ) ;
};
