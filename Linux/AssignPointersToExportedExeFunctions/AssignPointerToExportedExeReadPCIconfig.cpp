/*
 * AssignPointerToExportedExeReadPCIconfig.cpp
 *
 *  Created on: Sep 25, 2012
 *      Author: sgebauer
 */

#include <Controller/ExportedExeFunctions.h> //ReadMSR_func_type etc.
#include <preprocessor_macros/logging_preprocessor_macros.h> //DEBUGN()
#ifdef __linux__
  #include <dlfcn.h> //for dlopen(...)
  #include <stddef.h> //for "NULL"
#endif

void AssignPointerToExportedExeReadPCIconfig(
  ReadPCIconfigSpace_func_type & pfn_readpciconfigspace_func
  )
{
//  pfn_readpciconfigspace_func = (ReadPCIconfigSpace_func_type)
//  ::GetProcAddress(
//    ::GetModuleHandle(NULL),
//    "ReadPCIconfigSpace");
  //Adapted from http://linux.die.net/man/3/dlopen
  void * p_vModuleHandle = dlopen(
    //http://linux.die.net/man/3/dlopen:
    //"If filename is NULL, then the returned handle is for the main program."
    NULL
    , RTLD_LAZY
    ) ;
//  DEBUGN("p_vModuleHandle:" << p_vModuleHandle )
  if( p_vModuleHandle )
  {
    //from http://linux.die.net/man/3/dlopen
    char * pchError;
    dlerror();    /* Clear any existing error */
    pfn_readpciconfigspace_func = (ReadPCIconfigSpace_func_type) dlsym(
      p_vModuleHandle, "ReadPCIconfigSpace");
//    DEBUGN("pfn_readpciconfigspace_func:" << pfn_readpciconfigspace_func )
    if ( ( pchError = dlerror() ) )
    {
      DEBUGN ( "Error assigning function pointer:" << pchError );
    }
  }
  else
  {
    pfn_readpciconfigspace_func = NULL ;
  }
}
