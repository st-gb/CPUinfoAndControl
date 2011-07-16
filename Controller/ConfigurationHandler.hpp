/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */

//Base class for all classes that implement the loading (and saving of
//configuration files:
//This is the purpose of this class: e.g. Linux wasn't able to compile 
//with xerces as for loading the config file.
//But it is also senseful if using the wxWidgets XML instead of Xerces
//in order to save space in the binary files.
class IConfigurationHandler
{
};
