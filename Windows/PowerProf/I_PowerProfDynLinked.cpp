/*
 * I_PowerProfDynLinked.cpp
 *
 *  Created on: Apr 28, 2010
 *      Author: Stefan
 */
#include "I_PowerProfDynLinked.hpp"

  bool I_PowerProfDynLinked::DisableFrequencyScalingByOS()
  {
    bool bDesiredPowerSchemeExists = false ;
    //many gcc compile errors with LOGN _here_:
    // "/usr/lib/gcc/i686-pc-cygwin/4.3.4/include/c++/bits/istream.tcc:944:
    //error: expected primary-expression" etc.
    #ifdef _MSC_VER //if MS-compiler
    LOGN("Should disable Windows' Dynamic Voltage And Frequency Scaling.\n"
      "All available power schemes:")
    #endif
//    OutputAllPowerSchemes() ;
    //Even if the access to the power scheme differs between Windows Vista
    //and XP the logic for setting the power scheme is the same.
    //So to ensure the same implementation, implement the logic here
    //in the superordinate access.
    if( PowerSchemeToSetExists() == 1 )
    {
      //mp_dynfreqscalingaccess->ActivatePowerSchemeToSet() ;
      bDesiredPowerSchemeExists = true ;
    }
    else
    {
      if( CreatePowerSchemeWithWantedName() )
      {
    //many gcc compile errors with LOGN _here_:
    // "/usr/lib/gcc/i686-pc-cygwin/4.3.4/include/c++/bits/istream.tcc:944:
    //error: expected primary-expression" etc.
    #ifdef _MSC_VER //if MS-compiler
        LOGN("creating power scheme succeeded")
    #endif
        bDesiredPowerSchemeExists = true ;
      }
      else
      {
        //many gcc compile errors with LOGN _here_:
        // "/usr/lib/gcc/i686-pc-cygwin/4.3.4/include/c++/bits/istream.tcc:944:
        //error: expected primary-expression" etc.
        #ifdef _MSC_VER //if MS-compiler
        LOGN("creating power scheme failed")
         #endif
      }
    }
    if( bDesiredPowerSchemeExists )
    {
      DisableDVFSforPowerSchemeToSet() ;
      ActivatePowerSchemeToSet() ;
    }
    return false ;
  }
