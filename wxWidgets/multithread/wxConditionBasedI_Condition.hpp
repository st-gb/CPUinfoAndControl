/*
 * wxConditionBasedI_Condition.hpp
 *
 *  Created on: Jun 13, 2010
 *      Author: Stefan
 */

#ifndef WXCONDITIONBASEDI_CONDITION_HPP_
#define WXCONDITIONBASEDI_CONDITION_HPP_

class wxConditionBasedI_Condition
{
  wxMutex m_wxmutex ;
  wxCondition m_wxcondition ;
  wxConditionBasedI_Condition()
    : m_wxcondition (m_wxmutex)
  {

  }
  void Wait()
  {
    //http://docs.wxwidgets.org/stable/wx_wxcondition.html#wxconditionctor:
    //"The mutex must be locked by the caller before calling Wait  function."
    m_wxmutex.Lock() ;
    m_wxcondition.Wait() ;
  }
};

#endif /* WXCONDITIONBASEDI_CONDITION_HPP_ */
