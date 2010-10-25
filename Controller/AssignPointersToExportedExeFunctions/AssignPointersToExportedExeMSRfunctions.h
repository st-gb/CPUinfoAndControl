/*
 * AssignPointersToExportedExeMSRfunctions.hpp
 *
 *  Created on: 30.08.2010
 *      Author: Stefan
 */

#ifndef ASSIGNPOINTERSTOEXPORTEDEXEMSRFUNCTIONS_H_
#define ASSIGNPOINTERSTOEXPORTEDEXEMSRFUNCTIONS_H_

#include <Controller/ExportedExeFunctions.h> //ReadMSR_func_type etc.

//ReadMSR_func_type g_pfnreadmsr ;
//WriteMSR_func_type g_pfn_write_msr ;

//#ifdef _WIN32 //Built-in macro for MSVC, MinGW (also for 64 bit Windows)
//  #include<Windows/AssignPointersToExportedExeMSRfunctions.h>
//#endif
//#ifdef __linux__
//  #include<Linux/AssignPointersToExportedExeMSRfunctions.h>
//#endif

//Implementation/ definition of this function should be in a source file within
//the appropriate folder ( "[...]/Windows[...]" / "[...]/Linux[...]" etc.)
void AssignPointersToExportedExeMSRfunctions(
  ReadMSR_func_type & readmsr_func_type ,
  WriteMSR_func_type & writemsr_func_type ) ;

#endif /* ASSIGNPOINTERSTOEXPORTEDEXEMSRFUNCTIONS_H_ */
