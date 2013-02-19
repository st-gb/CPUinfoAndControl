/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
/*
 * AssignPointersToExportedExeMSRfunctions.cpp
 * (use extension ".cpp" in order to use the C++ include folders from eclipse)
 *  Created on: Sep 3, 2010
 *      Author: Stefan
 */
#include <Controller/ExportedExeFunctions.h> //ReadMSR_func_type etc.
#include <preprocessor_macros/logging_preprocessor_macros.h> //DEBUGN()

#include "AssignPointersToExportedExeMSRfunctions_inline.h"

#ifdef __linux__
  #include <dlfcn.h> //for dlopen(...)
  #include <stddef.h> //for "NULL"
#endif

ReadMSR_func_type g_pfnreadmsr ;
WriteMSR_func_type g_pfn_write_msr ;

void AssignPointersToExportedExeMSRfunctions(
  ReadMSR_func_type & readmsr_func_type ,
  WriteMSR_func_type & writemsr_func_type )
{
//  //Adapted from http://linux.die.net/man/3/dlopen
//  void * p_vModuleHandle = dlopen(
//    //http://linux.die.net/man/3/dlopen:
//    //"If filename is NULL, then the returned handle is for the main program."
//    NULL
//    , RTLD_LAZY
//    ) ;
//  DEBUGN("p_vModuleHandle:" << p_vModuleHandle )
//  if( p_vModuleHandle )
//  {
//    //from http://linux.die.net/man/3/dlopen
//    char * pchError;
//    dlerror();    /* Clear any existing error */
//    readmsr_func_type = (ReadMSR_func_type) dlsym(p_vModuleHandle, "ReadMSR");
//    DEBUGN("readmsr_func_type:" << readmsr_func_type )
//    if ( ( pchError = dlerror() ) )
//    {
//      DEBUGN ( "Error assigning function pointer:" << pchError );
//    }
//    writemsr_func_type = (WriteMSR_func_type) dlsym(p_vModuleHandle, "WriteMSR");
//  }
//  else
//  {
//    readmsr_func_type = NULL ;
//    writemsr_func_type = NULL ;
//  }
  AssignPointersToExportedExeMSRfunctions_inline(readmsr_func_type,
    writemsr_func_type);
}
