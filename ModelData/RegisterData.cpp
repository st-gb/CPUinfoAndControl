/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
#include "RegisterData.hpp"

MSRdata::MSRdata(//DWORD
  unsigned /*long*/ dwIndex
  , std::string & stdstrRegisterName )
  :
  //Initialize in the same order as textual in the declaration?
  //(to avoid g++ warnings)
  m_byCoreID(0)
//  , m_dwIndex ( dwIndex )
//  , m_stdstrRegisterName ( stdstrRegisterName )
  , CPUregisterProperties(dwIndex, stdstrRegisterName)
{
}

MSRdata & MSRdata::operator = (const MSRdata & cr_msrdata )
{
//    m_stdvec_bitrange = cr_registerdataToCopyFrom.m_stdvec_bitrange.
  return *this;  // Return a reference to myself.
}

//std::string 
bool //GetTableContainingDataName
  MSRdata::GetTableAttributeValue(
  std::vector<RegisterDataTable>::iterator iter_registerdatatable ,
  const std::vector<std::string> & stdvector_stdstringAttributeValue ,
  std::string & strValue )
{
//  bool bReplace =false ;
  std::string stdstrCurrentRegisterDataTableAttributeValue ;
  std::string stdstrCurrentRegisterDataValue ;
  std::string stdstrRegisterDataTableAttributeName ;
  std::string stdstrRegisterDataName ;
  //TODO: an indexed/ sorted table respectively a hashmap where 1 hash value 
  //made up of all attribute values except for the first column 
  //( e.g. "MHz  FID DID 
  //        2200  14   1 -> hash '14\n1'
  //        1100  14   2 -> hash '14\n2' " )
  //would be faster.
  for( //WORD 
    unsigned short wRowIndex = 1 ; wRowIndex <
    iter_registerdatatable->m_wRows ; ++ wRowIndex )
    for( //WORD 
      unsigned short wRegisterDataIndex = 0 ; wRegisterDataIndex < 
        stdvector_stdstringAttributeValue.size() ;
      ++ wRegisterDataIndex )
    {
      stdstrCurrentRegisterDataValue = stdvector_stdstringAttributeValue.
        at(//wColIndex-1
        wRegisterDataIndex ) ;
      stdstrRegisterDataName = m_stdvec_registerdata.at(//wRowIndex-1
        wRegisterDataIndex).m_strDataName ;
      for( //WORD 
        unsigned short wColIndex = 1 ; wColIndex < iter_registerdatatable->
        m_wCols ; ++ wColIndex )
      {
        stdstrCurrentRegisterDataTableAttributeValue = 
          iter_registerdatatable->m_ararstdstr//[wColIndex][wRowIndex] 
          [wRowIndex * iter_registerdatatable->m_wCols + wColIndex ] ;
        stdstrRegisterDataTableAttributeName =
          iter_registerdatatable->m_ararstdstr [ wColIndex ] ;

        //If the attribute NAME of the table matches the register name.
        if( //iter_registerdatatable->m_ararstdstr [ wColIndex ] 
          stdstrRegisterDataTableAttributeName
          == //m_stdvec_registerdata.at(wRowIndex-1).m_strDataName 
            stdstrRegisterDataName 
          )
        {
          //At least 1 attribute name is identical.
          if( //m_ararstdstr
            //iter_registerdatatable->m_ararstdstr[wColIndex][wRowIndex] 
            stdstrCurrentRegisterDataTableAttributeValue != 
            //stdvector_stdstringAttributeValue.at(wColIndex-1) 
            stdstrCurrentRegisterDataValue 
            )
          {
            //bReplace = true ;
          //}
          //else
          //{
            //(re)start at next row
            continue ;
          }
          //When all attribute values from the table match the register values.
          if( //wRowIndex 
            wColIndex + 1 == iter_registerdatatable->//m_wRows 
            m_wCols )
          {
            //If the last column is reached the inner loop has been 
            //completely passed and thus all values read from CPU register
            //match the values from the table from second column till the
            //last column.
            //  bReplace = true ;
            //if( bReplace )
            {
              //stdstrCurrentAttributeValue 
              strValue = iter_registerdatatable->
                m_ararstdstr[ 
                  //Get value at 1st column at row "wRowIndex" 
                  iter_registerdatatable->m_wCols * wRowIndex
                //2 
                ] ;
            }
            return //iter_registerdatatable->m_ararstdstr[0] [wRowIndex] 
              //iter_registerdatatable->m_ararstdstr[ 
              //  iter_registerdatatable->m_wCols * wRowIndex]
              //stdstrCurrentAttributeValue ;
              true 
              //bReplace 
              ;
          }
        }

      }
    }
    return false ;
}

//An object of THIS class may contain more than 1 table.
//ex.: looking for: "MHz"
//true: table for "strDataName" exists
bool MSRdata::GetTableAttributeValue(
  const std::vector<std::string> & stdvector_stdstringAttributeValue, 
  //const std::string & strDataName ,
  std::vector<RegisterDataTable>::iterator iter_registerdatatable ,
  std::string & strAttrVal )
{
  //std::vector<RegisterDataTable>::iterator iter_registerdatatable = 
  //  //m_stdvector_ararstdstr
  //  m_stdvector_registerdatatable.begin() ;
  //while( iter_registerdatatable != //m_stdvector_ararstdstr.end() 
  //  m_stdvector_registerdatatable.end() )
  {
    if( iter_registerdatatable->m_wRows > 0 )
    {
      //for( WORD wColIndex = 0 ; wColIndex < iter_registerdatatable->
      //  m_wCols ; ++ wColIndex )
      //  if( m_ararstdstr[0][wColIndex] == strDataName )

      //1st column and 1st row must be the attribute name that should 
      //be replaced.
      //if( iter_registerdatatable->m_ararstdstr[0][0] == strDataName )
        {
          //if( //only if an attribute name is not a data of MSR register
          //  //it should be replaced: if attributes in the TABLE are 
          //  //e.g. "FID" and "MHz", the value fo "FID" should be replaced
          //  //by the value of "MHz" and "MHz"'s value should NOT be 
          //  //replaced by "FID"'s value.
          //  NotWithinMSRregisterData(strDataName) )
          //{
            //search within table where values are same as in 
            //"stdvector_stdstringAttributeValue"
          //strAttrVal = GetTableAttributeValue(iter_registerdatatable, 
          //  stdvector_stdstringAttributeValue) ;
          //iter_registerdatatable = m_stdvector_registerdatatable.end() ;
          ////Fullfill outer loop condition: 
          ////Next increment it gets m_stdvec_registerdata.end() -> break loop
          //-- iter_registerdatatable ;
          //break;
          return //true 
            GetTableAttributeValue(iter_registerdatatable, 
            stdvector_stdstringAttributeValue, strAttrVal ) ;
        }
    }
    ++ iter_registerdatatable ;
  }
  return false ;
}
//A value may be also dependant on 2 or more values:
//-The freq in MHz for Griffin CPUs is dependant on Frequency ID Divisor ID:
//   Frequency ID  Divisor ID  freq in MHz
//            14            1         2200
//            13            1         2100
//            14            2         1100
void MSRdata::setTable(
  //A string gives high independance instead of e.g. an integer (->no floats
  //possible)
  std::string //[][] 
  * 
  ararstdstr
  , WORD wCols, WORD wRows)
{
  //m_stdvector_ararstdstr
  m_stdvector_registerdatatable.push_back(//ararstdstr
    RegisterDataTable(ararstdstr , 
    //WORD 
    wCols, 
    //WORD 
    wRows));
}
