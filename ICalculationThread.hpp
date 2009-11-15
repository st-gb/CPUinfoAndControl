#pragma once //incl guard

class ICalculationThread
{
public:
  virtual void EndCalculationThread() = 0 ;
  virtual bool IsRunning() = 0 ;
  virtual void StartCalculationThread(BYTE byCoreID) = 0 ;
};
