/*
 * BuildTimeString.cpp
 *
 *  Created on: Sep 10, 2013
 *      Author: stefan
 */

//#include <preprocessor_macros/BuildTimeString.h> //BUILT_TIME

/** Touch this file in a pre-build step in order to ensure the time is updated. */
///"By "const": avoid "warning: ISO C++ forbids converting a string constant 
/// to ‘char*’ [-Wwrite-strings]"
const char * g_ar_chBuildTimeString = //BUILT_DATE_AND_TIME;
  __DATE__ " " __TIME__ " GMT+1";
  //BUILT_TIME;
//  ;


