/** Source code from CrystalCPUID */

#include <Controller/CPU-related/AMD/AMD_MSR_adresses.h>
#include <hardware/CPU/x86/AMD/VoltageID.hpp>
//FIDVID_CTL_MSR_ADDRESS
#include <Controller/CPU-related/AMD/K7/AMD_K7_MSR_addresses.h>

#define UNDEFINED_VOLTAGE_ID 15

namespace CrystalCPUID
{
//  void SetPcr()
//  {
//    DWORD address = gAddress;
//    DWORD NorthID = gNorthID;
//    UCHAR c;
//
//    if( gK7FVID == TRUE ){
//      return;
//    }
//    gK7FVID = TRUE;
//
//    // Enable Change Multiplier for AMD 761
//    if(	NorthID == 0x1022700C // AMD-762 (760MP/MPX)
//    ||	NorthID == 0x1022700E // AMD-761 (760)
//    ){
//      c = _pciConfigReadChar( address >> 16, 0x44);
//      if( ! (c & 0x1) ){
//        c |= 0x1;
//        _pciConfigWriteChar( address >> 16, 0x44, c);
//      }
//    }
//    // Enable Change Multiplier for VIA KT133
//    if(	NorthID == 0x11060305 // KT133
//    ||	NorthID == 0x11060391 // KX133
//    ||	NorthID == 0x11060601 // PLE133
//    ||	NorthID == 0x11060605 // ProSavegeDDR PM133/PN133
//    ||	NorthID == 0x11063112 // Apollo KLE133
//    ){
//      c = _pciConfigReadChar( address >> 16, 0x55);
//      if( ! (c & 0x4) ){
//        c |= 0x4;
//        _pciConfigWriteChar( address >> 16, 0x55, c);
//      }
//    }
//
//    // Enable Change Multiplier for NVIDIA nForce2
//    if(	NorthID == 0x10DE01E0 // nForce2
//    ||	NorthID == 0x10DE01F0 // nForce2
//    ){
//      c = _pciConfigReadChar( address >> 16, 0xF6);
//      if( ! (c & 0x10) ){ // Bit 4
//        c |= 0x10;
//        _pciConfigWriteChar( address >> 16, 0xF6, c);
//      }
//      c = _pciConfigReadChar( address >> 16, 0xE7);
//      if( ! (c & 0x10) ){ // Bit 4
//        c |= 0x10;
//        _pciConfigWriteChar( address >> 16, 0xE7, c);
//      }
//    }
//
//  }
//
//  void UnsetPcr()
//  {
//    DWORD address = gAddress;
//    DWORD NorthID = gNorthID;
//    UCHAR c;
//
//    // Disable Change Multiplier for AMD 761
//    if(	NorthID == 0x1022700C // AMD-762 (760MP/MPX)
//    ||	NorthID == 0x1022700E // AMD-761 (760)
//    ){
//      c = _pciConfigReadChar( address >> 16, 0x44);
//      if( c & 0x1 ){ // Bit 0
//        c ^= 0x1;
//        _pciConfigWriteChar( address >> 16, 0x44, c);
//      }
//    }
//    // Disable Change Multiplier for VIA KT133
//    if(	NorthID == 0x11060305 // KT133
//    ||	NorthID == 0x11060391 // KX133
//    ||	NorthID == 0x11060601 // PLE133
//    ||	NorthID == 0x11060605 // ProSavegeDDR PM133/PN133
//    ||	NorthID == 0x11063112 // Apollo KLE133
//    ){
//      c = _pciConfigReadChar( address >> 16, 0x55);
//      if( c & 0x4 ){ // Bit 2
//        c ^= 0x4;
//        _pciConfigWriteChar( address >> 16, 0x55, c);
//      }
//    }
//
//    // Disable Change Multiplier for NVIDIA nForce2
//    if(	NorthID == 0x10DE01E0 // nForce2
//    ||	NorthID == 0x10DE01F0 // nForce2
//    ){
//      c = _pciConfigReadChar( address >> 16, 0xF6);
//      if( c & 0x10 ){ // Bit 4
//        c ^= 0x10;
//        _pciConfigWriteChar( address >> 16, 0xF6, c);
//      }
//      c = _pciConfigReadChar( address >> 16, 0xE7);
//      if( c & 0x10 ){ // Bit 4
//        c ^= 0x10;
//        _pciConfigWriteChar( address >> 16, 0xE7, c);
//      }
//    }
//  }

  #define LOOP_MAX 100000

