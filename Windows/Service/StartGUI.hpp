/*
 * StartGUI.hpp
 *
 *  Created on: 23.09.2011
 *      Author: Stefan
 */

#ifndef STARTGUI_HPP_
#define STARTGUI_HPP_

class ServiceAttributes;

void
StartGUIprocessDelayedAsync(ServiceAttributes & r_service_attributes,
    DWORD dwSessionID);

#endif /* STARTGUI_HPP_ */
