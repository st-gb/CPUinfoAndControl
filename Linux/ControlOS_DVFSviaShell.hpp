/*
 * ControlOS_DVFSviaShell.hpp
 *
 *  Created on: Mar 20, 2013
 *      Author: stefan
 */

#ifndef CONTROLOS_DVFSVIASHELL_HPP_
#define CONTROLOS_DVFSVIASHELL_HPP_

#include <Controller/IDynFreqScalingAccess.hpp>

class ControlOS_DVFSviaShell
  : public IDynFreqScalingAccess
{
public:
  ControlOS_DVFSviaShell();
  virtual
  ~ControlOS_DVFSviaShell();
};

#endif /* CONTROLOS_DVFSVIASHELL_HPP_ */
