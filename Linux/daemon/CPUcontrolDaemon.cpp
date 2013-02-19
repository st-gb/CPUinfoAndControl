/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
/*
 * CPUcontrolDaemon.cpp
 *
 *  Created on: Oct 11, 2010
 *      Author: sgebauer
 */

#include "CPUcontrolDaemon.hpp"
#include <Controller/GetNumberOfLogicalCPUcores.h>
#include <Linux/MSRdeviceFile.hpp>

#define EXIT_SUCCESS 0

namespace Linux
{

  CPUcontrolDaemon::CPUcontrolDaemon(
    //user interface is needed for outputting messages.
    UserInterface * p_userinterface)
    : CPUcontrolServiceBase(p_userinterface)
  {
  }

//  CPUcontrolDaemon::~CPUcontrolDaemon()
//  {
//  }

//  //Called by the base class.
//  void CPUcontrolDaemon::CreateHardwareAccessObject()
//  {
//    mp_i_cpuaccess = new MSRdeviceFile(
//      & m_dummyuserinterface ,
//      GetNumberOfLogicalCPUcores() ,
//      m_model
//      ) ;
//  }

  /** @return true; success false: otherwise */
  bool CPUcontrolDaemon::Start()
  {
    BYTE byVoltageAndFrequencyScalingType;
    if( Initialize(
      //dummy parameter values.
      0, NULL, byVoltageAndFrequencyScalingType) == EXIT_SUCCESS
      )
    {
//      if( mp_cpucoreusagegetter )
        return ! HandleStartDynVoltAndFreqScalingThread() ;
          //HandleStartCPUcoreLoadBasedDynVoltAndFreqScalingThread() ;
//      else
//        return HandleStartGetCPUcoreInformationThread() ;
    }
//    StartDynamicVoltageAndFrequencyScaling() ;
    return false ;
  }
}
