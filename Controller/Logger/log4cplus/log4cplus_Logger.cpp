/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
/*
 * log4cplus_Logger.cpp
 *
 *  Created on: Jul 9, 2010
 *      Author: Stefan
 */

#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/helpers/stringhelper.h>

using namespace std;
using namespace log4cplus;
using namespace log4cplus::helpers;


//static
Logger log4cplus_logger = Logger::getInstance(LOG4CPLUS_TEXT("log"));

void init_log4cplus()
{
  //from log4cplus's test_filter test
  LogLog::getLogLog()->setInternalDebugging(true);
  Logger root = Logger::getRoot();
  try {
      PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT("log4cplus.properties"));

//      LOG4CPLUS_WARN(root, "Testing....");
//      printDebug();
  }
  catch(...)
  {
//      cout << "Exception..." << endl;
      LOG4CPLUS_FATAL(root, "Exception occured...");
  }
}