  void SetFID(const fastestUnsignedDataType frequencyIDtoSet)
  {
    DEBUGN( /*FULL_FUNC_NAME << "--"*/ "begin FID:" << frequencyIDtoSet)
    const fastestUnsignedDataType NumberOfCPUs = 1;
    static fastestUnsignedDataType dualWaitTimeInMilliseconds = 1000;
    uint32_t EAX, EDX;
//    SetPcr();
    for( fastestUnsignedDataType i = 0; i <= NumberOfCPUs; i++ )
    {
//      DWORD mask;
  //    if( IsWindowsNT() ){
//        mask = (DWORD)SetThreadAffinityMask(GetCurrentThread(), 1<<(i-1));
//        if( i >= 2 ){
//          ::Sleep(dualWaitTimeInMilliseconds);
//  //      }
//      }
      ReadMSR(FIDVID_CTL_MSR_ADDRESS, & EAX, & EDX, 1);
      EDX = 0x7D0;		// Allow minimum 10 us setting time
      EAX &= 0xFFFFFFC0;
      EAX |= /*(1<<16)*/ BITMASK_FOR_SETTING_FREQUENCY_ID;	// Set bit 16
      EAX |= frequencyIDtoSet;
      WriteMSR(FIDVID_CTL_MSR_ADDRESS, EAX, EDX, 1);
      int loop = 0;
      do{
        ReadMSR(FIDVID_STATUS_MSR_ADDRESS, & EAX, & EDX, 1);
        //TODO maybe do a "::Sleep(0)" (give remainder of time slice to other
        //processes) under Windows here instead of busy waiting via
        //incrementing a variable/ continuously reading
        //FIDVID_STATUS_MSR_ADDRESS.
        loop ++;
      }while( /*(EAX >> 31) & 0x1*/
          FID_OR_VID_CHANGE_IS_PENDING(EAX) && loop < LOOP_MAX );
  //    if( IsNT() ){
//        SetThreadAffinityMask(GetCurrentThread(), mask);
  //    }
    }
  }

  /** @brief sets voltage in single Voltage ID steps. Sourcecode from CrytsalCPUID */
  void SetVID(const fastestUnsignedDataType voltageIDtoSet)
  {
    DEBUGN( /*FULL_FUNC_NAME << "--"*/ "begin VID:" << voltageIDtoSet)
//    SetPcr();
//    if((! FlagK7Desktop && voltageIDtoSet == 15 ) || voltageIDtoSet == 31){
//      break;
//    }
    static bool FlagK7Desktop = false;
    const fastestUnsignedDataType NumberOfCPUs = 1;
    uint32_t EAX, EDX;
    fastestUnsignedDataType currentVoltageID = 0;
    fastestUnsignedDataType voltageIDsummand;

    for( fastestUnsignedDataType i = 1; i <= NumberOfCPUs; i++ )
    {
//      DWORD mask;
//      if( IsNT() ){
//        mask = (DWORD)SetThreadAffinityMask(GetCurrentThread(), 1<<(i-1));
//        if( i >= 2 )
//        {
//          Sleep(DualWaitTime);
//        }
//      }
      DWORD count = 32;
      while( count -- )
      {
        DWORD NextVID = 0;

        ReadMSR( FIDVID_STATUS_MSR_ADDRESS, & EAX, & EDX, 1);
        currentVoltageID = EDX & /*0x1F*/ BITMASK_FOR_VOLTAGE_IDENTIFICATION;
//        if(voltageIDtoSet > currentVoltageID)
//        {
//          voltageIDsummand = AMD::x86::incrementVoltageID;
//        }
//        else
//          voltageIDsummand = AMD::x86::decrementVoltageID;
//        currentVoltageID += voltageIDsummand;
//        if( currentVoltageID == UNDEFINED_VOLTAGE_ID)
//          currentVoltageID += voltageIDsummand;

        if(voltageIDtoSet > currentVoltageID)
        {
          NextVID = currentVoltageID + 1;
          if( NextVID == 15 && ! FlagK7Desktop ){
            NextVID ++;
          }
        }else if(voltageIDtoSet < currentVoltageID)
        {
          NextVID = currentVoltageID - 1;
          if( NextVID == 15 && ! FlagK7Desktop ){
            NextVID--;
          }
        }else
        {
          break;
        }

        if( NextVID == 31 || NextVID < 0 || NextVID > 31 )
        {
          break;
        }

        ReadMSR(FIDVID_CTL_MSR_ADDRESS, & EAX, & EDX, 1);
//        EAX &= 0xFFFFE0FF; // unmask/ clear voltage ID
        EAX |= BITMASK_FOR_SETTING_VOLTAGE_ID; // Set bit 17    (VID)
//        EAX &= 0xFFFEFFFF; // Clear bit 16  (FID)
        EAX &= BITMASK_FOR_CLEARING_VOLTAGE_ID_AND_UNSETTING_FID;
        EDX = 0x64; // Allow minimum 100 ms settling time
        EAX |= ( NextVID << 8 );
        WriteMSR(FIDVID_CTL_MSR_ADDRESS, EAX, EDX, 1);
        int loop = 0;
        do{
          ReadMSR(FIDVID_STATUS_MSR_ADDRESS, & EAX, & EDX, 1);
          //TODO maybe do a "::Sleep(0)" (give remainder of time slice to other
          //processes) under Windows here instead of busy waiting via
          //incrementing a variable/ continuously reading
          //FIDVID_STATUS_MSR_ADDRESS.
          loop ++;
        }while( /*(EAX >> 31) & 0x1*/ FID_OR_VID_CHANGE_IS_PENDING(EAX)
          && loop < LOOP_MAX );
      }
//      if( IsNT() ){
//        SetThreadAffinityMask(GetCurrentThread(), mask);}
    }
  }

