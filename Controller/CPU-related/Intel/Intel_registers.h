/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
#pragma once //incl. guard
//Copyright 2010 by Trilobyte SE GmbH, Berlin, Germany

//Architectural(=for a bunch of Intel CPU types/ models) MSRs follow

//Intel Volume 3A System Programming Guide (document number "253668"):
//"For Pentium M processors (family [06H], models [09H, 0DH]); for Pentium 4
//processors, Intel Xeon processors (family [0FH], models [00H, 01H, or 02H]);
//and for P6 family processors: the time-stamp counter increments with every
//internal processor clock cycle.
//The internal processor clock cycle is determined by the current core-clock to bus-
//clock ratio. Intel® SpeedStep® technology transitions may also impact the
//processor clock."

//-> if 600MHz for 1 second, 1800 MHz the next second 600M+1800M=2400M 
// time-stamp clocks have passed.

//Intel Volume 3A System Programming Guide (document number "253668"):
//"For Pentium 4 processors, Intel Xeon processors (family [0FH], models [03H and
//higher]); for Intel Core Solo and Intel Core Duo processors (family [06H], model
//[0EH]); for the Intel Xeon processor 5100 series and Intel Core 2 Duo processors
//(family [06H], model [0FH]); for Intel Core 2 and Intel Xeon processors (family
//[06H], display_model [17H]); for Intel Atom processors (family [06H],
//display_model [1CH]): the time-stamp counter increments at a constant rate.
//That rate may be set by the maximum core-clock to bus-clock ratio of the
//processor or may be set by the maximum resolved frequency at which the
//processor is booted. The maximum resolved frequency may differ from the
//maximum qualified frequency of the processor, see Section 30.10.5 for more
//detail."
#define IA32_TIME_STAMP_COUNTER 0x10 //16 decimal

//Intel(R) 64 and IA-32 Architectures Software Developer's Manual, Volume 3B
//chapter 30.8.2 Performance Counters (Pentium 4):
//"Each performance counter is 40-bits wide (see Figure30-24)"
//number of performance counters: CPUID.0AH:EAX[15:8]
#define IA32_MISC_ENABLE 0x1A0 //416
#define IA32_PMC0 0xC1 //193 decimal, (PERFCTR0);General Performance Counter 0 (R/W)
#define IA32_PMC1 0xC2 //194 decimal, (PERFCTR1);General Performance Counter 0 (R/W)
#define IA32_PMC2 0xC3 //195 decimal, General Performance Counter 0 (R/W)
#define IA32_PMC3 0xC4 //196 decimal, General Performance Counter 0 (R/W)
//Performance Event Select Register 0 (R/W):available If CPUID.0AH: EAX[15:8] > 0
#define IA32_PERFEVTSEL0 0x186 //390; (PERFEVTSEL0) 
//Performance Event Select Register 1 (R/W):available If CPUID.0AH: EAX[15:8] > 1
#define IA32_PERFEVTSEL1 0x187 //391; (PERFEVTSEL1) 
//Performance Event Select Register 2 (R/W):available If CPUID.0AH: EAX[15:8] > 2
#define IA32_PERFEVTSEL2 0x188 //392; (PERFEVTSEL2) 
//Performance Event Select Register 3 (R/W):available If CPUID.0AH: EAX[15:8] > 3
#define IA32_PERFEVTSEL3 0x189 //393; (PERFEVTSEL3) 
#define IA32_PERF_STATUS 0x198 //408 decimal,(ReadOnly);since fam. 0F, model 3
#define IA32_PERF_CTL 0x199 //409 (Read/Write); since family 0F, model 03H
//Thermal Status Information (ReadOnly)
#define IA32_THERM_STATUS 0x19C //412 

//Critical Temperature Status (bit 4, RO): 1 << 4 = 2^4 = 16dec = 10000bin
#define CRITICAL_TEMPERATURE_STATUS_BIT_MASK 16
//Thermal Threshold #1 Status (bit 6, RO): 1 << 6 = 2^6 = 64dec = 1000000bin
#define THERMAL_THRESHOLD_1_STATUS_BIT_MASK 64
//#define CPU_CLOCKS_NOT_HALTED 121

#define MSR_TEMPERATURE_TARGET 0x1A2
#define MSR_TURBO_RATIO_LIMIT 0x1AD

//MPERF/ APERF = CPU load ?
#define IA32_MPERF //Maximum Qualified Performance Clock Counter (R/Write to clear)
#define IA32_APERF //Actual Performance Clock Counter (R/Write to clear)

//Pentium M:
#define INTEL_ARCHITECTURAL_CPU_CLOCKS_NOT_HALTED 121 //0x79
//"Number of Enhanced Intel SpeedStep technology transitions:"
#define EMON_EST_TRANS 0x58
//"Mask = 02H - Only Frequency transitions"
#define ONLY_FREQUENCY_TRANSITIONS_MASK 0x2

//Intel Core:
#define MSR_FSB_FREQ 0xCD

//Intel Volume 3B System Programming Guide (document number "253669") 
//appendix A.1, Table A-1:  event number
//Performance Counter Events (
#define UnHalted_Core_Cycles 0x3C //=60dec

//30.10.5 Cycle Counting and Opportunistic Processor Operation

#define LAST_LEVEL_CACHE_MISSES_EVENT_SELECT 0x2E
#define LAST_LEVEL_CACHE_MISSES_UMASK 0x41
#define LAST_LEVEL_CACHE_REFERENCES_EVENT_SELECT 0x2E
#define LAST_LEVEL_CACHE_REFERENCES_UMASK 0x4F

#define MSR_EBC_FREQUENCY_ID 0x2C

