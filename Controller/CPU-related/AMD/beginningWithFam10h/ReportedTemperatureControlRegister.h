/** File:   ReportedTemperatureControlRegister.h
 * Author: s.gebauer
 * Created on 31. Juli 2017, 20:46  */

#ifndef REPORTEDTEMPERATURECONTROLREGISTER_H
#define REPORTEDTEMPERATURECONTROLREGISTER_H

#define F3xA4_REPORTED_TEMPERATURE_CONTROL_REGISTER 0xA4
/** 31116 Rev 3.62 - January 11, 2013 AMD Family 10h Processor BKDG
 *    F3xA4 Reported Temperature Control Register :
 *    "31:21 CurTmp: current temperature"
 * 
 *  41256 Rev 3.00 - July 07, 2008 AMD Family 11h Processor BKDG 
 *    F3xA4 Reported Temperature Control Register  
 *    "31:21 CurTmp[10:0]: current temperature"
 * 
 *  "42301 Rev 3.14 - January 23, 2013 BKDG for AMD Family 15h Models 00h-0Fh
 *     Processors" 
 *    "31:21 CurTmp: current temperature"  */
#define AMD_FROM_K10_CURTMP_START_BIT 21
/** 31116 Rev 3.62 - January 11, 2013  AMD Family 10h Processor BKDG
 *   F3xA4 Reported Temperature Control Register
 *   "17:16 CurTmpSel."
 * 
 *  "42301 Rev 3.14 - January 23, 2013 BKDG for AMD Family 15h Models 00h-0Fh"
 *   "17:16 CurTmpTjSel:"
 *   "Tj" = "Junction Temperature" ?
 */
#define AMD_FROM_K10_CURR_TEMP_TJ_SELECT_START_BIT 16

#endif /* REPORTEDTEMPERATURECONTROLREGISTER_H */

