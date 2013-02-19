/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
/*
 * daemon_functions.hpp
 *
 *  Created on: Dec 31, 2010
 *      Author: sgebauer
 */

#ifndef DAEMON_FUNCTIONS_HPP_
#define DAEMON_FUNCTIONS_HPP_

#include <string> //class std::string

//template<typename charType> class CommandLineArgs
#include <Process/CommandLineArgs.hpp>

void init_daemon(//int argc, char *  argv[],
  std::string & stdstrCurrentWorkingDir ) ;

#endif /* DAEMON_FUNCTIONS_HPP_ */
