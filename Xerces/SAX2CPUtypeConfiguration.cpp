/*
 * SAX2CPUtypeConfiguration.cpp
 *
 *  Created on: 01.01.2013
 *      Author: Stefan
 */

#include <preprocessor_macros/logging_preprocessor_macros.h> //LOGN(...)
#include <ModelData/ModelData.hpp> //class Model
#include <UserInterface/UserInterface.hpp> //class UserInterface
#include "Xerces/SAX2CPUtypeConfiguration.hpp"
#include <Xerces/XercesAttributesHelper.hpp>
#include <Xerces/XercesString.hpp> //Xerces::ansi_or_wchar_string_compare(...)

namespace Xerces
{

  SAX2CPUtypeConfiguration::SAX2CPUtypeConfiguration(
    UserInterface & r_userinterface,
    const Model & r_model )
    :
//      m_r_model(r_model),
      m_c_r_cpucoredata(r_model.m_cpucoredata),
      m_r_model( (Model &) r_model),
      m_SuitabilityLevel(0),
      m_HighestSuitability(
        /** Set the minumum match to model, else if e.g. no controller for
        *  Intel Core (family 6, model 14) and Controller for Pentium M (family 6, model 13):
        *  it uses the Pentium M controller and the OS crashes */
        SAX2CPUtypeConfiguration::/*model family*/ vendor
        )
      , m_r_userinterface(r_userinterface)
  {
    LOGN_INFO(//"minimum suitability level: "
      "suitability level below minimum: "
      << m_HighestSuitability )
  }

  SAX2CPUtypeConfiguration::~SAX2CPUtypeConfiguration()
  {
    // TODO Auto-generated destructor stub
  }

