/*
 * InstableCPUcoreVoltageDefintions.h
 *
 *  Created on: 29.10.2011
 *      Author: Stefan
 */

#ifndef INSTABLECPUCOREVOLTAGEDEFINTIONS_H_
#define INSTABLECPUCOREVOLTAGEDEFINTIONS_H_

typedef void (* voidFunctionPointer)();

typedef voidFunctionPointer StopInstableVoltageDetectionFunctionPointer;

struct external_caller
{
  voidFunctionPointer m_pfnVoltageTooLow;
};

typedef void (* StartInstableVoltageDetectionFunctionPointer)(//unsigned char,
    uint32_t ui32CPUcoreMask,
    struct external_caller *);

#define START_INSTABLE_CPU_CORE_VOLTAGE_DETECTION_FCT_NAME \
  "StartInstableCPUcoreVoltageDetection"
#define STOP_INSTABLE_CPU_CORE_VOLTAGE_DETECTION_FCT_NAME \
  "StopInstableCPUcoreVoltageDetection"

#endif /* INSTABLECPUCOREVOLTAGEDEFINTIONS_H_ */
