/*
 * AMD_NPT_family_0Fh_FIDVID_STATUS_register.h
 *
 *  Created on: 12.06.2012
 *      Author: Stefan
 */

#ifndef AMD_NPT_FAMILY_0FH_FIDVID_STATUS_REGISTER_H_
#define AMD_NPT_FAMILY_0FH_FIDVID_STATUS_REGISTER_H_

// "61 IntPstateSup Intermediate P-state support"
#define INTERMEDIATE_P_STATE_SUPPORT_BIT_ADDRESS 61
// "56 PstateStep P-state Voltage Step"

#define MAX_RAMP_VID_START_ADDRESS_IN_BIT 24 // "29–24 MaxRampVID"

#define START_VID_START_ADDRESS_IN_BIT_IN_HIGHMOST_BYTES 8

#endif /* AMD_NPT_FAMILY_0FH_FIDVID_STATUS_REGISTER_H_ */
