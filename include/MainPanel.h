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
 
#ifndef MAINPANEL_H
#define MAINPANEL_H

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/notebook.h>
#include <wx/panel.h>
#include <wx/window.h>
#endif

namespace wxGUI {

    class VcashApp;

    enum class MainPage { Account, Transfer, History };

    class MainPanel : public wxPanel {
    private:
        wxNotebook *notebook;
    public:
        MainPanel(VcashApp &vcashApp, wxWindow &parent);
        void showPage(MainPage page);
    };
}

#endif // MAINPANEL_H