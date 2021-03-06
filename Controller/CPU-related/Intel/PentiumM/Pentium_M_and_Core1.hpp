/*
 * Pentium_M_GetAvailableMultipliers.hpp
 *
 *  Created on: 23.12.2012
 *      Author: Stefan
 */

#ifndef PENTIUM_M_GETAVAILABLEMULTIPLIERS_HPP_
#define PENTIUM_M_GETAVAILABLEMULTIPLIERS_HPP_

//#include
//for IA32_PERF_STATUS etc.
#include <Controller/CPU-related/Intel/Intel_registers.h>
extern ReadMSR_func_type g_pfnreadmsr ;

namespace Intel
{
  /** All functions within this namespace can be used for Intel Pentium M
   *  (family 6, model 13) and Intel Core 1 (family 6, model 14) CPUs. */
  namespace Pentium_M_and_Core1
  {
    //for "GetVoltage(...)" in GetDefaultPstates(...)
#ifdef PENTIUM_M_VOLTAGE
  #include <Controller/CPU-related/Intel/PentiumM/PentiumM_GetVoltage.hpp>
  using namespace Intel::Pentium_M;
#endif
#ifdef COMPILE_FOR_INTEL_CORE
  #include <Controller/CPU-related/Intel/Core/Core_and_Core2_GetVoltage.hpp>
  using namespace Intel::CoreAndCore2;
#endif

    inline float GetMaximumMultiplier(fastestUnsignedDataType highmostBits)
    {
      float fMaximumMultiplier = (float)
      //(BYTE) ( ui32LowmostBits & 255 ) ;
         ( ( highmostBits >> 8 ) & 255 ) ;
      return fMaximumMultiplier;
    }

    float *
      //the reference clock might change. So better get the possible multipliers.
      GetAvailableMultipliers(
        WORD * p_wNum )
    {
      float * ar_f = NULL;
      uint32_t ui32LowmostBits , ui32HighmostBits ;
    //  #ifdef _DEBUG
      //MSC-generated version has no problems
    //#ifndef _MSC_VER
    //    std::stringstream stdstrstream ;
    //  str << "DLL::GetMaximumFrequencyInMHz--Adress of CPUaccess: " << g_pi_cpuaccess ;
    //  //str << "address of CPUaccess->ReadMSRex: " << & I_CPUaccess::RdmsrEx ;
    //  str << "address of g_pfnreadmsr: " << g_pfnreadmsr ;
    //  MessageBox( NULL, str.str().c_str() , TEXT("") , MB_OK) ;
    //  #endif
      const fastestUnsignedDataType retVal =
       //g_pi_cpuaccess->RdmsrEx(
        ( * g_pfnreadmsr) (
          //MIN_AND_MAX_FID ,
          //According to the MSR walker of CrystalCPUID:
          //for Pentium M reg. addr. 0x198:
          //Bit 24-32 showed hex "0E" for a max. multipl. "14" for 1.86 133 MHz FSB.
          //Bit 24-32 showed hex "0C" for a max. multipl. "12" for 1.6 133 MHz FSB.
          IA32_PERF_STATUS ,
          & ui32LowmostBits,// bit  0-31 (register "EAX")
          & ui32HighmostBits,
          //m_dwAffinityMask
          1
          ) ;
      if( retVal)
      {
#ifdef EMULATE_N270
        const fastestUnsignedDataType minMultiplier = 6;
        const fastestUnsignedDataType maxMultiplier = 0xC;
#else
        const fastestUnsignedDataType maxMultiplier = GetMaximumMultiplier(
          ui32HighmostBits);
         //According to the MSR walker of CrystalCPUID the min. multi is at the
         //highmost byte: was "06" for every tested CPU (1.6GHz FSB133,
         // 1.86 GHz FSB 133, 1.8GHz FSB 100)
        const fastestUnsignedDataType minMultiplier =
          ( ui32HighmostBits >> 24 ) & 255 ;
#endif
      const fastestUnsignedDataType numMultis = maxMultiplier - minMultiplier
    #ifndef LEAVE_OUT_MULTIPLIER_7
           //add "+1" : if min and max identical, the array size must be "1"
           + 1
    #endif
           ;
       ar_f = new float[numMultis] ;
       //Allocation of storage succeeded.
       if( ar_f )
       {
         * p_wNum = numMultis ;
         float fMulti = minMultiplier ;
    //     stdstrstream << "float array addr.:" << ar_f << " avail. multis:" ;
         for( BYTE by = 0 ; by < numMultis ; ++ by )
         {
           ar_f[by] = fMulti ;
    #ifdef LEAVE_OUT_MULTIPLIER_7
           if( fMulti == 6) //skip multiplier "7" (next is multiplier "8")
           {
             ++ fMulti;
           }
    #endif
    //       stdstrstream << fMulti << " " ;
           ++ fMulti ;
         }
       }
       else
         *p_wNum = 0 ;
    //     MessageBox(NULL, stdstrstream.str().c_str(), "info", MB_OK) ;
    #ifdef _DEBUG
      //if( wCoreID == 0 )
      //{
      // str.clear() ;
      //str << "DLL::GetMaximumFrequencyInMHz--after of RdmsrEx: " << g_pi_cpuaccess
      //  << "multiplier:" << (WORD)byMaxMultiplier ;
      ////MessageBox( NULL, str.str().c_str() , TEXT("") , MB_OK) ;
      //}
    #endif
      }
      else
        p_wNum = 0;
      return ar_f ;
    }

