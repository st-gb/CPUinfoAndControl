/*
 * Pentium4.hpp
 *
 *  Created on: 05.02.2013
 *      Author: Stefan
 */

#ifndef PENTIUM4_HPP_
#define PENTIUM4_HPP_

#include "ODCM.hpp"

extern uint32_t gs_ui32_t1 ;
extern uint32_t gs_ui32_t2 ;
extern bool gs_HT;

namespace Intel
{
  namespace Pentium4
  {
    inline BYTE SetCurrentVoltageAndMultiplier(float fVoltageInVolt,
      float fMultiplier, WORD wCoreID)
    {
      gs_ui32_t1 = 0;
      //  else
      if( fMultiplier < 1.0f )
        gs_ui32_t1 |= ( (BYTE) (fMultiplier / 0.125f) << 1) | 16;
      gs_ui32_t2 = 0;
      DEBUGN( FULL_FUNC_NAME << "--" << fMultiplier)
      DEBUGN( FULL_FUNC_NAME << "--On-Demand Clock Modulation value:" << gs_ui32_t1)

      //Intel(R) 64 and IA-32 Architectures Software Developer's Manual, Volume 3A:
      //"For Hyper-Threading Technology enabled processors, the
      //  IA32_CLOCK_MODULATION register is duplicated for each logical
      //  processor. In order for the On-demand clock modulation feature to
      //  work properly, the feature must be enabled on all the logical
      //  processors within a physical processor. If the programmed duty cycle
      //  is not identical for all the logical processors, the processor clock
      //  will modulate to the highest duty cycle programmed."
      if (gs_HT )
      {
        if( wCoreID % 2 == 0 ) //1st core, 3rd core, ...
          WriteMSR(IA32_CLOCK_MODULATION_REG_ADDR, gs_ui32_t1, gs_ui32_t2,
            1 << (wCoreID + 1) );
        else //2nd, 4th core ...
          WriteMSR(IA32_CLOCK_MODULATION_REG_ADDR, gs_ui32_t1, gs_ui32_t2,
            1 << (wCoreID - 1) );
      }
      //TODO to detect Hyperthreading:
      //Intel(R) 64 and IA-32 Architectures Software Developer's Manual, Volume 3A,
      // "8.6 DETECTING HARDWARE MULTI-THREADING SUPPORT AND TOPOLOGY"
      return WriteMSR(IA32_CLOCK_MODULATION_REG_ADDR, gs_ui32_t1, gs_ui32_t2,
        1 << wCoreID);
    }
  }
}

#endif /* PENTIUM4_HPP_ */
