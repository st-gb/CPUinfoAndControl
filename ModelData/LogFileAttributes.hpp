/*
 * LogFileAttributes.hpp
 *
 *  Created on: 19.10.2012
 *      Author: Stefan
 */

#ifndef LOGFILEATTRIBUTES_HPP_
#define LOGFILEATTRIBUTES_HPP_

#include <string> //class std::string

class LogFileAttributes
{
public:
  bool m_bAppendProcessID ;
  unsigned m_maxNumberOfLines;
  std::wstring m_std_wstrLogFilePath;
  std::string m_std_strFormat;
  std::string m_std_strLevel;
  std::string m_std_strTimeFormat;
//  bool m_bTruncateLogFileForEveryStartup ;

  LogFileAttributes();
  virtual
  ~LogFileAttributes();
};

#endif /* LOGFILEATTRIBUTES_HPP_ */
