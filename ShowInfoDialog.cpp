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
#include <wx/button.h>
#endif

#include <Resources.h>
#include "ShowInfoDialog.h"

using namespace wxGUI;

#if defined(__WXOSX__)
#define STYLE wxCAPTION
#else
#define STYLE wxDEFAULT_DIALOG_STYLE
#endif

ShowInfoDialog::ShowInfoDialog(wxWindow &parent, const wxString &title, std::function<wxSizer *(void)> contents, int timeout)
        : closeTimer(this)
        , wxDialog(&parent, wxID_ANY,  title, wxDefaultPosition, wxDefaultSize, STYLE) {

    wxButton *closeBt = new wxButton(this, wxID_CLOSE, wxT("Close"));

    const int border = 10;
    wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);
    vbox->Add(contents(), 0, wxALIGN_CENTER  | wxALL, border);
    vbox->Add(closeBt, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, border);

    SetSizerAndFit(vbox);
    SetMinSize(GetSize());
    SetMaxSize(GetSize());

    SetAffirmativeId(wxID_CLOSE);
    SetEscapeId(wxID_CLOSE);

    closeBt->SetDefault();
    closeBt->SetFocus();

    SetIcon(Resources::vcashIcon);
    CenterOnParent();

    if(timeout>=0)
        closeTimer.StartOnce(timeout);

    ShowModal();
    Destroy();
}


