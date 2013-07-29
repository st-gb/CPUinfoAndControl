/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
/*
 * AssignPointersToExportedExeMSRfunctions.h
 *
 *  Created on: 30.08.2010
 *      Author: sgebauer
 */

#ifndef ASSIGNPOINTERSTOEXPORTEDEXEMSRFUNCTION_INLINE_H_
#define ASSIGNPOINTERSTOEXPORTEDEXEMSRFUNCTIONS_INLINE_H_

#ifdef __linux__
  #include <dlfcn.h> //for dlopen(...)
  #include <stddef.h> //NULL
#endif
#include <preprocessor_macros/logging_preprocessor_macros.h> //DEBUGN()

//http://z505.com/cgi-bin/qkcont/qkcont.cgi?p=Export%20Functions%20from%20Executables:

inline void AssignPointersToExportedExeMSRfunctions_inline(
  ReadMSR_func_type & readmsr_func_type ,
  WriteMSR_func_type & writemsr_func_type )
{
  //Adapted from http://linux.die.net/man/3/dlopen
  void * p_vModuleHandle = dlopen(
    //http://linux.die.net/man/3/dlopen:
    //"If filename is NULL, then the returned handle is for the main program."
    NULL
    , RTLD_LAZY
    ) ;
  DEBUGN("p_vModuleHandle:" << p_vModuleHandle )
  if( p_vModuleHandle )
  {
    //from http://linux.die.net/man/3/dlopen
    char * pchError;
    dlerror();    /* Clear any existing error */
    readmsr_func_type = (ReadMSR_func_type) dlsym(p_vModuleHandle, "ReadMSR");
    DEBUGN("readmsr_func_type:" << readmsr_func_type )
    if ( ( pchError = dlerror() ) )
    {
      DEBUGN ( "Error assigning function pointer:" << pchError );
    }
    writemsr_func_type = (WriteMSR_func_type) dlsym(p_vModuleHandle, "WriteMSR");
  }
  else
  {
    readmsr_func_type = NULL ;
    writemsr_func_type = NULL ;
  }
}

#endif /* ASSIGNPOINTERSTOEXPORTEDEXEMSRFUNCTIONS_INLINE_H_ */
