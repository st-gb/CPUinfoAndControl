#pragma once //include guard

//#include "PumaStateCtrl.h"
#include <Controller/CPU-related/AMD/Griffin/AMD_family17.h>

#define NOT_SPECIFIED_BY_USER 255
#include <stdio.h> //for "NULL" constant.
#include <memory.h> //for memset(...)
#include "../global.h" //for WORD
#include <math.h> //for ceil()
#include "Pstate.hpp" //for class Pstate
#include "DIDandFID.hpp" //for class Pstate

//typedef unsigned char BYTE;
typedef unsigned char BYTE;
//extern FILE * fileDebug; //for debug logging

//pre-declare.
class DIDandFID ;

class PStates
{
  BYTE m_byNumber;
  BYTE m_byFrequencyID;
  BYTE m_byDivisorID;
  BYTE m_byVoltageID;
  WORD m_wMaxFreq ;
public:
  bool m_bConfirm ;
  //bool m_bSkipCPUtypeCheck ;
  PState * m_arp_pstate[NUMBER_OF_PSTATES] ;
  PStates()
    //C++ style inits:
    //: //m_bSkipCPUtypeCheck(false) 
    //,
    : m_bConfirm(false)
  {
    //Set all pointers in the array to NULL.
    memset( m_arp_pstate ,
      //if passing "NULL": compiler warning 
      //NULL
      0 
      , NUMBER_OF_PSTATES * sizeof(PState *) );
    //TODO make also compatible with Z-80 (max. 2000 MHz) and ZM-84 (2400MHz), etc.
    //m_wMaxFreq = 2200 ;
  }
  ~PStates()
  {
    BYTE byIndex = 0; 
    for( ; byIndex < NUMBER_OF_PSTATES ; ++ byIndex )
    {
      //Is NULL if none exits for a p-state ID.
      if( m_arp_pstate[byIndex] )
      {
        //Free memory.
        delete m_arp_pstate[byIndex] ;
      }
    }
  }

  PState * GetPstateWithNearestFreqGreaterEqual( 
    WORD wFreqInMHz
    //Limit the p-state number because in some p-state there may be shit content
    //resp. to make it available for K10 CPUs who have only 5 pstates
    , BYTE byHighestPstateNumber
    )
  {
    BYTE byIndex = 0; 
    WORD wFreqInMHzOfCurrentPstate ;
    PState * p_pstatewithnearestfreqgreaterequal = NULL ;
    for( ; byIndex < byHighestPstateNumber ; ++ byIndex )
    {
      //Is NULL if none exists for a p-state ID.
      if( m_arp_pstate[byIndex] )
      {
        wFreqInMHzOfCurrentPstate  = m_arp_pstate[byIndex]->GetFreqInMHz() ;
        if( wFreqInMHzOfCurrentPstate >= wFreqInMHz )
          if( p_pstatewithnearestfreqgreaterequal )
          {
            if( wFreqInMHzOfCurrentPstate < 
              p_pstatewithnearestfreqgreaterequal->GetFreqInMHz() 
              )
              p_pstatewithnearestfreqgreaterequal = m_arp_pstate[byIndex] ;
          }
          else
            p_pstatewithnearestfreqgreaterequal = m_arp_pstate[byIndex] ;
          //return m_arp_pstate[byIndex] ;
      }
    } 
    return //NULL ;
      p_pstatewithnearestfreqgreaterequal ;
  }

  PState * GetPstateWithNearestFreqLowerEqual( 
    WORD wFreqInMHz
    //Limit the p-state number because in some p-state there may be shit content
    //resp. to make it available for K10 CPUs who have only 5 pstates
    , BYTE byHighestPstateNumber
    )
  {
    BYTE byIndex = 0; 
    PState * p_pstatewithnearestfreqlowerequal = NULL ;
    WORD wFreqInMHzOfCurrentPstate ;
    for( ; byIndex < byHighestPstateNumber ; ++ byIndex )
    {
      //Is NULL if none exits for a p-state ID.
      if( m_arp_pstate[byIndex] )
      {
        //if( m_arp_pstate[byIndex].GetFreqInMHz() <= wFreqInMHz )
        //  return m_arp_pstate[byIndex] ;
        wFreqInMHzOfCurrentPstate  = m_arp_pstate[byIndex]->GetFreqInMHz() ;
        if( wFreqInMHzOfCurrentPstate <= wFreqInMHz )
          if( p_pstatewithnearestfreqlowerequal )
          {
            if( wFreqInMHzOfCurrentPstate > 
              p_pstatewithnearestfreqlowerequal->GetFreqInMHz() 
              )
              p_pstatewithnearestfreqlowerequal = m_arp_pstate[byIndex] ;
          }
          else
            p_pstatewithnearestfreqlowerequal = m_arp_pstate[byIndex] ;      
      }
    } 
    return p_pstatewithnearestfreqlowerequal ;
  }

  void SetNumber(BYTE byNumber) ;

    void SetPStateVID(BYTE byPStateID, BYTE byVID) ;
    void SetPStateDID( BYTE byPStateID, BYTE byDID) ;
    void SetPStateFID(BYTE byPStateID, BYTE byFID) ;

