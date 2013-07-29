/*
 * CPU_core_temperature_defs.h
 *
 *  Created on: 07.08.2012
 *      Author: Stefan
 */

#ifndef CPU_CORE_TEMPERATURE_DEFS_H_
#define CPU_CORE_TEMPERATURE_DEFS_H_

//see http://en.wikipedia.org/wiki/Kelvin
#define O_KELVIN_IN_DEG_CELSIUS -273.15
//http://de.wikipedia.org/wiki/Gleitkommazahl#IEEE_754_und_andere_Normen:
//IEEE 754: mantissa: 23 bit, exponent: 8 bit
#define CPU_TEMP_IS_BELOW_PROCHOT -290.0f
#define CPU_TEMP_IS_PROCHOT -285.0f
#define CPU_TEMP_IS_BELOW_CRITICAL -300.0f
#define CPU_TEMP_IS_CRITICAL -280.0f

#endif /* CPU_CORE_TEMPERATURE_DEFS_H_ */
