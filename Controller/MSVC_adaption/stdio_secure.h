/* 
 * File:   stdio_secure.h
 * Author: Stefan
 *
 * Created on 4. April 2009, 18:47
 */

#ifndef _STDIO_SECURE_H
#define	_STDIO_SECURE_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdio.h> //for FILE

  //errno_t
  int fopen_s(
    FILE ** pp_file
    , const char * cp_chPath
    , const char * cp_chMode
    ) ;


#ifdef	__cplusplus
}
#endif

#endif	/* _STDIO_SECURE_H */

