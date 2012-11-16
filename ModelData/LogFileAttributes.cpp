/*
 * LogFileAttributes.cpp
 *
 *  Created on: 19.10.2012
 *      Author: Stefan
 */

#include "ModelData/LogFileAttributes.hpp"

#define LOG_FILE_PATH L"log_files"

LogFileAttributes::LogFileAttributes()
  : m_bAppendProcessID(true)
  , m_std_wstrLogFilePath( LOG_FILE_PATH )
  , m_std_strFormat("html")
  , m_std_strTimeFormat("%year%-%month%-%day%&nbsp;%hour%:%minute%:%second%s"
    "%millisecond%ms")

{
  // TODO Auto-generated constructor stub

}

LogFileAttributes::~LogFileAttributes()
{
  // TODO Auto-generated destructor stub
}

