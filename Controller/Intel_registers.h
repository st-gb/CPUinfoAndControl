
//Architectural(=for a bunch of Intel CPU types/ models) MSRs follow

//Intel Volume 3A System Programming Guide (document number "253668"):
//For Pentium M processors (family [06H], models [09H, 0DH]); for Pentium 4
//processors, Intel Xeon processors (family [0FH], models [00H, 01H, or 02H]);
//and for P6 family processors: the time-stamp counter increments with every
//internal processor clock cycle.
//The internal processor clock cycle is determined by the current core-clock to bus-
//clock ratio. Intel® SpeedStep® technology transitions may also impact the
//processor clock.
//-> if 600MHz for 1 second, 1800 MHz the next second 600M+1800M=2400M 
// time-stamp clocks have passed.

//For Pentium 4 processors, Intel Xeon processors (family [0FH], models [03H and
//higher]); for Intel Core Solo and Intel Core Duo processors (family [06H], model
//[0EH]); for the Intel Xeon processor 5100 series and Intel Core 2 Duo processors
//(family [06H], model [0FH]); for Intel Core 2 and Intel Xeon processors (family
//[06H], display_model [17H]); for Intel Atom processors (family [06H],
//display_model [1CH]): the time-stamp counter increments at a constant rate.
//That rate may be set by the maximum core-clock to bus-clock ratio of the
//processor or may be set by the maximum resolved frequency at which the
//processor is booted. The maximum resolved frequency may differ from the
//maximum qualified frequency of the processor, see Section 30.10.5 for more
//detail.
#define IA32_TIME_STAMP_COUNTER 0x10 //16 decimal

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
#define IA32_PERF_STATUS 0x198 //408 decimal, (ReadOnly), gibt es seit 0F_03H
#define IA32_PERF_CTL 0x199 //409 (Read/Write); gibt es seit 0F_03H 
//Thermal Status Information (ReadOnly)
#define IA32_THERM_STATUS 0x19C //412 
//#define CPU_CLOCKS_NOT_HALTED 121

//MPERF/ APERF = CPU load ?
#define IA32_MPERF //Maximum Qualified Performance Clock Counter (R/Write to clear)
#define IA32_APERF //Actual Performance Clock Counter (R/Write to clear)

//Pentium M:
#define INTEL_ARCHITECTURAL_CPU_CLOCKS_NOT_HALTED 121 //0x79

//Intel Volume 3B System Programming Guide (document number "253669") 
//appendix A.1, Table A-1:  event number
//Performance Counter Events (
#define UnHalted_Core_Cycles 0x3C

//30.10.5 Cycle Counting and Opportunistic Processor Operation

#define LAST_LEVEL_CACHE_MISSES_EVENT_SELECT 0x2E
#define LAST_LEVEL_CACHE_MISSES_UMASK 0x41
#define LAST_LEVEL_CACHE_REFERENCES_EVENT_SELECT 0x2E
#define LAST_LEVEL_CACHE_REFERENCES_UMASK 0x4F
