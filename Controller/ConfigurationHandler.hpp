
//Base class for all classes that implement the loading (and saving of
//configuration files:
//This is the purpose of this class: e.g. Linux wasn't able to compile 
//with xerces as for loading the config file.
//But it is also sensful if using the wxWidgets XML instead of Xerces 
//in order to save space in the binary files.
class IConfigurationHandler
{
};
