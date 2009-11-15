
//architectural(=for a buch of Intel CPU types/ models) MSRs follow
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

#define INTEL_ARCHITECTURAL_CPU_CLOCKS_NOT_HALTED 121
#define LAST_LEVEL_CACHE_MISSES_EVENT_SELECT 0x2E
#define LAST_LEVEL_CACHE_MISSES_UMASK 0x41
#define LAST_LEVEL_CACHE_REFERENCES_EVENT_SELECT 0x2E
#define LAST_LEVEL_CACHE_REFERENCES_UMASK 0x4F
