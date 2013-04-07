/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
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
  //see http://groups.google.com/group/comp.programming/browse_thread/thread/
  //  4275d11ee0c2e4c7/3e571b1c164d0261?
  //  hl=en&ie=UTF-8&q=export+function+executable#3e571b1c164d0261
  //: "and then the dynamic library can use
  //  GetProcAddress(GetModuleHandle(NULL),"name_of_exported_function") ."
  pfn_read_msr_func_type = (ReadMSR_func_type)::GetProcAddress(
    ::GetModuleHandle(NULL),
    "ReadMSR");
  pfn_write_msr_func_type = (WriteMSR_func_type)::GetProcAddress(
    ::GetModuleHandle(NULL),
    "WriteMSR");
}
