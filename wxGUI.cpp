#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
#   include "wx/wx.h"
#endif //precompiled headers

#include "wxGUI.h"

//Wird (vom Framework) aufgerufen, um das Programm zu initalisieren.
bool wxPumaStateCtrlApp::OnInit()
{
#ifdef __WXWINCE__
    srand((unsigned) CeGetRandomSeed());
#else
    srand((unsigned) time(NULL));
#endif

    m_wxframe = new BombsFrame(&m_game);

    //m_frame->NewGame(bombsID_EASY, false);

    return true;
}

BEGIN_EVENT_TABLE(wxPumaStateCtrlFrame, wxFrame)
    EVT_MENU(wxID_NEW,           BombsFrame::OnNewGame)
    EVT_MENU(wxID_EXIT,          BombsFrame::OnExit)
    EVT_MENU(wxID_ABOUT,         BombsFrame::OnAbout)
END_EVENT_TABLE()

wxPumaStateCtrlFrame::wxPumaStateCtrlFrame()
    //: wxFrame(NULL, wxID_ANY, wxT("wxBombs"), wxDefaultPosition,
    : wxFrame(NULL, wxID_ANY, wxT("PumaStateCtrl version"+__DATE__), 
      wxDefaultPosition,
      wxSize(300, 300), wxDEFAULT_DIALOG_STYLE|wxMINIMIZE_BOX)
{
    //SetIcon(wxICON(bombs));

#if wxUSE_STATUSBAR
    CreateStatusBar();
#endif

    // Create a menu bar for the frame
    wxMenuBar *menuBar = new wxMenuBar;
    wxMenu *menuFile = new wxMenu;
    wxMenu *menuLevel = new wxMenu;
    //menuLevel->AppendRadioItem(bombsID_EASY, wxT("&Easy (10x10)\tCtrl-1"));
    //menuLevel->AppendRadioItem(bombsID_MEDIUM, wxT("&Medium (15x15)\tCtrl-2"));
    //menuLevel->AppendRadioItem(bombsID_HARD, wxT("&Hard (25x20)\tCtrl-3"));

    //menuFile->Append(wxID_NEW, wxT("&New game\tCtrl-N"));
    //menuFile->Append(bombsID_LEVEL, wxT("&Level"),menuLevel, wxT("Starts a new game"));
    //menuFile->AppendCheckItem(bombsID_EASYCORNER, wxT("&Easy corner"));

//    menuFile->Append(wxID_NEW, wxT("&test dynamic array\tCtrl-N"));
//    menuFile->Append(bombsID_EASYCORNER, wxT("&Test FTP\tCtrl-E"));
    menuFile->Append(wxID_NEW, wxT("&load translator DLL\tCtrl-N"));
    menuFile->Append(bombsID_EASYCORNER, wxT("&translate text\tCtrl-E"));

    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT, wxGetStockLabel(wxID_EXIT), wxT("Quits the application"));

    menuBar->Append(menuFile, wxT("&File"));


    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT, wxT("&About"),
        wxT("Displays the program information") );

    menuBar->Append(menuHelp, wxT("&Help"));

    SetMenuBar(menuBar);

    // Create child subwindows.
    m_canvas = new BombsCanvas(this, m_game);

    // Ensure the subwindows get resized o.k.
    //  OnSize(width, height);

    // Centre frame on the screen.
    Centre(wxBOTH);

    // Show the frame.
    Show();
}

void BombsFrame::OnExit(wxCommandEvent& WXUNUSED(event))
{
    Close();
}
