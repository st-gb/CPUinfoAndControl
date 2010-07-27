/*
 * Windows.cpp
 *
 *  Created on: 05.04.2010
 *      Author: Stefan
 */

//Attempt to add standby/ hibernate detection to a console app:
//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
//ATOM PumaStateCtrl::MyRegisterClass(//HINSTANCE hInstance
//                     )
//{
//  ATOM atom ;
//  WNDCLASSEX wcex;
//  HWND hwnd ;
//  DEBUG("PumaStateCtrl::MyRegisterClass begin\n");
//  hwnd = ::GetConsoleWindow();
//  if(hwnd== NULL)
//  {
//    DEBUG("there is no such associated console.\n");
//  }
//  else
//    DEBUG("hwnd: %lu\n", //(DWORD)
//      hwnd);
//  DEBUG("PumaStateCtrl::MyRegisterClass before ::GetModuleHandle(NULL)\n");
//  //The following is only for getting standby and hibernate
//  //notifications for console apps (but it does not work yet).
//#ifndef X64 //does not compile under x64 because of
//  //"Fehler   2   error C2065: 'GWL_HINSTANCE': nichtdeklarierter Bezeichner  "
//  m_hinstanceThisModule = (HINSTANCE) ::GetWindowLong(
//    hwnd,
//    GWL_HINSTANCE//Retrieves a handle to the application instance.
//    );
//#endif
//  ::SetWindowLongPtr(hwnd,GWLP_WNDPROC, (LONG)&DialogProc);
//  //If the function fails, the return value is zero.
//  if(m_hinstanceThisModule == 0 )
//  {
//    DEBUG("::GetWindowLong(...) failed\n");
//  }
//  else
//    DEBUG("::GetWindowLong(...) succeeded\n");
//  //m_hinstanceThisModule = //(HANDLE)
//  //  ::GetModuleHandle(NULL); //If this parameter is NULL, GetModuleHandle returns a handle to the file used to create the calling process (.exe file).
//  ////If the function fails, the return value is zero.
//  //if(m_hinstanceThisModule == 0 )
//  //{
//  //  DEBUG("::GetModuleHandle(...) failed\n");
//  //}
//  //else
//  //  DEBUG("::GetModuleHandle(...) succeeded\n");
//
//  DEBUG("PumaStateCtrl::MyRegisterClass begin\n");
//  wcex.cbSize = sizeof(WNDCLASSEX);
//
//  wcex.style          = CS_HREDRAW | CS_VREDRAW;
//  //wcex.lpfnWndProc  = (WNDPROC)WndProc;
//  wcex.lpfnWndProc    = (WNDPROC)DialogProc;
//  wcex.cbClsExtra     = 0;
//  //Reserve extra window memory by specifying a nonzero value in the
//  //cbWndExtra member of the WNDCLASSEX structure used with the RegisterClassEx function.
//  wcex.cbWndExtra     = 0;
//  //wcex.hInstance        = hInstance;
//  wcex.hInstance      = m_hinstanceThisModule ;
//  //wcex.hIcon            = LoadIcon(hInstance, (LPCTSTR)IDI_OS_EVENTS);
//  wcex.hIcon = 0 ;
//  //wcex.hCursor      = LoadCursor(NULL, IDC_ARROW);
//  wcex.hCursor        = 0;
//  wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
//  //wcex.lpszMenuName = (LPCSTR)IDC_OS_EVENTS;
//  wcex.lpszMenuName   = 0;
//  //wcex.lpszClassName    = szWindowClass;
//  wcex.lpszClassName  = _T("PumaStateCtrl");
//  //wcex.hIconSm      = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);
//  wcex.hIconSm        = 0;
//
//  DEBUG("before RegisterClassEx(&wcex)\n");
//  atom = RegisterClassEx(&wcex);
//  //If the function fails, the return value is zero.
//  if( atom == 0 )
//    DEBUG("RegisterClassEx(...) failed. ::GetLastError():%lu\n",
//      ::GetLastError());
//  DEBUG("PumaStateCtrl::MyRegisterClass before return\n");
//  return atom ;
//}
