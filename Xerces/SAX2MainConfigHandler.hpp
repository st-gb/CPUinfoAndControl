//#ifdef COMPILE_WITH_XERCES
//#include <global.h> //for DEBUG(...) etc.

//Include "xerces...\include" in the "include path" for the following "#include"s.
////class XERCES_CPP_NAMESPACE::Attributes
//#include <xercesc/sax2/Attributes.hpp>
//Base class XERCES_CPP_NAMESPACE::DefaultHandler
#include <xercesc/sax2/DefaultHandler.hpp>

#include <string> //for std::string

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
class I_CPUcontroller ;
class Model ;
class UserInterface ;
// need to properly scope any forward declarations
XERCES_CPP_NAMESPACE_BEGIN
  class Attributes;
XERCES_CPP_NAMESPACE_END

class SAX2MainConfigHandler
  : public XERCES_CPP_NAMESPACE::DefaultHandler
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
  void fatalError(const XERCES_CPP_NAMESPACE::SAXParseException & );
  void HandleDynamicVoltage_and_FrequencyScaling(
    const XERCES_CPP_NAMESPACE::Attributes & attrs ) ;
  void HandleTopmostXMLelement(
    const XERCES_CPP_NAMESPACE::Attributes & attrs ) ;
  void startElement(
    const XMLCh * const cpc_xmchURI,
    const XMLCh * const cpc_xmchLocalName,
    const XMLCh * const cpc_xmchQualifiedName,
    const XERCES_CPP_NAMESPACE::Attributes & attrs
  );
};
//#endif//COMPILE_WITH_XERCES
