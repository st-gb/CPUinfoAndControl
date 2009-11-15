#pragma once //include guard

#include "../global.h" //for BYTE, WORD

class DIDandFID
{
public:
  BYTE m_byDivisorID ;
  BYTE m_byFreqID ;
  
  DIDandFID() {} ;
  DIDandFID(BYTE DID, BYTE FID)
  {
    m_byDivisorID = DID ;
    m_byFreqID = FID ;
  }
  WORD GetDiff(WORD wFreq)
  {
    WORD wFreqInMHzForDIDandFID = ( WORD) (( m_byFreqID + 8 )*100 ) 
      / (1<<m_byDivisorID) ;
    return wFreqInMHzForDIDandFID > wFreq?
      wFreqInMHzForDIDandFID - wFreq :
      wFreq - wFreqInMHzForDIDandFID ;
  }

  WORD GetFreqInMHz()
  {
    return ( WORD) (( m_byFreqID + 8 )*100 ) / ( 1 << m_byDivisorID ) ;
  }
};
