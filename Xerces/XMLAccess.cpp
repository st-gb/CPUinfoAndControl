/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
//#ifdef COMPILE_WITH_XERCES
//If not included: compiler error "C1010" with MSVC++.
#include "../stdafx.h"

#include "XMLAccess.hpp"
#include <global.h> //for SUCCESS, FAILURE
//#include <winnt.h> //for LPWSTR
#include <Controller/Logger/Logger.hpp>
//for std::string convertToStdString(typename )
#include <Controller/character_string/stdstring_format.hpp>
#include <Controller/character_string/stdtstr.hpp>//for GetStdString(...)
#include <ModelData/ModelData.hpp>
#include <UserInterface/UserInterface.hpp>

//#include <Xerces/XercesHelper.hpp> //
#include <Xerces/XercesString.hpp> //GET_WCHAR_STRING_FROM_XERCES_STRING(...)

#include <iostream>
//class XERCES_CPP_NAMESPACE::LocalFileInputSource
#include <xercesc/framework/LocalFileInputSource.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>

#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
  //#include "SAX2Handler.hpp"

using namespace std;
using namespace xercesc;

//extern FULLY_QUALIFIED_LOGGER_CLASS_NAME g_logger ;

char Apache_Xerces::ReadXMLfileInitAndTermXerces(
  const char * cp_chXMLfilePath,
  Model & model,
  UserInterface * p_userinterface ,
 //Base class of implementing Xerces XML handlers.
 //This is useful because there may be more than one XML file to read.
 //So one calls this functions with different handlers passed.
  XERCES_CPP_NAMESPACE::DefaultHandler & r_defaulthandler
  )
{
//  LOG( "read XML configuration--filename: \"" << cp_chXMLfilePath << "\"" );
  WRITE_TO_LOG_FILE_AND_STDOUT_NEWLINE(
    "read XML configuration--filename: \"" << cp_chXMLfilePath << "\"" )
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
    LOGN( "Xerces access successfully initialized"//"\n"
      )
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
    LOGN( "XML error:" << stdostringstream.str() << "\n" );
    XMLString::release( & p_archMessage);
    return FAILURE;
  }
  if( bXercesSuccessfullyInitialized )
  {
    //  XERCES_CPP_NAMESPACE::LocalFileInputSource xerces_localfileinputsource(
    //    XERCES_STRING_FROM_ANSI_STRING(cp_chXMLfilePath) ) ;
    XMLCh * p_xmlchXMLfilePath = XMLString::transcode(cp_chXMLfilePath);
    if( p_xmlchXMLfilePath )
    {
      XERCES_CPP_NAMESPACE::LocalFileInputSource xerces_localfileinputsource(
        p_xmlchXMLfilePath ) ;
      LOGN( "after creating a LocalFileInputSource obj");
      Apache_Xerces::ReadXMLdocument(
        xerces_localfileinputsource ,
//        model,
        p_userinterface ,
        r_defaulthandler
        ) ;
      XMLString::release( & p_xmlchXMLfilePath);
    }
    //http://xerces.apache.org/xerces-c/program-3.html:
    //"Independent of the API you want to use, DOM, SAX, or SAX2, your
    //application must [...] and terminate it after you are done.
    //When Terminate() was called in another block (even if in a function that
    //is called in the same block) than program crash.
    XMLPlatformUtils::Terminate();
    LOG( "Xerces access terminated"//"\n"
      )
  }
  return SUCCESS ;
}

char ReadXMLdocumentInitAndTermXerces(
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
  LOGN("readXMLConfig begin")
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
    LOGN( "Xerces access successfully initialized"//\n"
      )
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
    XMLString::release( & p_archMessage);
    return FAILURE;
  }
  if( bXercesSuccessfullyInitialized )
  {
    Apache_Xerces::ReadXMLdocumentWithoutInitAndTermXerces(
      arbyXMLdata ,
      dwSizeInByte ,
      lpwstrBufferIdentifier ,
      model,
      p_userinterface ,
     //Base class of implementing Xerces XML handlers.
     //This is useful because there may be more than one XML file to read.
     //So one calls this functions with different handlers passed.
      r_defaulthandler
      ) ;
    DEBUGN( "terminating XML usage" );
    //http://xerces.apache.org/xerces-c/program-3.html:
    //"Independent of the API you want to use, DOM, SAX, or SAX2, your
    //application must [...] and terminate it after you are done.
    //When Terminate() was called in another block (even if in a function that
    //is called in the same block) than program crash.
    XMLPlatformUtils::Terminate();
    LOG( "Xerces access terminated"//\n"
      )
  }
  return SUCCESS ;
}