    /** @brief Intel Pentium M, Intel Core 1 and Atom have default voltages
     *  for min and max multiplier stored in MSRegister. */
    inline BYTE GetDefaultPstates(
      float & fVoltageForLowestMulti,
      float & fLowestMulti,
      float & fVoltageForHighestMulti,
      float & fHighestMulti
      )
    {
      uint32_t ui32LowmostBits, ui32HighmostBits;
      if( ReadMSR(
        //MIN_AND_MAX_FID ,
        //According to the MSR walker of CrystalCPUID:
        //for Pentium M reg. addr. 0x198:
        //Bit 24-32 showed hex "0E" for a max. multipl. "14" for 1.86 133 MHz FSB.
        //Bit 24-32 showed hex "0C" for a max. multipl. "12" for 1.6 133 MHz FSB.
        IA32_PERF_STATUS ,
        & ui32LowmostBits, // bits 0-31 (register "EAX")
        & ui32HighmostBits,
        //m_dwAffinityMask
    //    1 << wCoreID
        1
        )
      )
      {
        //CrystalCPUID's MSR walker:
        //00000198 : 06120C26 06000C26
        //           06 : min multi
        //             12: default voltage for min multi
        //               0C : max multi
        //                 26: default voltage for max multi
        fVoltageForHighestMulti = GetVoltage(
          ( ui32HighmostBits & BITMASK_FOR_LOWMOST_8BIT ) ) ;
        DEBUGN("default voltageForHighestMulti: " << fVoltageForHighestMulti )

        ui32HighmostBits >>= 8 ;
        fHighestMulti = ( ui32HighmostBits & BITMASK_FOR_LOWMOST_8BIT ) ;
        DEBUGN("fHighestMulti: " << fHighestMulti )

        ui32HighmostBits >>= 8 ;
        fVoltageForLowestMulti = GetVoltage(
          ( ui32HighmostBits & BITMASK_FOR_LOWMOST_8BIT ) ) ;
        DEBUGN("default voltage forLowestMulti: " << fVoltageForLowestMulti )

        ui32HighmostBits >>= 8 ;
        fLowestMulti = ( ui32HighmostBits & BITMASK_FOR_LOWMOST_8BIT ) ;
        DEBUGN("fLowestMulti: " << fLowestMulti )
        return 1 ;
      }
      return 0 ;
    }

#ifdef INSERT_DEFAULT_P_STATES
    inline void InsertDefaultVoltagePstates(I_CPUaccess * pi_cpuaccess)
    {
      float fVoltageForLowestMulti ;
      float fLowestMulti ;
      float fVoltageForHighestMulti ;
      float fHighestMulti ;

      if( //Intel::Pentium_M_and_Core1::
          GetDefaultPstates(
          fVoltageForLowestMulti,
          fLowestMulti,
          fVoltageForHighestMulti,
          fHighestMulti
          )
        )
      {
        DEBUGN("adding default voltage " << fVoltageForLowestMulti << " for "
          << (WORD) fLowestMulti * g_fReferenceClockInMHz << "MHz" )
        pi_cpuaccess->mp_model->m_cpucoredata.//m_stdsetvoltageandfreqDefault.insert() ;
          AddDefaultVoltageForFreq_inline( fVoltageForLowestMulti,
            (WORD) ( fLowestMulti * g_fReferenceClockInMHz )
            ) ;
        DEBUGN("adding default voltage " << fVoltageForHighestMulti << " for "
          << (WORD) fHighestMulti * g_fReferenceClockInMHz << "MHz" )
        pi_cpuaccess->mp_model->m_cpucoredata.//m_stdsetvoltageandfreqDefault.insert() ;
          AddDefaultVoltageForFreq_inline( fVoltageForHighestMulti,
            (WORD) ( fHighestMulti * g_fReferenceClockInMHz)
            ) ;
      }
      DEBUGN( FULL_FUNC_NAME << " end")
    }
#endif //INSERT_DEFAULT_P_STATES
  }//namespace Pentium_M_and_Core1
}//namespace Intel

#endif /* PENTIUM_M_GETAVAILABLEMULTIPLIERS_HPP_ */