  //void SetPStateFreq(BYTE byPStateID, WORD wFreqInMHz)
  //{
  //  //m_byNumber = byPStateID ;
  //  if(wFreqInMHz <= m_wMaxFreq && 
  //    //The variable value is always >= 0 if it has an unsigned type.
  //    //byPStateID >=0 &&
  //    byPStateID < NUMBER_OF_PSTATES )
  //  {
  //    if( ! m_arp_pstate[byPStateID] )
  //      m_arp_pstate[byPStateID] = new PState();
  //    //If already allocated or allocating memory succeeded.
  //    if( m_arp_pstate[byPStateID] )
  //    {
  //      BYTE byDID = 0 , byFID = 14 , byPreviousFID = 14 ;
  //      BYTE byNumIters = 0 ;
  //      DIDandFID didnfidObereSchranke(0,14), didnfidUntereSchranke(8,0);
  //      WORD wFreqInMHzForDIDandFID ;
  //      do
  //      {
  //        DIDandFID didnfidCurrent(byDID,byFID) ;
  //        wFreqInMHzForDIDandFID = ( WORD) (( byFID + 8 )*100 ) / (1<<byDID) ;
  //        if(wFreqInMHz <= wFreqInMHzForDIDandFID )
  //        {
  //          //didnfidObereSchranke = DIDandFID(byDID,byFID);
  //          if( didnfidCurrent.GetDiff(wFreqInMHz) < 
  //            didnfidObereSchranke.GetDiff(wFreqInMHz) )
  //            didnfidObereSchranke = didnfidCurrent ;
  //          ++ byDID ;
  //        }
  //        else if(wFreqInMHz > wFreqInMHzForDIDandFID )
  //        {
  //          //didnfidUntereSchranke = DIDandFID(byDID,byFID);
  //          if( didnfidCurrent.GetDiff(wFreqInMHz) < 
  //            didnfidUntereSchranke.GetDiff(wFreqInMHz) )
  //            didnfidUntereSchranke = didnfidCurrent ;
  //          //if(byPreviousFID

  //          byPreviousFID = byFID ;
  //          //Do a kind of binary search because it's fast.
  //          if( byFID > 0 )
  //            byFID /= 2;
  //          else
  //            break ;
  //          //byDID = 0 ;
  //          if( byDID > 0 )
  //            -- byDID ;
  //          else
  //            break;
  //        }
  //        ++ byNumIters ;
  //      }while( ( byNumIters < 255 && wFreqInMHz != wFreqInMHzForDIDandFID ) 
  //      	|| byDID == 8 );
  //      if( didnfidUntereSchranke.GetDiff(wFreqInMHz) < 
  //        didnfidObereSchranke.GetDiff(wFreqInMHz) )
  //        m_arp_pstate[byPStateID]->SetDIDandFID(didnfidUntereSchranke);
  //      else
  //        m_arp_pstate[byPStateID]->SetDIDandFID(didnfidObereSchranke);
  //      //m_arp_pstate[byPStateID]->byFID = byFID ;
  //      //m_arp_pstate[byPStateID]->m_byDivisorID = byDID ;
  //      m_arp_pstate[byPStateID]->m_byNumber = byPStateID ;
  //    }
  //  }
  //}

  //void SetBruteForceFIDandDIDbyFreq(BYTE byPStateID, WORD wFreqInMHz)
  //{
  //  //m_byNumber = byPStateID ;
  //  if(wFreqInMHz <= m_wMaxFreq && byPStateID >=0 && byPStateID < 
  //    NUMBER_OF_PSTATES )
  //  {
  //    if( ! m_arp_pstate[byPStateID] )
  //      m_arp_pstate[byPStateID] = new PState();
  //    //If already allocated or allocating memory succeeded.
  //    if( m_arp_pstate[byPStateID] )
  //    {
  //      BYTE byCurrentDID = 0 ;
  //      BYTE byCurrentFID = 0 ;
  //      DIDandFID didnfidObereSchranke(0,14), didnfidUntereSchranke(8,0);
  //      WORD wFreqInMHzForDIDandFID ;
  //      for( ; byCurrentFID < 15 ; ++ byCurrentFID )
  //        for( byCurrentDID = 0; byCurrentDID < 4 ; ++ byCurrentDID )
  //        {
  //          DIDandFID didnfidCurrent(byCurrentDID,byCurrentFID) ;
  //          wFreqInMHzForDIDandFID = ( WORD) (( byCurrentFID + 8 )*100 ) / (1<<byCurrentDID) ;
  //          if(wFreqInMHz <= wFreqInMHzForDIDandFID )
  //          {
  //            //didnfidObereSchranke = DIDandFID(byDID,byFID);
  //            if( didnfidCurrent.GetDiff(wFreqInMHz) < 
  //              didnfidObereSchranke.GetDiff(wFreqInMHz) )
  //              didnfidObereSchranke = didnfidCurrent ;
  //          }
  //          else if(wFreqInMHz > wFreqInMHzForDIDandFID )
  //          {
  //            //didnfidUntereSchranke = DIDandFID(byDID,byFID);
  //            if( didnfidCurrent.GetDiff(wFreqInMHz) < 
  //              didnfidUntereSchranke.GetDiff(wFreqInMHz) )
  //              didnfidUntereSchranke = didnfidCurrent ;
  //          }
  //        }
  //      if( didnfidUntereSchranke.GetDiff(wFreqInMHz) < 
  //        didnfidObereSchranke.GetDiff(wFreqInMHz) )
  //        m_arp_pstate[byPStateID]->SetDIDandFID(didnfidUntereSchranke);
  //      else
  //        m_arp_pstate[byPStateID]->SetDIDandFID(didnfidObereSchranke);
  //      //m_arp_pstate[byPStateID]->byFID = byFID ;
  //      //m_arp_pstate[byPStateID]->m_byDivisorID = byDID ;
  //      m_arp_pstate[byPStateID]->m_byNumber = byPStateID ;
  //    }
  //  }
  //}

//  bool ISSafe(
//    PState & pstateCurrent,
//    BYTE byVID, 
//    BYTE byFrequencyID, 
//    BYTE byDivisorID)
//  {
//    
//  } ;
  static bool IsSafe(BYTE byVID, BYTE byFrequencyID, BYTE byDivisorID);
};
