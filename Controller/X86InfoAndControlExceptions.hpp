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
