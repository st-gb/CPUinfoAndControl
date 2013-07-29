/*
 * CoreAndCore2.hpp
 *
 *  Created on: 23.12.2012
 *      Author: Stefan
 */

#ifndef COREANDCORE2_HPP_
#define COREANDCORE2_HPP_

#include <float.h> //FLT_MIN
#include <fastest_data_type.h> //typedef fastestUnsignedDataType

extern BYTE g_byValue1;
extern uint32_t g_dwValue1, g_dwValue2;
extern float g_fReferenceClockInMHz;

#ifdef COMPILE_FOR_INTEL_CORE2
  //for Intel Core 2 Celeron 900:
  //Core Temp 1.0 RC3 shows TjMax of 105°C
  //RealTemp 3.70 and hwmonitor 1.22.0 , driver version 136 assumes 100°C ?
  #define JUNCTION_TEMPERATURE_IN_DEGREES_CELSIUS 105
#endif
#ifdef COMPILE_FOR_INTEL_ATOM
  //mobile-atom-n270-single-core-datasheet-.pdf: May 2008 Document Number: 320032-001
  // chapter "3.10  Processor DC Specifications"
  //"Unless specified otherwise, all specifications for the processor are at TJ = 90°C."
  //http://ark.intel.com/products/36331/Intel-Atom-Processor-N270-512K-Cache-1_60-GHz-533-MHz-FSB:
  // "TJUNCTION   90°C"
  #define JUNCTION_TEMPERATURE_IN_DEGREES_CELSIUS 90
#else
  #ifdef COMPILE_FOR_INTEL_CORE
    #define JUNCTION_TEMPERATURE_IN_DEGREES_CELSIUS 100
  #endif
#endif

namespace Intel
{
  namespace CoreAndCore2
  {
    inline float * AllocateAndFillVoltageArray(
      fastestUnsignedDataType lowestVoltageID,
      fastestUnsignedDataType highestVoltageID,
      uint16_t & r_byNumVoltages)
    {
      //Number of different voltages = highest voltage ID-lowest voltage ID + 1
//      g_byValue1 =
      r_byNumVoltages =
        //Highest voltage ID.
        highestVoltageID - lowestVoltageID
//        //lowest voltage ID
//        - g_byValue2
        + 1 ;
      DEBUGN("# voltages:" << (WORD) r_byNumVoltages )
      float * ar_f = new float [ //g_byValue1
        r_byNumVoltages ] ;
      //g_byValue1 = r_byNumVoltages ;
      if( ar_f ) //Allocating memory on heap succeeded.
      {
        const float minVoltageInVolt = //0.7125 + lowestVoltageID * 0.0125;
          GetVoltage(lowestVoltageID);
        DEBUGN( "min voltage in Volt:" << minVoltageInVolt)
        float voltageInVolt;
        //for( -- g_byValue1 ; g_byValue1 > 255 ; -- g_byValue1 )
        for( g_byValue1 = 0 ; g_byValue1 < r_byNumVoltages ;
            ++ g_byValue1 )
        {
          voltageInVolt = minVoltageInVolt +
          //            //Minimum voltage ID + Index
          //            ( g_byValue2 + g_byValue1 )
              g_byValue1 * 0.0125 ;
          DEBUGN("adding " << voltageInVolt << "V")
          ar_f[g_byValue1] = voltageInVolt;
        }
      }
      return ar_f;
    }

    inline void GetReferenceClockFromMSR_FSB_FREQ()
    {
      //"CDH 205 MSR_FSB_FREQ Shared [between cores?] Scaleable Bus Speed(RO)".
  //      This field indicates the intended scaleable bus
  //      clock speed for processors based on Intel Core
  //      microarchitecture:
  //    "2:0"
      if( ReadMSR(
          MSR_FSB_FREQ ,
          & g_dwValue1 , //lowmost bits 0-31
          & g_dwValue2 ,
          1
          )
        )
      {
  //      DEBUGN("Scaleable Bus Speed(RO):" <<
  //        g_dwValue1 & BITMASK_FOR_LOWMOST_3BIT )
        //bits: "2:0"
        switch( g_dwValue1 & BITMASK_FOR_LOWMOST_3BIT )
        {
  //      •101B: 100 MHz (FSB 400)
  //      •001B: 133 MHz (FSB 533)
  //      •011B: 167 MHz (FSB 667)
  //      •010B: 200 MHz (FSB 800)
  //      •000B: 267 MHz (FSB 1067)
  //      •100B: 333 MHz (FSB 1333)
        case 0:
          g_fReferenceClockInMHz = 267 ;
          break ;
        case 1:
          g_fReferenceClockInMHz = 133 ;
          break ;
        case 2:
          g_fReferenceClockInMHz = 200 ;
          break ;
        case 3:
          g_fReferenceClockInMHz = 167 ;
          break ;
        case 4:
          g_fReferenceClockInMHz = 333 ;
          break ;
        case 5: //101bin
          g_fReferenceClockInMHz = 100 ;
          break ;
        }
      }
    }

