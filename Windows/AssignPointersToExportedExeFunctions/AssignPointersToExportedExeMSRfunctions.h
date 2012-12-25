/*
 * AssignPointersToExportedExeFunctions.h
 *
 *  Created on: Jun 4, 2010
 *      Author: Stefan
 * Copyright 2010 by Trilobyte SE GmbH, Berlin, Germany
 */

#ifndef ASSIGNPOINTERSTOEXPORTEDEXEFUNCTIONS_H_
#define ASSIGNPOINTERSTOEXPORTEDEXEFUNCTIONS_H_

#include <windows.h> //for GetProcAddress(...)
#include <Controller/ExportedExeFunctions.h> //ReadMSR_func_type etc.

ReadMSR_func_type g_pfnreadmsr ;
WriteMSR_func_type g_pfn_write_msr ;

inline void AssignPointersToExportedExeMSRfunctions(
  ReadMSR_func_type & pfn_read_msr_func_type ,
  WriteMSR_func_type & pfn_write_msr_func_type
  )
{
  pfn_read_msr_func_type = (ReadMSR_func_type)::GetProcAddress(
    ::GetModuleHandle(NULL),
    "ReadMSR");
  pfn_write_msr_func_type = (WriteMSR_func_type)::GetProcAddress(
    ::GetModuleHandle(NULL),
    "WriteMSR");
}

#endif /* ASSIGNPOINTERSTOEXPORTEDEXEFUNCTIONS_H_ */