  void SAX2CPUtypeConfiguration::startElement(
    const XMLCh * const cpc_xmlchURI ,
    const XMLCh * const cpc_xmlchLocalName ,
    const XMLCh * const cpc_xmlchQualifiedName ,
    const XERCES_CPP_NAMESPACE::Attributes & cr_xercesc_attributes
    )
  {
    unsigned numItemsSpecified = 0;
    //bool bCPUinfoGetterAndOrControllerSpecified = false;
    //bool bCPUregisterDataSpecified = false;
    //bool bCPUcoreUsageGetterSpecified = false;
    m_SuitabilityLevel = 0;
    if( Xerces::ansi_or_wchar_string_compare(cpc_xmlchLocalName,
        ANSI_OR_WCHAR("CPUID") ) == 0)
    {
      LOGN_DEBUG("CPUinfoGetterAndOrController element")
      std::string std_strAttributeName, std_str,
        std_strCPUinfoGetterAndOrController,
        std_strCPUregisterData, std_strCPUcoreUsageGetter;
      std_strAttributeName = "CPUinfoGetterAndOrController" /*"dynLibName"*/;
      if( XercesAttributesHelper::GetAttributeValue(
          cr_xercesc_attributes, std_strAttributeName, std_strCPUinfoGetterAndOrController) ==
          XercesAttributesHelper::getting_attribute_value_succeeded )
      {
        ++ numItemsSpecified;
        //bCPUinfoGetterAndOrControllerSpecified = true;
      }
      std_strAttributeName = "CPUregisterData";
      if( XercesAttributesHelper::GetAttributeValue(
          cr_xercesc_attributes, std_strAttributeName, std_strCPUregisterData) ==
          XercesAttributesHelper::getting_attribute_value_succeeded )
      {
        ++ numItemsSpecified;
        //bCPUregisterDataSpecified = true;
      }
      std_strAttributeName = "CPUcoreUsageGetter";
      if( XercesAttributesHelper::GetAttributeValue(
          cr_xercesc_attributes, std_strAttributeName, std_strCPUcoreUsageGetter) ==
          XercesAttributesHelper::getting_attribute_value_succeeded )
      {
        ++ numItemsSpecified;
        //bCPUcoreUsageGetterSpecified = true;
      }
      if( numItemsSpecified )
      {
        LOGN_DEBUG( std_strAttributeName << " attribute's value:" << std_str)
        std_strAttributeName = "vendor";
        if( XercesAttributesHelper::GetAttributeValue(
            cr_xercesc_attributes, std_strAttributeName, std_str) ==
            XercesAttributesHelper::getting_attribute_value_succeeded )
        {
          LOGN_DEBUG( std_strAttributeName << " attribute's value:" << std_str)
          if(m_c_r_cpucoredata.m_strVendorID == std_str)
          {
            m_SuitabilityLevel ++;
            std_strAttributeName = "family";
            if( XercesAttributesHelper::GetAttributeValue(
                cr_xercesc_attributes, std_strAttributeName, std_str) ==
                XercesAttributesHelper::getting_attribute_value_succeeded
              )
            {
              LOGN_DEBUG( std_strAttributeName << " attribute's value:" << std_str)
//              if( std_str == convertToStdString(m_c_r_cpucoredata.m_wFamily) )
              unsigned valueFromXML;
              /** Converting string to a number and then comparing the numbers
                  has the advatage that also non-decimal number in the XML file 
               *  can be compared to a decimal number. */
              if( ConvertStdStringToTypename(valueFromXML, std_str) )
                if( valueFromXML == m_c_r_cpucoredata.m_wFamily )
              {
                m_SuitabilityLevel ++;
                std_strAttributeName = "model";
                if( XercesAttributesHelper::GetAttributeValue(
                    cr_xercesc_attributes, std_strAttributeName, std_str) ==
                    XercesAttributesHelper::getting_attribute_value_succeeded )
                {
                  LOGN_DEBUG( //FULL_FUNC_NAME <<
                    std_strAttributeName
                    << " attribute's value:" << std_str << " level:"
                    << m_SuitabilityLevel)
                  if( std_str == convertToStdString<WORD>(m_c_r_cpucoredata.m_byModel) )
                  {
                    m_SuitabilityLevel ++;
                    std_strAttributeName = "stepping";
                    if( XercesAttributesHelper::GetAttributeValue(
                        cr_xercesc_attributes, std_strAttributeName, std_str) ==
                        XercesAttributesHelper::getting_attribute_value_succeeded )
                    {
                      LOGN_DEBUG( std_strAttributeName + " attribute's value:"
                        << std_str)
                      if( std_str == convertToStdString<WORD>(m_c_r_cpucoredata.
                        m_byStepping) )
                        m_SuitabilityLevel ++;
                      else //Don't allow controller for other stepping than CPU the
                        //program runs on.
                        m_SuitabilityLevel = 0;
                    }
                  }
                  else //Don't allow controller if model differs from the CPU
                    //the program runs on.
                    m_SuitabilityLevel = 0;
                }
              }
              else //Don't allow controller for other family than CPU the
                //program runs on.
                m_SuitabilityLevel = 0;
            }
            else
            {
              std::ostringstream std_oss;
              std_oss << "at least the \"" << std_strAttributeName
                << "\" attribute value must be specified";
              LOGN_ERROR(std_oss.str())
              std::wstring std_wstrDocumentIDandLocation;
              GetDocumentIDandLocation(std_wstrDocumentIDandLocation);
              std::wstring message = GetStdWstring(std_oss.str() ) +
                std_wstrDocumentIDandLocation;
              ShowMessage( message);
            }
          }
        }
        LOGN_DEBUG("current suitability level:" << m_SuitabilityLevel )
        if( m_SuitabilityLevel > m_HighestSuitability )
        {
          LOGN_INFO("for CPU model this program runs on: currently most "
            "suitable (level:" << m_SuitabilityLevel
            << ") CPU controller/ info getter: "
            << std_strCPUinfoGetterAndOrController
            << " CPU usage getter: "
            << std_strCPUcoreUsageGetter
            << " CPU register data file: "
            << std_strCPUregisterData
            )
          m_mostSuitableCPUinfoGetterAndOrController =
            std_strCPUinfoGetterAndOrController;
          m_mostSuitableCPUcoreUsageGetter = std_strCPUcoreUsageGetter;
          m_r_model.m_mostSuitableCPUregisterDataFile = std_strCPUregisterData;
          m_HighestSuitability = m_SuitabilityLevel;
        }
      }
      else
      {
        std::ostringstream std_oss;
        std_oss << "the \"" << std_strAttributeName
          << "\" attribute value must be specified";
        LOGN_ERROR(std_oss.str())
        std::wstring std_wstrDocumentIDandLocation;
        GetDocumentIDandLocation(std_wstrDocumentIDandLocation);
        std::wstring message = GetStdWstring(std_oss.str() ) +
          std_wstrDocumentIDandLocation;
        ShowMessage( message);
      }
    }
  }
  void SAX2CPUtypeConfiguration::ShowMessage(const std::wstring & message)
  {
    m_r_userinterface.MessageWithTimeStamp(message.c_str() );
  }
} /* namespace Xerces */
