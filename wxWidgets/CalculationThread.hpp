#pragma once //include guard

#include <wx/thread.h> //for class wxThread

namespace wxWidgets
{
  class CalculationThread
  {
  private:
    wxThread * mp_wxthread ;
  public:
    void StartCalculationThread(BYTE byCore)
    {
      mp_wxthread = new wxThread();
      if( mp_wxthread->Create() == wxTHREAD_NO_ERROR )
        m_wxthread->Run() ;
    }
  };
};
