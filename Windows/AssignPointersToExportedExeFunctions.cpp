/*
 * AssignPointersToExportedExeFunctions.c
 *
 *  Created on: Jun 4, 2010
 *      Author: Stefan
 * Copyright 2010 by Trilobyte SE GmbH, Berlin, Germany
 */

#include <Controller/ExportedExeFunctions.h> //ReadMSR_func_type etc.

#include <windows.h>

  ReadMSR_func_type g_pfnreadmsr ;
  WriteMSR_func_type g_pfn_write_msr ;

  void AssignPointersToExportedExeFunctions()
  {
    //from http://www.codeguru.com/cpp/w-p/dll/article.php/c3649
    //("Calling an Exported Function in an EXE from Within a DLL"):
    g_pfnreadmsr = (ReadMSR_func_type)::GetProcAddress(
      ::GetModuleHandle(NULL),
      "ReadMSR");
    g_pfn_write_msr = (WriteMSR_func_type)::GetProcAddress(
      ::GetModuleHandle(NULL),
      "WriteMSR");
  }

