#pragma once

#include <hardware/CPU/fastest_data_type.h> //fastestUnsignedDataType

namespace AMD
{
  namespace family15h /** Bulldozer*/
  {
    inline float GetMultiplier(
        const fastestUnsignedDataType frequencyID,
        const fastestUnsignedDataType divisorID)
    {
      /** "42301 Rev 3.14 - January 23, 2013 BKDG for AMD Family 15h Models
       *  "00h-0Fh Processors" :
       *  "CoreCOF = 100 * (MSRC001_00[6B:64][CpuFid] + 10h) /
       *   (2^MSRC001_00[6B:64][CpuDid])"  */
      return (float) (frequencyID + 0x10) / (float) (2 << divisorID);
    }

    /** inbetween VID "148" is shown when under load and on highest multiplier
     *  VID 11 corresponds to 1.428 V? in CPU-Z
     *  VID 14 corresponds to 1.296/ 1.284 V in CPU-Z
     *  VID 47 corresponds to 1.008 V shown in CPU-Z
     *
     *  1.296V-1.008V=0,288V  47-14=33
     *  1.428V- 1.008V=0,42V
     * */
    inline float GetVoltageInVolt(const fastestUnsignedDataType voltageID)
    {
      return 1.55f-voltageID*0.0125f;
    }

    inline BYTE GetCurrentVoltageAndFrequency(
        float * p_fVoltageInVolt
        //multipliers can also be floats: e.g. 5.5 for AMD Griffin.
        , float * p_fMultiplier
        , float * p_fReferenceClockInMHz
        , fastestUnsignedDataType wCoreID
        )
    {
      static uint32_t EAX, EDX;
      static fastestUnsignedDataType frequencyID, divisorID;
      static BYTE by ;
      DEBUGN( /*FULL_FUNC_NAME << "--"*/ "begin core ID:" << wCoreID)
      /** "42301 Rev 3.14 - January 23, 2013 BKDG for AMD Family 15h Models 00h-0Fh Processors":
       * CoreCOF = 100 * (MSRC001_00[6B:64][CpuFid] + 10h) / (2^MSRC001_00[6B:64][CpuDid]). */
      by = ::ReadMSR(
        COFVID_STATUS_REGISTER_MSR_ADDRESS,
        & EAX,
        & EDX,
        1 << wCoreID
        );

      AMD::fromK10::GetDivisorID(EAX, divisorID);
      frequencyID = AMD::fromK10::GetFrequencyID(EAX);
      * p_fMultiplier = AMD::family15h::GetMultiplier(frequencyID, divisorID);

      const fastestUnsignedDataType voltageID = AMD::fromK10::
        GetVoltageIDfromCOFVIDstatusRegisterBits(EAX);
      * p_fVoltageInVolt = AMD::family15h::GetVoltageInVolt(voltageID);
//      * p_fReferenceClockInMHz = 0.0f;
      AMD::fromK10::GetReferenceClock(p_fReferenceClockInMHz, wCoreID);
      return by;
    }
  }
}
