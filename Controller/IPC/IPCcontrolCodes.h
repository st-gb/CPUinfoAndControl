/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
/*
 * IPCcontrolCodes.h
 *
 *  Created on: 20.02.2011
 *      Author: Stefan
 */

#ifndef IPCCONTROLCODES_H_
#define IPCCONTROLCODES_H_

enum IPCcontrolCodes
{
  pause_DVFS = 0
  , start_DVFS
  , stop_DVFS
  , continue_DVFS
  , setVoltageAndFrequency
  , stop_service
  , pause_service
  , continue_service
  , copy_attribute_data
  , get_CPU_load
  //The DVFS state can't be determined as normal user (not admin) on WinXP.
  //So make it available via IPC from the DVFS service.
  , other_DVFS_is_enabled
  , disable_other_DVFS
  , enable_other_DVFS
  , sync_GUI_show_data_with_service
  //This should get the current data like temperature, CPU core load, voltage,
  // frequency.
  , get_current_CPU_data
  //Gets min/ max/ wanted voltages (etc.) from the service.
  , get_configuration_data
  , setCPUcoreThrottleTemperature
  , setMaximumCPUcoreMultplier
  , getAvailablePowerSchemeNames
  , setActivePowerScheme
};

#endif /* IPCCONTROLCODES_H_ */
