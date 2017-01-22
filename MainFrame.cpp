/******************************************************************************
* wxVcashGUI: a GUI for Vcash, a decentralized currency
*             for the internet (https://vcash.info).
*
* Copyright (c) The Vcash Developers
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
******************************************************************************/

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/menu.h>
#include <wx/sizer.h>
#endif

#include "ContextMenu.h"
#include "MainFrame.h"
#include "MainPanel.h"
#include "OnPairsEvent.h"
#include "Resources.h"
#include "StatusBar.h"
#include "ToolsFrame.h"
#include "VcashApp.h"

#ifdef __WXGTK__
#define STYLE (wxDEFAULT_FRAME_STYLE & ~wxRESIZE_BORDER)
#else
#define STYLE wxDEFAULT_FRAME_STYLE
#endif

using namespace wxGUI;

MainFrame::MainFrame(VcashApp &vcashApp)
        : vcashApp(vcashApp)
        , wxFrame(NULL, wxID_ANY, wxT("Vcash"), wxDefaultPosition, wxDefaultSize,STYLE) {

    SetIcon(Resources::vcashIcon);

    MainPanel *mainPanel = new MainPanel(vcashApp, *this);

    ToolsFrame *toolsFrame = new ToolsFrame(vcashApp, *this);
    toolsFrame->Show(false);

    vcashApp.view.statusBar = new StatusBar(vcashApp, *this, *toolsFrame);
    SetStatusBar(vcashApp.view.statusBar);

    vcashApp.view.mainFrame = this;

    wxSizer *sizerV = new wxBoxSizer(wxVERTICAL);
    sizerV->Add(mainPanel
            , 1        // make horizontally stretchable
            , wxEXPAND // make it fill parent
    );

    SetSizerAndFit(sizerV);

    Bind(wxEVT_ONSTATUS, [&vcashApp](OnStatusEvent &ev) {
        vcashApp.controller.onStatus(ev.GetPairs());
    });

    Bind(wxEVT_ONERROR, [&vcashApp](OnErrorEvent &ev) {
        vcashApp.controller.onError(ev.GetPairs());
    });

    Bind(wxEVT_ONALERT, [&vcashApp](OnAlertEvent &ev) {
        vcashApp.controller.onAlert(ev.GetPairs());
    });

    Bind(wxEVT_MOVE, [toolsFrame](wxMoveEvent &ev) {
        toolsFrame->updatePosition();
        ev.Skip();
    });

    Bind(wxEVT_SIZE, [toolsFrame](wxSizeEvent &ev) {
        toolsFrame->updatePosition();
        ev.Skip();
    });

    Bind(wxEVT_ICONIZE, [this, &vcashApp, toolsFrame](wxIconizeEvent& ev) {
        if(vcashApp.taskBarIconEnabled) {
            bool wantIconize = IsIconized();
            if(wantIconize) {
                minimizeToTray();
            }
        } else {
            toolsFrame->Hide();
            ev.Skip();
        }
    });

    Bind(wxEVT_CLOSE_WINDOW, [this, &vcashApp](wxCloseEvent ev) {
       if(!vcashApp.canExit())
           ev.Veto();
       else
           Destroy();
    });

    Bind(wxEVT_SET_FOCUS, [mainPanel](wxFocusEvent &ev) {
        mainPanel->SetFocus();
    });

#if defined (__WXOSX__)
    wxMenuBar *menuBar = new wxMenuBar();
    wxMenu *menu = menuBar->OSXGetAppleMenu();

    menu->Insert(0, wxID_ABOUT, wxT("About"));
    menu->InsertSeparator(1);
    SetMenuBar(menuBar);

    Bind(wxEVT_MENU, [this] (wxCommandEvent &ev) {
        VcashApp &vcashApp = this->vcashApp;
        int id = ev.GetId();
        ContextMenu::MenuEntry selection;
        switch (id) {
            case wxID_ABOUT: {
                selection = static_cast<ContextMenu::MenuEntry>(ContextMenu::MenuEntry::About);
                break;
            }
            case wxID_EXIT: {
                selection = static_cast<ContextMenu::MenuEntry>(ContextMenu::MenuEntry::Exit);
                break;
            }
            default: {
                selection = static_cast<ContextMenu::MenuEntry>(id);
                break;
            }
        }
        ContextMenu::processSelection(vcashApp, *vcashApp.view.mainFrame, selection);
    });
#endif
};

void MainFrame::minimizeToTray() {
    vcashApp.view.toolsFrame->Iconize(true);
    vcashApp.view.toolsFrame->Hide();
    vcashApp.view.mainFrame->Iconize(true);
    vcashApp.view.mainFrame->Hide();
}

void MainFrame::restoreFromTray() {
    vcashApp.view.mainFrame->Iconize(false); // restore the window if minimized
    vcashApp.view.mainFrame->SetFocus();     // focus on my window
    vcashApp.view.mainFrame->Restore();      // restore the window if minimized
    vcashApp.view.mainFrame->Raise();        // bring window to front
    vcashApp.view.mainFrame->Show(true);     // show the window
}

void MainFrame::minimizeToRestoreFromTray() {
    if(IsShown())
        minimizeToTray();
    else
        restoreFromTray();
}