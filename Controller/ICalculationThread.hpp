/* Do not remove this header/ copyright information.
 *
 * Copyright © Trilobyte Software Engineering GmbH, Berlin, Germany 2010-2011.
 * You are allowed to modify and use the source code from
 * Trilobyte Software Engineering GmbH, Berlin, Germany for free if you are not
 * making profit with it or its adaption. Else you may contact Trilobyte SE.
 */
#pragma once //incl guard

class ICalculationThread
{
public:
  enum { started , ended } ;
  virtual void EndCalculationThread() = 0 ;
  virtual bool IsRunning() = 0 ;
  virtual void StartCalculationThread(BYTE byCoreID) = 0 ;
};