inline void OutputXMLExceptionErrorMessage(
  const XMLException & cr_xmlexception,
  XERCES_CPP_NAMESPACE::InputSource & r_inputsource,
  UserInterface *& p_userinterface)
{
  XMLFileLoc xmlfilelocSrcLine = cr_xmlexception.getSrcLine();

  //Use wide string because maybe Chinese file names.
  std::wstring stdwstrMessage =
    std::wstring( L"XML exception in document \"" )
    + GET_WCHAR_STRING_FROM_XERCES_STRING(r_inputsource.getSystemId() )
    + L"\"\n"
    L"in line " + convertToStdWstring( xmlfilelocSrcLine)
    + L"\" :" + //message
    GET_WCHAR_STRING_FROM_XERCES_STRING(cr_xmlexception.getMessage() );
  //        char * message = XMLString::transcode(toCatch.getMessage());

  std::string std_strMessage;
  getUTF8string_inline(stdwstrMessage, std_strMessage);
//  LOGN_TYPE( "XMLException", )
  g_logger.Log_inline( std_strMessage,
    LogLevel::error);
  p_userinterface->Confirm(
    stdwstrMessage
    );
  //          return FAILURE;
}

inline void OutputSAXParseExceptionErrorMessage(
  const SAXParseException & cr_saxparseexception,
  XERCES_CPP_NAMESPACE::InputSource & r_inputsource,
  UserInterface *& p_userinterface
  )
{
  LOGN( "SAXParseException" );
  XMLFileLoc xmlfilelocColumnNumber = cr_saxparseexception.getColumnNumber();
  XMLFileLoc xmlfilelocLineNumber = cr_saxparseexception.getLineNumber();
  std::wstring stdwstrInputSourceName =
        GET_WCHAR_STRING_FROM_XERCES_STRING(
            r_inputsource.getSystemId() ) ;
  std::wstring stdwstrMessage = L"SAX parse exception in document \""
    + stdwstrInputSourceName
    + L"\"\n"
    L"in line " + convertToStdWstring( xmlfilelocLineNumber )
    + L", column " + convertToStdWstring(xmlfilelocColumnNumber )
    + L":\n\"" +
    GET_WCHAR_STRING_FROM_XERCES_STRING(cr_saxparseexception.getMessage() )
    ;
  if( ! xmlfilelocColumnNumber && ! xmlfilelocLineNumber)
  {
    stdwstrMessage += L"\n\nThis probably means that this document/ file does "
      "not exist";
  }
  stdwstrMessage += L"\n\n"//"Sorry, no further information.\n"
    "In order to solve "
    "this problem you may look into the XML specifications for element names "
    "etc.";

//  LOGN_TYPE( GetStdString_Inline( stdwstrMessage ),
//    I_LogFormatter::log_message_typeERROR )
  std::string std_strMessage;
  getUTF8string_inline(stdwstrMessage, std_strMessage);
  g_logger.Log_inline( std_strMessage,
    LogLevel::error, /*FULL_PUNC_NAME*/ __PRETTY_FUNCTION__);
  /** From non-GUI thread */
  p_userinterface->/*ShowMessageGUIsafe*/MessageWithTimeStamp(stdwstrMessage.c_str() );
}

inline void OutputSAXExceptionErrorMessage(
  const SAXException & cr_saxexception,
  XERCES_CPP_NAMESPACE::InputSource & r_inputsource,
  UserInterface *& p_userinterface)
{
  LOGN_TYPE( "SAXException", LogLevel::error);
  //Use wide string because maybe Chinese file names.
  std::wstring stdwstrMessage = L"SAX exception in document \""
    + GET_WCHAR_STRING_FROM_XERCES_STRING( r_inputsource.getSystemId() ) +
    L"\" :"
  //  + "\", line " + convertToStdWstring( r_saxexception.getLineNumber() )
  //  + ", column " + convertToStdWstring( r_saxexception.getColumnNumber() )
    +
    GET_WCHAR_STRING_FROM_XERCES_STRING( cr_saxexception.getMessage() )
    + L"\nIn order to solve this problem you may look into the XML "
    "specifications for element names etc" ;

  p_userinterface->Confirm(stdwstrMessage );
  //Outputting wide strings via std::ofstream (=ANSI) leads to "gedit"
  //not to recognize the character encoding of the log file.
  //        LOGWN_WSPRINTF(L"%ls", stdwstrMessage.c_str() )

//  LOGN_TYPE( GetStdString_Inline( stdwstrMessage ),
//    I_LogFormatter::log_message_typeERROR )
  std::string std_strMessage;
  getUTF8string_inline(stdwstrMessage, std_strMessage);
  g_logger.Log_inline( std_strMessage,
    LogLevel::error);
  //          return FAILURE;
}

