//Include "xerces...\include" in the "include path" for the following "#include"s.
#ifdef WIN32 //with Linux I can't compile with xerces yet.
  //#define COMPILE_WITH_XERCES
#endif
#ifdef COMPILE_WITH_XERCES
#include <global.h> //for DEBUG(...) etc.
#include <ModelData/ModelData.hpp>

#include <xercesc/sax2/Attributes.hpp> //for "xercesc_2_8::Attributes"
#include <xercesc/sax2/DefaultHandler.hpp>

#include <string> //for std::string

//Needed for verzichten auf the exact namspace.
XERCES_CPP_NAMESPACE_USE

//http://xerces.apache.org/xerces-c/build-3.html:
//If you are linking your application to the static Xerces-C++ library,
//then you will need to compile your application with the
//XERCES_STATIC_LIBRARY
//preprocessor macro defined in order to turn off the DLL
//import/export mechanism.
//#define XERCES_STATIC_LIBRARY

//#define MB_CUR_MAX 1 

//Forward declaration (because _this_ header file may be included very often /
//more than once) is faster than to #include the while declaration file.
class UserInterface ;

class SAX2MainConfigHandler
  : public DefaultHandler 
  //ContentHandler
{
private:
  Model * m_p_model ;
  I_CPUcontroller * mp_cpucontroller ;
  std::string m_strProcessorName ;
  std::string m_strElementName ;
  UserInterface * m_p_userinterface ;

public:
  SAX2MainConfigHandler(
      Model & m_model,
      UserInterface * p_userinterface //,
      //I_CPUcontroller * p_cpucontroller
      );

    //MySAX2Handler(const MySAX2Handler & mysax2){};
  void startElement(
      const   XMLCh* const    uri,
      const   XMLCh* const    localname,
      const   XMLCh* const    qname,
      const   Attributes&     attrs
  );
  void fatalError(const SAXParseException&);

  void handlePstateElement(  
    const   //xercesc_2_8::
    Attributes & attrs
    ) ;

  void HandleDynamicVoltage_and_FrequencyScaling(const Attributes & attrs) ;

  void handleFreqAndLowestStableVoltageElement(
    const   //xercesc_2_8::
      Attributes & attrs
    ) ;

  void handleFreqAndMaxVoltageElement(
    const   //xercesc_2_8::
    Attributes & attrs
    );
  void HandleTopmostXMLelement(const Attributes & attrs) ;
};
#endif//COMPILE_WITH_XERCES

