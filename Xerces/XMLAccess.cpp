//#ifdef COMPILE_WITH_XERCES
//If not included: compiler error "C1010" with MSVC++.
#include "../stdafx.h"

#include "XMLAccess.hpp"
#include <global.h> //for SUCCESS, FAILURE
#include <Controller/Logger.hpp>
#include <Controller/stdstring_format.hpp> //for to_strstring()
#include <ModelData/ModelData.hpp>
#include <UserInterface/UserInterface.hpp>
#include <Xerces/XercesHelper.hpp> // for "X()"

#include <iostream>
#include <xercesc/framework/LocalFileInputSource.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
  //#include "SAX2Handler.hpp"

using namespace std;
using namespace xercesc;

extern Logger g_logger ;

char readXMLConfig(
  const char * cp_chXMLfilePath, //PStates & pstates
  Model & model,
  UserInterface * p_userinterface ,
 //Base class of implementing Xerces XML handlers.
 //This is useful because there may be more than one XML file to read.
 //So one calls this functions with different handlers passed.
  XERCES_CPP_NAMESPACE::DefaultHandler & r_defaulthandler
  )
{
  LOG( "read XML configuration--filename: \"" << cp_chXMLfilePath << "\"\n" );
  bool bXercesSuccessfullyInitialized = false ;
  //from http://xerces.apache.org/xerces-c/program-sax2-3.html:
  try
  {
    //http://xerces.apache.org/xerces-c/program-3.html:
    //"Independent of the API you want to use, DOM, SAX, or SAX2, your
    //application must initialize the Xerces system before using the API[...]"
    //Initialize() must be called _before_ any Xerces function call, else SIGSEV
    // /program crash.
    XMLPlatformUtils::Initialize();
    LOG( "XML access successfully initialized\n" );
    bXercesSuccessfullyInitialized = true ;
  }
  catch (const XMLException & toCatch )
  {
    char * p_archMessage = XMLString::transcode(toCatch.getMessage());
    std::ostringstream stdostringstream ;
    stdostringstream << "Xerces Error during initialization! :\n"
      "Exception message is: \n"
         << p_archMessage << "\n" ;
    p_userinterface->Confirm( stdostringstream ) ;
      //DEBUG("XML error:%s\n",message);
//      LOG( "XML error:" << message << "\n" );
    XMLString::release(&p_archMessage);
    return FAILURE;
  }
  if( bXercesSuccessfullyInitialized )
  {
    //  XERCES_CPP_NAMESPACE::LocalFileInputSource xerces_localfileinputsource(
    //    X(cp_chXMLfilePath) ) ;
    XMLCh * p_xmlchXMLfilePath = XMLString::transcode(cp_chXMLfilePath);
    if( p_xmlchXMLfilePath )
    {
      XERCES_CPP_NAMESPACE::LocalFileInputSource xerces_localfileinputsource(
        p_xmlchXMLfilePath ) ;
      readXMLConfig(
        xerces_localfileinputsource ,
        model,
        p_userinterface ,
        r_defaulthandler
        ) ;
      XMLString::release(&p_xmlchXMLfilePath);
    }
    //http://xerces.apache.org/xerces-c/program-3.html:
    //"Independent of the API you want to use, DOM, SAX, or SAX2, your
    //application must [...] and terminate it after you are done.
    //When Terminate() was called in another block (even if in a function that
    //is called in the same block) than program crash.
    XMLPlatformUtils::Terminate();
  }
  return SUCCESS ;
}