/** @return readingXMLdocSucceeded, readingXMLdocFailed*/
enum Apache_Xerces::ReadXMLdocumentRetCodes Apache_Xerces::ReadXMLdocument(
  XERCES_CPP_NAMESPACE::InputSource & r_inputsource,
//	  Model & model,
  UserInterface * p_userinterface ,
   //Base class of implementing Xerces XML handlers.
   //This is useful because there may be more than one XML file to read.
   //So one calls this functions with different handlers passed.
  XERCES_CPP_NAMESPACE::DefaultHandler & r_defaulthandler
  , XERCES_CPP_NAMESPACE::DefaultHandler * const errorHandler// = NULL
  )
{
    LOGN( "begin contenthandler:" << & r_defaulthandler);
//    BYTE byReturn = readingXMLdocFailed ;
    enum Apache_Xerces::ReadXMLdocumentRetCodes returnCode =
      readingXMLdocFailed;
      //DEBUG("ReadXMLfileInitAndTermXerces begin--filename:%s\n",xmlFile);
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
      /** The "XERCES_CPP_NAMESPACE::DefaultHandler"(-derived) class whose
      * callback/ virtual ("startElement" etc.) methods should be called by
      * the Xerces framework. */
      p_sax2xmlreader->setContentHandler(//&sax2
        & r_defaulthandler );
      /** Calls "XERCES_CPP_NAMESPACE::DefaultHandler"(-derived)
      * "fatalError(const XERCES_CPP_NAMESPACE::SAXParseException &)" method
      * instead of throwing an exception within this method. */
      if( errorHandler)
        /**http://xerces.apache.org/xerces-c/apiDocs-3/classSAX2XMLReader.html#910e9acbec9c8bbd4ff3fdb92c76cf0e
         * "If the application does not register an error event handler, all
         * error events reported by the SAX parser will be silently ignored,
         * except for fatalError, which will throw a SAXException */
        p_sax2xmlreader->setErrorHandler(//& r_defaulthandler
          errorHandler
          );
      else
        /** error handler must provide*/
        p_sax2xmlreader->setErrorHandler(& r_defaulthandler);
      XERCES_CPP_NAMESPACE::ErrorHandler * sax2errorHandler =
        p_sax2xmlreader->getErrorHandler();
      LOGN( "error handler:" << sax2errorHandler );
      try
      {
        LOGN( "before parsing XML document "
          << GetStdString_Inline(
            GET_WCHAR_STRING_FROM_XERCES_STRING(
              r_inputsource.getSystemId()
            )
          )
        )
        p_sax2xmlreader->
          //from SAX2XMLReader::parse(const   InputSource&    source):
  //        * @exception SAXException Any SAX exception, possibly
  //        *            wrapping another exception.
  //        * @exception XMLException An exception from the parser or client
  //        *            handler code.
          parse(//xmlFile
            r_inputsource );
        LOGN_TYPE("Successfully parsed XML document/ input source \""
          << GetStdString_Inline(GET_WCHAR_STRING_FROM_XERCES_STRING(
              r_inputsource.getSystemId() )
            ) << "\"",
            LogLevel::success
          )
//          if( model.m_bTruncateLogFileForEveryStartup )
//              g_logger.TruncateFileToZeroAndRewrite() ;
//        byReturn = SUCCESS;
        returnCode = readingXMLdocSucceeded;
      }
      catch ( const XMLException & cr_xmlexception )
      {
        OutputXMLExceptionErrorMessage(cr_xmlexception,
          r_inputsource, p_userinterface);
      }
      catch ( const SAXParseException & cr_saxparseexception
         )
      {
        OutputSAXParseExceptionErrorMessage(cr_saxparseexception,
          r_inputsource, p_userinterface);
      }
      catch( const SAXException & cr_saxexception )
      {
        OutputSAXExceptionErrorMessage( cr_saxexception, r_inputsource,
          p_userinterface);
      }
      catch (...)
      {
        LOGN_TYPE( "Unexpected Exception at parsing XML", LogLevel::
          error ) ;
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
    else //if( p_sax2xmlreader )
      p_userinterface->Confirm( "Xerces failed to create an XML reader" );
//	  return SUCCESS;
    return //byReturn ;
      returnCode;
  }
//#endif //#ifdef COMPILE_WITH_XERCES
