/** File:   MSR_register_addresses.h
 * Author: s.gebauer
 * Created on 31. Juli 2017, 20:43   */

#ifndef MSR_REGISTER_ADDRESSES_H
#define MSR_REGISTER_ADDRESSES_H

/** 31116 Rev 3.62 - January 11, 2013 AMD Family 10h Processor BKDG 
*  41256 Rev 3.00 - July 07, 2008 AMD Family 11h Processor BKDG
*   "MSRC001_00[68:64] P-State [4:0] Registers" : "63 PstateEn"
* 2147483648 dec= 10000000 00000000 00000000 00000000 bin
*                 --8 bit- --8 bit- --8 bit- --8 bit-  */
#define SET_P_STATE_TO_VALID(highDWORD) ( highDWORD |= 2147483648UL )

//see AMDs "BIOS and Kernel Dev Guide" for family 10h, 11h:
#define P_STATE_CURRENT_LIMIT_REGISTER 0xC0010061 //"MSRC001_0061 P-state Current Limit Register"
#define P_STATE_CONTROL_REGISTER 0xC0010062 //"MSRC001_0062 P-state Control Register"
#define P_STATE_STATUS_REGISTER 0xC0010063 //"MSRC001_0063 P-state Status Register"

//"MSRC001_0070 COFVID Control Register"
#define COFVID_CONTROL_REGISTER_MSR_ADDRESS 0xC0010070
/** see "31116  Rev 3.00 - September 07, 2007  AMD Family 10h Processor BKDG"
 * "MSRC001_0071 COFVID Status Register" */
#define COFVID_STATUS_REGISTER_MSR_ADDRESS 0xC0010071

/** 41256 Rev 3.00 - July 07, 2008 AMD Family 11h Processor BKDG :
*  "CpuDid: core divisor ID. Read-write. Specifies the CPU frequency divisor;
*     see CpuFid.
*    0h=Divisor of 1     3h=Divisor of 8
*    1h=Divisor of 2     4h - 7h=Reserved
*    2h=Divisor of 4"  */
#define AMD_FAMILY_11H_MAXIMUM_CPU_CORE_DIVISOR_ID 3

#endif /* MSR_REGISTER_ADDRESSES_H */