  namespace adapted
  {
    /** @brief sets voltage in single Voltage ID steps. Sourcecode from CrytsalCPUID */
    void SetVID(const fastestUnsignedDataType voltageIDtoSet)
    {
      DEBUGN( /*FULL_FUNC_NAME << "--"*/ "begin VID:" << voltageIDtoSet)
  //    SetPcr();
  //    if((! FlagK7Desktop && voltageIDtoSet == 15 ) || voltageIDtoSet == 31){
  //      break;
  //    }
      static bool FlagK7Desktop = false;
      const fastestUnsignedDataType NumberOfCPUs = 1;
      uint32_t EAX, EDX;
      fastestUnsignedDataType currentVoltageID = 0;
      fastestUnsignedDataType voltageIDsummand;

      for( fastestUnsignedDataType i = 1; i <= NumberOfCPUs; i++ )
      {
  //      DWORD mask;
  //      if( IsNT() ){
  //        mask = (DWORD)SetThreadAffinityMask(GetCurrentThread(), 1<<(i-1));
  //        if( i >= 2 )
  //        {
  //          Sleep(DualWaitTime);
  //        }
  //      }
        DWORD count = 32;
        while( count -- )
        {
          DWORD NextVID = 0;

          ReadMSR( FIDVID_STATUS_MSR_ADDRESS, & EAX, & EDX, 1);
          currentVoltageID = EDX & /*0x1F*/ BITMASK_FOR_VOLTAGE_IDENTIFICATION;
          if(voltageIDtoSet > currentVoltageID)
          {
            voltageIDsummand = AMD::x86::incrementVoltageID;
          }
          else
            voltageIDsummand = AMD::x86::decrementVoltageID;
          currentVoltageID += voltageIDsummand;
          if( currentVoltageID == UNDEFINED_VOLTAGE_ID)
            currentVoltageID += voltageIDsummand;

          ReadMSR(FIDVID_CTL_MSR_ADDRESS, & EAX, & EDX, 1);
  //        EAX &= 0xFFFFE0FF; // unmask/ clear voltage ID
          EAX |= BITMASK_FOR_SETTING_VOLTAGE_ID; // Set bit 17    (VID)
  //        EAX &= 0xFFFEFFFF; // Clear bit 16  (FID)
          EAX &= BITMASK_FOR_CLEARING_VOLTAGE_ID_AND_UNSETTING_FID;
          EDX = 0x64; // Allow minimum 100 ms settling time
          EAX |= ( NextVID << 8 );
          WriteMSR(FIDVID_CTL_MSR_ADDRESS, EAX, EDX, 1);
          int loop = 0;
          do{
            ReadMSR(FIDVID_STATUS_MSR_ADDRESS, & EAX, & EDX, 1);
            //TODO maybe do a "::Sleep(0)" (give remainder of time slice to other
            //processes) under Windows here instead of busy waiting via
            //incrementing a variable/ continuously reading
            //FIDVID_STATUS_MSR_ADDRESS.
            loop ++;
          }while( /*(EAX >> 31) & 0x1*/ FID_OR_VID_CHANGE_IS_PENDING(EAX)
            && loop < LOOP_MAX );
        }
  //      if( IsNT() ){
  //        SetThreadAffinityMask(GetCurrentThread(), mask);}
      }
    }
  }
}