    float GetVoltageID_asFloat(float fVoltageInVolt)
    {
      //see "GetCurrentVoltageAndFrequency(...)":
      //VoltageInVolt = 0.7125 + VoltageID * 0.0125 | -0.7125
      //VoltageInVolt - 0.7125 = VoltageID * 0.0125 | :0.0125
      //( VoltageInVolt - 0.7125 ) / 0.0125 = VoltageID
      //<=> VoltageID = ( VoltageInVolt - 0.7125 ) / 0.0125
      return (fVoltageInVolt - 0.7125f ) / 0.0125f ;
    }

    /** Purpose of this function: converting from float to integer causes rounding
    * errors:
    * Analogy: for an AMD Griffin at 1.18 V the voltage ID in float is 29.999998
    *  and would get 29 (because automatically rounded down 1.164 V) when
    *  converting to an integer.
    * For 1.164 voltage ID in float is 29.000004 */
    inline BYTE GetClosestVoltageID(float fVoltageInVolt )
    {
      //WORD wVoltageID = (fVoltageInVolt - 0.7 ) / 0.016 ;
    //  float fVoltageMinusLowestVoltage = (fVoltageInVolt - 0.7 ) ;
      float fVoltageID = //fVoltageMinusLowestVoltage / 0.016 ;
        GetVoltageID_asFloat(fVoltageInVolt) ;
      DEBUGN("GetClosestVoltageID--fVoltageID" << fVoltageID )
      //ceil( fVoltageID );
      WORD wVoltageID = //(fVolt - 0.7 ) / 0.016 ;
        //Avoid g++ warning "warning: converting to `WORD' from `float'"
        (WORD) fVoltageID ;
      DEBUGN("GetClosestVoltageID--wVoltageID" << wVoltageID )
      if( fVoltageID - (float) wVoltageID >=
          //e.g. if desired voltage is 7.014: 7.014 - 7 = 0.014; 0.014/0.016=0.875
          0.5 )
        ++ wVoltageID ;
      DEBUGN("GetClosestVoltageID--wVoltageID" << wVoltageID )
      DEBUGN("GetClosestVoltageID--voltage in Volt" << fVoltageInVolt
        << "-> voltage ID:" << wVoltageID )
      return wVoltageID ;
    }

    /** for Intel Celeron 900 (max. multiplier 11 at 200 MHz freq)
     * temperature in hwmonitor): */
#ifdef DO_NOT_COMPILE_INLINE
#else
    inline
#endif
    float GetTemperatureInDegCelsius(
      WORD wCoreID )
    {
      static unsigned diffToJunctionTemp;
      static unsigned resolutionInDegCelsius;
      DEBUGN( FULL_FUNC_NAME << " begin")
      //"23:16 Temperature Target. (R) "
  //    byTempTarget = ( dwLowmostBits >> 16 ) & 255 ;
  //    (*g_pfnreadmsr) (
      g_byValue1 = ReadMSR(
         IA32_THERM_STATUS ,
         & g_dwValue1 , // bit  0-31 (register "EAX")
         & g_dwValue2 ,
         //m_dwAffinityMask
         1 << wCoreID
         ) ;
      if( g_byValue1 ) //if successfull
      {
        //"30:27 Resolution in Degrees Celsius (RO) If CPUID.06H:EAX[0] = 1"
        resolutionInDegCelsius = (g_dwValue1 >> 27) &
          BITMASK_FOR_LOWMOST_4BIT;
        //Intel: "22:16 Digital Readout (RO)"
        diffToJunctionTemp = ( g_dwValue1 >> 16 ) & BITMASK_FOR_LOWMOST_7BIT ;
  //      //Intel: "30:27 Resolution in Degrees Celsius (RO)"
  //      byResolutionInDegreesCelsius = ( dwLowmostBits >> 27 ) &
  //        BITMASK_FOR_LOWMOST_5BIT ;

        g_byValue1 = //-56 -
          JUNCTION_TEMPERATURE_IN_DEGREES_CELSIUS
          //TODO has this calculation correct? e.g.
          //-junction - diffToJunction * resInDegC e.g. 100 - 20*3=100-60=40
          //-(junction - diffToJunction) * resInDegC e.g.(100-20)*3=80*3=240
          - diffToJunctionTemp * resolutionInDegCelsius ;
        return g_byValue1 ;
      }
      return FLT_MIN ;
    }
  }
}


#endif /* COREANDCORE2_HPP_ */
