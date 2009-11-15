//Include "xerces...\include" in the "include path" for the following "#include"s.
#ifdef WIN32 //with Linux I can't compile with xerces yet.
  //#define COMPILE_WITH_XERCES
#endif
#ifdef COMPILE_WITH_XERCES
#include <xercesc/sax2/DefaultHandler.hpp>
#include <ModelData/PStates.h>
#include "global.h" //for DEBUG(...) etc.
#include <xercesc/sax2/Attributes.hpp> //for "xercesc_2_8::Attributes"
#include <string> //for std::string
#include <ModelData/ModelData.hpp>
#include <UserInterface.hpp>
//#include "../Controller/PumaStateCtrl.h"
#include <Controller/AMD/Griffin/GriffinController.hpp>

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
namespace Griffin
{
//This SAX2 handler is specific for AMD Griffin (e.g. Pentium Ms do
//not have a divisor ID or special performance state MSRs.
class SAX2MainConfigHandler
  : public DefaultHandler 
  //ContentHandler
{
private:
  //PStates * m_p_pstates ;
  Model * m_p_model ;
  GriffinController * mp_pumastatectrl ;
  std::string m_strProcessorName ;
  std::string m_strElementName ;
  UserInterface * m_p_userinterface ;

public:
  //MySAX2Handler(PStates & pstates);
  SAX2MainConfigHandler(
      Model & m_model,
      UserInterface * p_userinterface, 
      GriffinController * mp_pumastatectrl
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

  void handleFreqAndLowestStableVoltageElement(
    const   //xercesc_2_8::
      Attributes & attrs
    ) ;

  void handleFreqAndMaxVoltageElement(
    const   //xercesc_2_8::
    Attributes & attrs
    );
};
}; //namespace Griffin

#endif//COMPILE_WITH_XERCES
