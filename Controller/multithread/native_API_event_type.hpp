/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
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
