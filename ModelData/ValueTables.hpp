/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
#pragma once

#include <map>
#include <string>
#include <vector>

class MSRdata ;

#define ARRAY_INDEX_OF_ATTR_NAME 0
#define ARRAY_INDEX_OF_ATTR_VALUE 1

//Holds attribute values of registers data only: column "MHz" is NOT included.
// MHz  | FID | DID
// 1100 | 14  |  2
class RegisterValueTableRow
{
//private:
public:
  //Attribute values sorted by attribute name.
  std::vector<std::string> m_vecstrAttributeValue ;
  RegisterValueTableRow(
    std::vector<std::string> & r_vecstrAttributeValue
//      std::string & r_strResultingAttributeName
    ) ;
  RegisterValueTableRow(
    std::map<std::string, std::string> & r_stdmap_stdstringName2Value ) ;
  bool operator < ( const RegisterValueTableRow & second ) const ;
  //Assignment ("=") operator adapted from:
  //http://www.cs.caltech.edu/courses/cs11/material/cpp/donnie/cpp-ops.html:
  RegisterValueTableRow & operator = (const RegisterValueTableRow & cr_msrdata ) ;
};

//  //for STL container:
//  inline bool operator < (
//    const RegisterValueTableRow & first,
//    const RegisterValueTableRow & second)
//  {
//    std::vector<std::string>::const_iterator iterSelf =
//       first.
//       m_vecstrAttributeValue.begin() ;
//    std::vector<std::string>::const_iterator iterOther =
//       second.m_vecstrAttributeValue.begin() ;
//    while( iterSelf != first.m_vecstrAttributeValue.end() &&
//       iterOther != second.m_vecstrAttributeValue.end() )
//    {
//        if( //compare string
//            *iterSelf < *iterOther )
//            return true ;
//        else
//            return false ;
//        ++ iterSelf ;
//        ++ iterOther ;
//    }
//    return false ;
//  }

//Holds all attribute values:
// MHz  | FID | DID
// 1100 | 14  |  2
class ValueTableRow
{
  std::string m_strResultingValue ;
  //By using a map it the list is sorted by the attribute name.
  std::map<std::string, std::string> m_stdmap_stdstringName2Value ;
};

class ValueTable
{
public:
  //The headings for the attribute names.
  std::vector<std::string> m_vecstdstrAttributeName ;
    //Using a map prevents double original (register) values like:
    //resulting value | original (register) values
    //    MHz         | FID | DID
    //  1100          | 14  |  2
    //  2200          | 14  |  2
    std::map<RegisterValueTableRow,std::string>
        m_stdmap_registervaluetablerow2stdstrResultingVal ;
  ValueTable(std::vector<std::string> & r_vecstdstrAttributeName)
  {
    m_vecstdstrAttributeName = r_vecstdstrAttributeName ;
  }
  void AddRow(
    std::map<std::string, std::string> & r_stdmap_stdstringName2Value ,
    std::string & //r_strResultingAttributeName
      r_strResultingAttributeValue
    ) ;
  //Assignment ("=") operator adapted from:
  //http://www.cs.caltech.edu/courses/cs11/material/cpp/donnie/cpp-ops.html:
  ValueTable & operator = (const ValueTable & cr_msrdata ) ;
};

class ValueTables
{
public:
  std::vector<ValueTable> m_vec_valuetable ;
  void AddValueTableRow(
    std::vector<MSRdata>::reverse_iterator rev_iter_msrdata ,
    //std::vector<std::string [2]> & r_stdvecstdstrAttrNameAndValue
    std::map<std::string, std::string> & r_stdmap_stdstringName2Value ,
    //std::string & r_strResultingAttributeName
    std::string ar_strResultingAttributeNameAndValue []
    ) ;
  //AAssignment ("=") operator adapted from:
  //http://www.cs.caltech.edu/courses/cs11/material/cpp/donnie/cpp-ops.html:
  ValueTables & operator = (const ValueTables & cr_msrdata ) ;
} ;
