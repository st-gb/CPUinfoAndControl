/* 
 * File:   specstrings_strict.h
 * Author: Stefan
 *
 * Created on 13. November 2009, 22:43
 */

#ifndef _SPECSTRINGS_STRICT_H
#define	_SPECSTRINGS_STRICT_H

#ifdef	__cplusplus
extern "C" {
#endif

//  #define __allowed(p) __$allowed_##p
//  #define __success(expr)          __allowed(on_function_or_typedecl)
  #define __success(expr)

  //from VS2005 specstring_strict.h:
  // __in_opt -> __allowed(on_parameter) -> __$allowed_on_parameter
  //   #define __$allowed_on_parameter /* empty */
  #define __in_opt /* empty */
  #define __out_opt /* empty */
  #define __in /* empty */
  #define __inout /* empty */
  //from VS2005 specstring_strict.h:
  //#define __deref_out                                   __allowed(on_parameter)
  #define __deref_out /* empty */
  //#define __out                                         __allowed(on_parameter)
  #define __out /* empty */
#ifdef	__cplusplus
}
#endif

#endif	/* _SPECSTRINGS_STRICT_H */

