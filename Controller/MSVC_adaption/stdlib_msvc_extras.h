/* 
 * File:   stdlib_mscv_extras.h
 * Author: Stefan
 *
 * Created on 4. April 2009, 19:03
 */

#ifndef _STDLIB_MSCV_EXTRAS_H
#define	_STDLIB_MSCV_EXTRAS_H

#undef min
#undef max
#ifdef __CYGWIN__

  //"stdlib.h" 's "__max" preprocessor macro that is not available under gcc.
  #define __max(a,b)     (( (a) > (b) ) ? (a) : (b) )

  //"stdlib.h" 's "__max" preprocessor macro that is not available under gcc.
  #define __min(a,b)     (( (a) < (b) ) ? (a) : (b) )

//namespace std
//{
//    min
//}



#ifdef	__cplusplus
extern "C" {
#endif

int getche(void) ;

#ifdef	__cplusplus
}
#endif

#endif //#ifdef __CYGWIN__

#endif	/* _STDLIB_MSCV_EXTRAS_H */

