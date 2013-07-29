/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
/*
 * ValueTables.cpp
 *
 *  Created on: Jun 22, 2010
 *      Author: Stefan
 */

//TODO
//g++ warnings (because of vector containing another vector?
//see http://cboard.cprogramming.com/cplusplus-programming/
//  101030-warnings-when-using-vector-vector.html:
//http://www.c-plusplus.de/forum/viewtopic-var-p-is-1724876.html
//C:/MinGW/bin/../lib/gcc/mingw32/3.4.5/../../../../include/c++/3.4.5/bits/
// stl_uninitialized.h:
// In copy constructor `std::vector<_Tp, _Alloc>::vector(const std::vector<
// _Tp, _Alloc>&) [with _Tp = std::string, _Alloc = std::allocator<std::string>]':
//C:/MinGW/bin/../lib/gcc/mingw32/3.4.5/../../../../include/c++/3.4.5/bits/
//stl_uninitialized.h:82: warning: '__cur' might be used uninitialized in this
// function

#include "ValueTables.hpp"
#include "RegisterData.hpp"

//TODO warnings like this (class that has container member is used as container
// in other class?:
  //see http://cboard.cprogramming.com/cplusplus-programming/
  //  101030-warnings-when-using-vector-vector.html
//"C:/MinGW/bin/../lib/gcc/mingw32/3.4.5/../../../../include/c++/3.4.5/bits/
// stl_uninitialized.h:
//In copy constructor `std::vector<_Tp, _Alloc>::vector(
// const std::vector<_Tp, _Alloc>&) [with _Tp = std::string, _Alloc =
//  std::allocator<std::string>]':
// C:/MinGW/bin/../lib/gcc/mingw32/3.4.5/../../../../include/c++/3.4.5/bits/
//  stl_uninitialized.h:82:
// warning: '__cur' might be used uninitialized in this function"
//when compiling this file

RegisterValueTableRow::RegisterValueTableRow(
  std::vector<std::string> & r_vecstrAttributeValue
  )
{
  m_vecstrAttributeValue = r_vecstrAttributeValue ;
}

RegisterValueTableRow::RegisterValueTableRow(
  std::map<std::string, std::string> & r_stdmap_stdstringName2Value )
{
  std::map<std::string, std::string>::iterator iter =
    r_stdmap_stdstringName2Value.begin() ;
  while( iter != r_stdmap_stdstringName2Value.end() )
  {
      m_vecstrAttributeValue.push_back( iter->second ) ;
      ++ iter ;
  }
//        r_stdmap_stdstringName2Value ;
}

bool RegisterValueTableRow::operator < ( const RegisterValueTableRow & second )
  const
{
  std::vector<std::string>::const_iterator iterSelf =
     m_vecstrAttributeValue.begin() ;
  std::vector<std::string>::const_iterator iterOther =
     second.m_vecstrAttributeValue.begin() ;
  while( iterSelf != m_vecstrAttributeValue.end() &&
     iterOther != second.m_vecstrAttributeValue.end() )
  {
      if( //compare string
          *iterSelf < *iterOther )
          return true ;
      else
          return false ;
      ++ iterSelf ;
      ++ iterOther ;
  }
  return false ;
}

RegisterValueTableRow & RegisterValueTableRow::operator = (
  const RegisterValueTableRow & cr_registervaluetablerow )
{
  return *this;  // Return a reference to myself.
}


void ValueTable::AddRow(
  std::map<std::string, std::string> & r_stdmap_stdstringName2Value ,
  std::string & //r_strResultingAttributeName
    r_strResultingAttributeValue
  )
{
  m_stdmap_registervaluetablerow2stdstrResultingVal.insert(
    std::pair<RegisterValueTableRow,std::string>(
      RegisterValueTableRow(r_stdmap_stdstringName2Value),
        //r_strResultingAttributeName
        r_strResultingAttributeValue ) ) ;
}

ValueTable & ValueTable::operator = (const ValueTable & cr_valuetable )
{
  return *this;  // Return a reference to myself.
}


