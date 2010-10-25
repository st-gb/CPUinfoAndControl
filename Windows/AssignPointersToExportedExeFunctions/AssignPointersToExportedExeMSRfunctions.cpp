/*
 * AssignPointersToExportedExeFunctions.c
 *
 *  Created on: Jun 4, 2010
 *      Author: Stefan
 * Copyright 2010 by Trilobyte SE GmbH, Berlin, Germany
 */

#include <windows.h> //for GetProcAddress(...)
#include <Controller/ExportedExeFunctions.h> //ReadMSR_func_type etc.

ReadMSR_func_type g_pfnreadmsr ;
WriteMSR_func_type g_pfn_write_msr ;

void AssignPointersToExportedExeMSRfunctions(
  ReadMSR_func_type & pfn_read_msr_func_type ,
  WriteMSR_func_type & pfn_write_msr_func_type
  )
{
  //from http://www.codeguru.com/cpp/w-p/dll/article.php/c3649
  //("Calling an Exported Function in an EXE from Within a DLL"):
  pfn_read_msr_func_type = (ReadMSR_func_type)::GetProcAddress(
    ::GetModuleHandle(NULL),
    "ReadMSR");
  pfn_write_msr_func_type = (WriteMSR_func_type)::GetProcAddress(
    ::GetModuleHandle(NULL),
    "WriteMSR");
}
