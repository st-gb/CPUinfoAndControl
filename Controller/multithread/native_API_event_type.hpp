/*
 * native_API_event_type.hpp
 *
 *  Created on: Aug 11, 2010
 *      Author: Stefan
 */

#ifndef NATIVE_API_EVENT_TYPE_HPP_
#define NATIVE_API_EVENT_TYPE_HPP_

class Win32EventBasedCondition ;

namespace x86IandC
{
#ifdef _WINDOWS
  typedef Win32EventBasedCondition native_API_event_type ;
#endif
}

#endif /* NATIVE_API_EVENT_TYPE_HPP_ */
