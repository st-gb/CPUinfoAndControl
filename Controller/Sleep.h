/* 
 * File:   Sleep.h
 * Author: Stefan
 *
 * Created on 14. November 2009, 03:40
 */

#ifndef _SLEEP_H
#define	_SLEEP_H

#ifdef	__cplusplus
extern "C" {
#endif

#ifdef _WINDOWS
  #define SLEEP_1_MILLI_SECOND ::Sleep(1) ;
#endif
  
#ifdef	__cplusplus
}
#endif

#endif	/* _SLEEP_H */

