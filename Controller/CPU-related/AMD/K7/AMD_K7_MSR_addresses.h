/*
 * AMD_K7_MSR_addresses.h
 *
 *  Created on: Aug 28, 2013
 *      Author: Stefan
 */

#ifndef AMD_K7_MSR_ADDRESSES_H_
#define AMD_K7_MSR_ADDRESSES_H_

/** for AMD K7, K8:
* see 32559 Rev. 3.16 November 2009 BIOS and Kernel Developer’s Guide for AMD
* NPT Family 0Fh.pdf:
* "MSR C001_0041h (FIDVID_CTL) and MSR C001_0042h (FIDVID_STATUS) are used for
initiating P-state transitions and verifying that they are complete." */
#define FIDVID_CTL_MSR_ADDRESS 0xC0010041
#define FIDVID_STATUS_MSR_ADDRESS 0xC0010042

//for AMD K7; from CrystalCPUID sourcecode
#define BITMASK_FOR_VOLTAGE_IDENTIFICATION 0x1F //=1 1111 1111bin
#define BITMASK_FOR_CLEARING_VOLTRAGE_ID 0xFFFFE0FF
#define BITMASK_FOR_SETTING_VOLTAGE_ID 0x00020000
#define BITMASK_FOR_SETTING_FREQUENCY_ID 0x00010000
#define BITMASK_FOR_NOT_SETTING_FREQUENCY_ID 0xFFFEFFFF
#define BITMASK_FOR_CLEARING_VOLTAGE_ID_AND_UNSETTING_FID \
  BITMASK_FOR_CLEARING_VOLTRAGE_ID & BITMASK_FOR_NOT_SETTING_FREQUENCY_ID

#endif /* AMD_K7_MSR_ADDRESSES_H_ */
