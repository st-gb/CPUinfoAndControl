#include "stdio_secure.h"

  //errno_t
  int fopen_s(
    FILE ** pp_file
    , const char * cp_chPath
    , const char * cp_chMode
    )
  {
    //errno_t
    int errnoReturn = 1;
    *pp_file = fopen(cp_chPath,cp_chMode) ;
    //If not NULL (=success)
    if( pp_file )
      errnoReturn = 0 ;
    return errnoReturn ;
  }

