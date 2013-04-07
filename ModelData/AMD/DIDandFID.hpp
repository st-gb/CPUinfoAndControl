/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
#pragma once //include guard

#include <global.h> //for BYTE, WORD

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

  float GetMultiplier()
  {
    return ( float) ( m_byFreqID + 8 ) / (float)( 1 << m_byDivisorID ) ;
  }
};