void ValueTables::AddValueTableRow(
  std::vector<MSRdata>::reverse_iterator rev_iter_msrdata ,
  //std::vector<std::string [2]> & r_stdvecstdstrAttrNameAndValue
  std::map<std::string, std::string> & r_stdmap_stdstringName2Value ,
  //std::string & r_strResultingAttributeName
  std::string ar_strResultingAttributeNameAndValue []
  )
{
  std::vector<ValueTable>::iterator iter_valuetable =
    m_vec_valuetable.begin() ;
  std::vector<std::string> stdvecstrAttributeNameOfTupelToInsert ;
  stdvecstrAttributeNameOfTupelToInsert.push_back(
    //r_strResultingAttributeName
    ar_strResultingAttributeNameAndValue[ARRAY_INDEX_OF_ATTR_NAME]
    ) ;
    //std::vector<std::string [2]>::iterator
  std::map<std::string, std::string>::iterator iter_Name2Value =
    //r_stdvecstdstrAttrNameAndValue.begin() ;
    r_stdmap_stdstringName2Value.begin() ;

  while( iter_Name2Value != //r_stdvecstdstrAttrNameAndValue.end()
    r_stdmap_stdstringName2Value.end() )
  {
      stdvecstrAttributeNameOfTupelToInsert.push_back(
        iter_Name2Value->first ) ;
      ++ iter_Name2Value ;
  }

  std::vector<std::string>::iterator
      iter_stdvecstrAttributeNameOfTupelToInsert =
      stdvecstrAttributeNameOfTupelToInsert.begin() ;
  std::vector<std::string>::iterator
    iter_vecstdstrTableAttributeName ;
  bool bTableFound = //true
    false ;
  while( iter_valuetable != m_vec_valuetable.end() )
  {
//            std::vector<std::string [2]>::iterator iter_Name2Value =
//              //r_stdvecstdstrAttrNameAndValue.begin() ;
//              r_stdmap_stdstringName2Value.begin() ;
    if( iter_valuetable->m_vecstdstrAttributeName.size() ==
      stdvecstrAttributeNameOfTupelToInsert.size()
      )
    {
      iter_stdvecstrAttributeNameOfTupelToInsert =
        stdvecstrAttributeNameOfTupelToInsert.begin() ;
        bTableFound = true ;
      iter_vecstdstrTableAttributeName =
        iter_valuetable->m_vecstdstrAttributeName.begin() ;
      while( //iter_Name2Value !=
        iter_stdvecstrAttributeNameOfTupelToInsert !=
              //r_stdvecstdstrAttrNameAndValue.end()
        //r_stdmap_stdstringName2Value.end()
        stdvecstrAttributeNameOfTupelToInsert.end()
        //&& iter_vecstdstrTableAttributeName !=
        //iter_valuetable->m_vecstdstrAttributeName.end()
        )
      {
          if( //*iter_Name2Value
            *iter_stdvecstrAttributeNameOfTupelToInsert !=
              //iter_valuetable->m_vecstdstrAttributeName
              *iter_vecstdstrTableAttributeName )
          {
              bTableFound = false ;
              break ;
          }
          //++ iter_Name2Value ;
          ++ iter_stdvecstrAttributeNameOfTupelToInsert ;
          ++ iter_vecstdstrTableAttributeName ;
      }
      if( bTableFound )
      {
          iter_valuetable->AddRow(r_stdmap_stdstringName2Value,
            //r_strResultingAttributeName
            ar_strResultingAttributeNameAndValue [
              //ARRAY_INDEX_OF_ATTR_NAME
              ARRAY_INDEX_OF_ATTR_VALUE ]
            ) ;
          break ;
      }
      ++ iter_valuetable ;
    }
  }// Loop traversing value tables.
  if( ! bTableFound )
  {
//            iter_valuetable = m_vec_valuetable.push_back( ValueTable(
//                stdvecstrAttributeNameOfTupelToInsert)) ;
//            iter_valuetable->AddRow(r_stdmap_stdstringName2Value,
//              r_strResultingAttributeName ) ;
      ValueTable valuetable (
          stdvecstrAttributeNameOfTupelToInsert) ;
      valuetable.AddRow(r_stdmap_stdstringName2Value,
        //r_strResultingAttributeName
        ar_strResultingAttributeNameAndValue [
          //ARRAY_INDEX_OF_ATTR_NAME
          ARRAY_INDEX_OF_ATTR_VALUE ]
        ) ;
      m_vec_valuetable.push_back( valuetable ) ;
      //break ;
  }
}

ValueTables & ValueTables::operator = (const ValueTables & cr_valuetables )
{
  return *this;  // Return a reference to myself.
}