char readXMLConfig(
  BYTE arbyXMLdata [] ,
  DWORD dwSizeInByte ,
  const LPWSTR lpwstrBufferIdentifier ,
  Model & model,
  UserInterface * p_userinterface ,
 //Base class of implementing Xerces XML handlers.
 //This is useful because there may be more than one XML file to read.
 //So one calls this functions with different handlers passed.
  XERCES_CPP_NAMESPACE::DefaultHandler & r_defaulthandler
  )
{
  bool bXercesSuccessfullyInitialized = false ;
//  LOG( "read XML document \"" << lpwstrBufferIdentifier << "\"\n" );
  //from http://xerces.apache.org/xerces-c/program-sax2-3.html:
  //Use this try block for no other _Xerces_ function that may throw an
  //exception than XMLPlatformUtils::Initialize() because
  //XMLPlatformUtils::Terminate should only be called if
  try
  {
    //http://xerces.apache.org/xerces-c/program-3.html:
    //"Independent of the API you want to use, DOM, SAX, or SAX2, your
    //application must initialize the Xerces system before using the API[...]"
    //Initialize() must be called _before_ any Xerces function call, else SIGSEV
    // /program crash.
    XMLPlatformUtils::Initialize();
    bXercesSuccessfullyInitialized = true ;
    LOG( "XML access successfully initialized\n" );
  }
  catch (const XMLException & toCatch )
  {
    char * p_archMessage = XMLString::transcode(toCatch.getMessage());
    std::ostringstream stdostringstream ;
    stdostringstream << "Xerces Error during initialization! :\n"
      << "Exception message is: \n"
         << p_archMessage << "\n" ;
    p_userinterface->Confirm( stdostringstream ) ;
      //DEBUG("XML error:%s\n",message);
//      LOG( "XML error:" << message << "\n" );
    XMLString::release(&p_archMessage);
    return FAILURE;
  }
  if( bXercesSuccessfullyInitialized )
  {
    XERCES_CPP_NAMESPACE::MemBufInputSource membufinputsource(
      arbyXMLdata,
      dwSizeInByte ,
      L"IPC_buffer" ) ;
    readXMLConfig(
      membufinputsource ,
      model,
      p_userinterface ,
      r_defaulthandler
      ) ;
    DEBUGN( "terminating XML usage" );
    //http://xerces.apache.org/xerces-c/program-3.html:
    //"Independent of the API you want to use, DOM, SAX, or SAX2, your
    //application must [...] and terminate it after you are done.
    //When Terminate() was called in another block (even if in a function that
    //is called in the same block) than program crash.
    XMLPlatformUtils::Terminate();
  }
  return SUCCESS ;
}

  char readXMLConfig(
    XERCES_CPP_NAMESPACE::InputSource & r_inputsource, //PStates & pstates
	  Model & model, 
	  UserInterface * p_userinterface ,
   //Base class of implementing Xerces XML handlers.
   //This is useful because there may be more than one XML file to read.
   //So one calls this functions with different handlers passed.
	  XERCES_CPP_NAMESPACE::DefaultHandler & r_defaulthandler
    )
	{
    BYTE byReturn = FAILURE ;
      //DEBUG("readXMLConfig begin--filename:%s\n",xmlFile);
    //Initialize to NULL just to avoid (g++) compiler warning.
	  XERCES_CPP_NAMESPACE::SAX2XMLReader * p_sax2xmlreader = NULL ;
	  p_sax2xmlreader = XMLReaderFactory::createXMLReader();
    if( p_sax2xmlreader )
    {
      p_sax2xmlreader->setFeature(XMLUni::fgSAX2CoreValidation, true);
      p_sax2xmlreader->setFeature(XMLUni::fgSAX2CoreNameSpaces, true);   // optional

      //DefaultHandler * p_defaultHandler = new DefaultHandler();
      DefaultHandler defaultHandler;
      //p_sax2xmlreader->setContentHandler(defaultHandler);
      p_sax2xmlreader->setContentHandler(//&sax2
        & r_defaulthandler );
      p_sax2xmlreader->setErrorHandler(//p_defaultHandler
        & defaultHandler );
      try
      {
//        LOGN( "before parsing XML document" );
        p_sax2xmlreader->parse(//xmlFile
          r_inputsource );
        DEBUG("-----------End of loading from config file\n") ;
//          if( model.m_bTruncateLogFileForEveryStartup )
//              g_logger.TruncateFileToZeroAndRewrite() ;
         byReturn = SUCCESS;
      }
      catch (const XMLException & toCatch)
      {
        LOGN( "XMLException" );
        //Use wide string because maybe chinese file names.
        std::wstring stdwstrMessage =
          std::wstring( L"XML exception in document \"" )
          + std::wstring( r_inputsource.getSystemId() ) +
          L"\" :" + //message
          toCatch.getMessage() ;
//        char * message = XMLString::transcode(toCatch.getMessage());
        p_userinterface->Confirm( //std::string( "XML exception in file" ) +
          //xmlFile +
          stdwstrMessage
          );
//        XMLString::release(&message);
//	        return FAILURE;
      }
      catch ( const SAXParseException & r_saxparseexception
         //const SAXException & r_saxexception
         )
      {
        LOGN( "SAXParseException" );
//        LOGN( "SAXException" );
//        char * pchMessage = XMLString::transcode(
//            r_saxparseexception.getMessage()
//            r_saxexception.getMessage()
//          );
//        std::string strMessage = "XML error in file: \"" +
//          std::string(xmlFile)
        //Use wide string because maybe chinese file names.
        std::wstring stdwstrMessage = L"XML exception in document \""
          + std::wstring( r_inputsource.getSystemId() ) +
          L"\" :"
//          + "\", line " + to_stdstring( r_saxparseexception.getLineNumber() )
//          + ", column " + to_stdstring( r_saxparseexception.getColumnNumber() )
          + L"\", line " + GetStdWstring( to_stdstring(
              r_saxparseexception.getLineNumber() ) )
          + L", column " + GetStdWstring( to_stdstring(
            r_saxparseexception.getColumnNumber() ) )
//          + "\", line " + to_stdstring( r_saxexception.getLineNumber() )
//          + ", column " + to_stdstring( r_saxexception.getColumnNumber() )
          + L": " + r_saxparseexception.getMessage()
          + L"\nIn order to solve this problem you may look into the XML "
          "specifications for element names etc" ;
        p_userinterface->Confirm(//pchMessage
          //strMessage
          stdwstrMessage );
//        XMLString::release( & pchMessage );
//	        return FAILURE;
      }
      catch (...)
      {
        LOGN( "Unexpected Exception at parsing XML\n" ) ;
        p_userinterface->Confirm("Unexpected Exception parsing the XML document\n");
//	        return FAILURE;
      }
      //__finally
     //   {
     ////        //http://xerces.apache.org/xerces-c/apiDocs-2/classXMLReaderFactory.html#_details:
     //   //"The parser object returned by XMLReaderFactory is owned by the
     //   //calling users, and it's the responsiblity of the users to
     //   //delete that parser object, once they no longer need it."
     //   delete p_sax2xmlreader;
     //   }
      //delete p_defaultHandler;

      //http://xerces.apache.org/xerces-c/apiDocs-2/classXMLReaderFactory.html#_details:
      //"The parser object returned by XMLReaderFactory is owned by
      //the calling users, and it's the responsiblity of the users to
      //delete that parser object, once they no longer need it."
      delete p_sax2xmlreader;
    }
    else
      p_userinterface->Confirm( "Xerces failed to create an XML reader" );
//	  return SUCCESS;
    return byReturn ;
	}
//#endif //#ifdef COMPILE_WITH_XERCES
