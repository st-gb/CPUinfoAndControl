/*
 * Griffin.cpp
 *
 *  Created on: Jul 11, 2010
 *      Author: Stefan
 * functions that are used by I_CPUcontroller-derived class and by DLL
 */
//#include <global.h> //for DEBUGN
//#include <Controller/CPU-related/AMD/Griffin/AMD_family17.h>
//#include <preprocessor_helper_macros.h>
//#include "Griffin.h"
#include <windef.h> //BYTE, DWORD

//max. CPU core frequency ID -> max. freq= g_byMainPllOpFreqIdMax* ref. clock
//BYTE g_byMainPllOpFreqIdMax ;
BYTE g_byFreqID,g_byDivisorID ;
BYTE g_byDivisor ;
DWORD g_dwMSRhighmost, g_dwMSRlowmost ;
//MSRC001_0071 COFVID Status Register
//54:49 MainPllOpFreqIdMax: main PLL operating frequency ID maximum.
float g_fMainPllOpFreqIdMax ;
float g_fMainPllOpFreqId ;
float g_fMainPllOpFreqIdMaxDiv2 ;
float g_fMaxMultiDiv2 ;
float g_fValue1 ;


