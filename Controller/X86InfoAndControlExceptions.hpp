/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
#pragma once

#include <Controller/character_string/stdtstr.hpp>

class VoltageSafetyException
{
public:
  std::tstring m_stdtstrMessage ;
  VoltageSafetyException( const std::tstring & cr_stdtstr )
  {
    m_stdtstrMessage = cr_stdtstr ;
  }
} ;
