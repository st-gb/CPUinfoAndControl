/** AMD_NPT_family_0Fh_FIDVID_STATUS_register.h
 *  Created on: 12.06.2012
 *      Author: Stefan  */

#ifndef AMD_NPT_FAMILY_0FH_FIDVID_STATUS_REGISTER_H_
#define AMD_NPT_FAMILY_0FH_FIDVID_STATUS_REGISTER_H_

/** All information from:
 *  BIOS and Kernel Developer’s Guide for AMD NPT Family 0Fh Processors
 *  document ID: 32559 Rev. 3.08    July 2007 */

/** "61 IntPstateSup Intermediate P-state support" */
#define INTERMEDIATE_P_STATE_SUPPORT_BIT_ADDRESS 61
// "56 PstateStep P-state Voltage Step"

/**  "37–32 CurrVID Current VID" */
#define CURRENT_VID_START_ADDRESS_IN_BIT 32

/** "29–24 MaxRampVID" */
#define MAX_RAMP_VID_START_ADDRESS_IN_BIT 24
/** "21–16 MaxFID Max FID" */
#define MAX_FID_START_ADDRESS_IN_BIT 16

#define START_VID_START_ADDRESS_IN_BIT_IN_HIGHMOST_BYTES 8

#endif /* AMD_NPT_FAMILY_0FH_FIDVID_STATUS_REGISTER_H_ */
