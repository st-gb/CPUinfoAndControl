#pragma once

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
      )
    {
        m_vecstrAttributeValue = r_vecstrAttributeValue ;
    }
    RegisterValueTableRow(
      std::map<std::string, std::string> & r_stdmap_stdstringName2Value )
    {
        std::map<std::string, std::string>::iterator iter =
          r_stdmap_stdstringName2Value.begin() ;
        while( iter != r_stdmap_stdstringName2Value.end() )
        {
            m_vecstrAttributeValue.push_back( iter->second ) ;
            ++ iter ;
        }
        r_stdmap_stdstringName2Value ;
    }

};
    //for STL container:
    inline bool operator < (
      const RegisterValueTableRow & first,
      const RegisterValueTableRow & second)
    {
        std::vector<std::string>::const_iterator iterSelf =
           first.
           m_vecstrAttributeValue.begin() ;
        std::vector<std::string>::const_iterator iterOther =
           second.m_vecstrAttributeValue.begin() ;
        while( iterSelf != first.m_vecstrAttributeValue.end() &&
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
      )
    {
      m_stdmap_registervaluetablerow2stdstrResultingVal.insert(
        std::pair<RegisterValueTableRow,std::string>(
            RegisterValueTableRow(r_stdmap_stdstringName2Value),
              //r_strResultingAttributeName
              r_strResultingAttributeValue ) ) ;
    }
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
} ;
