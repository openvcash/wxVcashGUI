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
#include <wx/statbmp.h>
#endif

#include "ContextMenu.h"
#include "Resources.h"
#include "VcashApp.h"
#include "WalletActions.h"

using namespace wxGUI;

ContextMenu::ContextMenu(VcashApp &vcashApp) : wxMenu() {
    bool loaded = vcashApp.controller.isWalletLoaded();
    if (loaded) {
        wxMenu *submenu = new wxMenu();
        if (!vcashApp.controller.isWalletLocked()) {
            submenu->Append(static_cast<int>(MenuEntry::Seed), wxT("&Show HD seed"));
        } else {
            submenu->Append(static_cast<int>(MenuEntry::Seed), wxT("&Show HD seed (unlock first!)"));
            submenu->Enable(static_cast<int>(MenuEntry::Seed), false);
        }
        if (vcashApp.controller.isWalletCrypted())
            submenu->Append(static_cast<int>(MenuEntry::ChangePass), wxT("&Change password"));
        if (!vcashApp.controller.isWalletCrypted())
            submenu->Append(static_cast<int>(MenuEntry::Encrypt), wxT("&Encrypt"));
        if (vcashApp.controller.isWalletLocked())
            submenu->Append(static_cast<int>(MenuEntry::Unlock), wxT("&Unlock"));
        else if (vcashApp.controller.isWalletCrypted()) {
            submenu->Append(static_cast<int>(MenuEntry::Lock), wxT("&Lock"));
        }
        submenu->Append(static_cast<int>(MenuEntry::Rescan), wxT("&Rescan"));
        AppendSubMenu(submenu, wxT("Wallet"));
    }
    Append(static_cast<int>(MenuEntry::About), wxT("&About"));

    Append(static_cast<int>(MenuEntry::Exit), wxT("&Exit"));
    Enable(static_cast<int>(MenuEntry::Exit), loaded);
}

void ContextMenu::processSelection(VcashApp &vcashApp, wxWindow &parent, MenuEntry selection) {
    switch(selection) {
        case MenuEntry::About: {
            class AboutDlg : public ShowInfoDialog {
            public:
                AboutDlg(VcashApp &vcashApp, wxWindow &parent) : ShowInfoDialog(parent, wxT("About wxVcash"), [this, &vcashApp]() {
                    wxStaticBitmap *bm= new wxStaticBitmap(this, wxID_ANY, wxBitmap(Resources::vcashImage64));
                    wxStaticText *text =
                            new wxStaticText(this, wxID_ANY,
                                             wxT("A wxWidgets wallet for Vcash.\n"
                                                         "Copyright (C) The Vcash Developers.\n\n"
                                                         "Vcash version: "+vcashApp.controller.getVcashVersion()));
                    wxBoxSizer *hbox = new wxBoxSizer(wxHORIZONTAL);

                    hbox->Add(bm, 0, wxALL | wxALIGN_CENTER, 10);
                    hbox->Add(text, 0, wxALL | wxALIGN_CENTER, 10);
                    return hbox;
                }) {}
            };

            new AboutDlg(vcashApp, parent);
            break;
        }
        case MenuEntry::ChangePass: {
            WalletActions::changePassword(vcashApp, parent);
            break;
        }
        case MenuEntry::Exit: {
            vcashApp.exit();
            break;
        }
        case MenuEntry::Encrypt: {
            WalletActions::encrypt(vcashApp, parent);
            break;
        }
        case MenuEntry::Lock: {
            WalletActions::lock(vcashApp, parent);
            break;
        }
        case MenuEntry::Rescan: {
            WalletActions::rescan(vcashApp, parent);
            break;
        }
        case MenuEntry::Seed: {
            WalletActions::dumpHDSeed(vcashApp, parent);
            break;
        }
        case MenuEntry::Unlock: {
            WalletActions::unlock(vcashApp, parent);
            break;
        }
        default:
            break;
    }
}

void ContextMenu::runContextMenu(VcashApp &vcashApp, wxWindow &parent, wxPoint pos) {
    auto contextMenu = new ContextMenu(vcashApp);
    auto select = parent.GetPopupMenuSelectionFromUser(*contextMenu, pos);
    processSelection(vcashApp, parent, static_cast<MenuEntry>(select));
}


