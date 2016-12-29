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
#include <wx/graphics.h>
#include <wx/sizer.h>
#include <wx/textdlg.h>
#endif

#include "coin/constants.hpp"
#include "coin/utility.hpp"

#include "EntryDialog.h"
#include "Resources.h"
#include "VcashApp.h"
#include "WalletActions.h"

using namespace wxGUI;

bool WalletActions::encrypt(VcashApp &vcashApp, wxWindow &parent) {
    wxString title = wxT("Encrypt wallet");

    bool result = false;
    if(vcashApp.controller.isWalletCrypted()) {
        wxMessageBox(
                wxT("Cannot encrypt wallet. Wallet is already encrypted.\n"
                    "Please, try to change password intead."),
                title, wxOK | wxICON_EXCLAMATION, &parent);
    } else {
        int res = wxMessageBox(
                wxT("Encrypting your wallet keeps it safe in case it is lost or stolen.\n"
                    "Do you want to encrypt your wallet?"),
                title, wxYES_NO | wxICON_QUESTION | wxYES_DEFAULT, &parent);
        if(res == wxYES) {
            wxMessageBox(
                    wxT("Do a safe back up of your password. Once encrypted, you\n"
                        "will not be able to use your funds without the password."),
                    title, wxOK | wxICON_INFORMATION, &parent);

            auto pair = EntryDialog::run( parent, title
                    , { { wxT("Password"), wxTE_PASSWORD, wxT("Enter your password"), wxDefaultSize }
                      , { wxT("Confirm"), wxTE_PASSWORD, wxT("Reenter your password"), wxDefaultSize }
                      }
                    , [](std::vector<wxString> values){
                        // check that password and confirmation are the same
                        return values[0] == values[1];
                    }
            );

            switch(pair.first) {
                case wxID_CANCEL:
                    break;
                case wxID_OK:
                    result = vcashApp.controller.onWalletWantEncrypt(pair.second[0].ToStdString());
                default:
                    break;
            }
        }
    }
    vcashApp.view.setWalletStatus(vcashApp.controller.getWalletStatus());
    return result;
}

void WalletActions::dumpHDSeed(VcashApp &vcashApp, wxWindow &parent) {
    new DumpHDSeedDlg(vcashApp, parent);
}


std::pair<bool, std::string> WalletActions::restoreHDSeed(wxWindow &parent) {
    // toDo check that deterministic wallets are set on config.dat
    wxString title = wxT("Create wallet");

    int result = wxMessageBox(
            wxT("Do you want to restore your wallet from\na backed up hierarchical deterministic seed?"),
            wxT("Restore wallet"), wxYES_NO | wxICON_QUESTION | wxYES_DEFAULT, &parent);

    if(result==wxYES) {

        std::vector<EntryDialog::Entry> entries = {
                {wxT("HD seed"), wxTE_MULTILINE, wxT("Enter your hierarchical deterministic seed"), wxSize(300, 100)},
        };

        auto pair = EntryDialog::run( parent, wxT("Restore wallet"), entries
                , [](std::vector<wxString> values) {
                    return !values[0].empty();
                }
        );
        if(pair.first == wxID_OK) {
            std::string seed = pair.second[0].ToStdString();
            // toDo check that this is a valied HD seed
            return std::make_pair(true, seed);
        } else {
            wxMessageBox(
                    wxT("You did not provide a hierarchical deterministic seed."),
                    title, wxOK | wxICON_INFORMATION, &parent);
        }
    }
    wxMessageBox(
            wxT("A new wallet will be generated.\n"
                "Wait until wallet is loaded to do a safe\n"
                "back up of your hierarchical deterministic seed."),
            title, wxOK | wxICON_INFORMATION, &parent);
    return std::make_pair(false, "");
}

bool WalletActions::changePassword(VcashApp &vcashApp, wxWindow &parent) {
    // toDo: we cannot currently check that walletChangePassword ended successfully.
    // We need to modify wallet_change_passphrase on stack.hpp so that it returns a bool.
    // Problem is wallet_change_passphrase runs in a different thread.
    // toDo: wallet_change_passphrase does not modify lock status for wallet, but we let
    // wallet locked if old password is weong. We need to lock in order to check provided
    // old password was correct.
    wxString title = wxT("Change password");

    bool result = false;
    if(!vcashApp.controller.isWalletCrypted()) {
        wxMessageBox(
                wxT("Cannot change password. Wallet is not currently encrypted"),
                title, wxOK | wxICON_EXCLAMATION, &parent);
    } else {
        int res = wxMessageBox(
                wxT("Do you want to change your wallet password?"),
                title, wxYES_NO | wxICON_QUESTION | wxYES_DEFAULT, &parent);
        if (res == wxYES) {
            wxMessageBox(
                    wxT("In order to change your password, you need\n"
                        "to provide your old password firstly.\n"
                        "Do a safe back up of your new password. Once\n"
                        "encrypted, you will not be able to use your\n"
                        "funds without this password."),
                    title, wxOK | wxICON_INFORMATION, &parent);

            bool wasLocked = vcashApp.controller.isWalletLocked();
            // lock wallet to be able to check old password
            vcashApp.controller.onWalletWantLock();

            auto pair = EntryDialog::run(parent, title,
                                         { {wxT("Old password"),         wxTE_PASSWORD, wxT("Enter your old password"),   wxDefaultSize}
                                         , {wxT("New password"),         wxTE_PASSWORD, wxT("Enter your new password"),   wxDefaultSize}
                                         , {wxT("Confirm new password"), wxTE_PASSWORD, wxT("Reenter your new password"), wxDefaultSize}
                                         }, [&vcashApp](std::vector<wxString> values) {
                        return (values[1] == values[2]) // confirm new password and confirmation are the same
                               && vcashApp.controller.onWalletWantUnlock(
                                values[0].ToStdString()); // we can unlock if provided old password is correct
                    }
            );

            switch (pair.first) {
                case wxID_CANCEL:
                    // we cannot restore old locked status as we don't know old password
                    if(!wasLocked) {
                        wxMessageBox(
                                wxT("Wallet was unlocked before but it could not\n"
                                    "be unlocked now as old password is unknown"),
                                title, wxOK | wxICON_INFORMATION, &parent);
                    }

                    break;
                case wxID_OK:
                    vcashApp.controller.walletChangePassword(
                            pair.second[0].ToStdString(),
                            pair.second[1].ToStdString());
                    result = true; // here we are assuming password was changed successfully
                    break;
                default:
                    break;
            }

            // Wallet could be currently unlocked. Lock it in case it was locked before
            if(wasLocked)
                vcashApp.controller.onWalletWantLock();
        }
    }
    vcashApp.view.setWalletStatus(vcashApp.controller.getWalletStatus());
    return result;
}

bool WalletActions::lock(VcashApp &vcashApp, wxWindow &parent) {
    wxString title = wxT("Lock wallet");

    bool result = false;
    if(vcashApp.controller.isWalletLocked()) {
        wxMessageBox(
                wxT("Cannot lock wallet. Wallet is already locked"),
                title, wxOK | wxICON_EXCLAMATION, &parent);
    } else {
        result = vcashApp.controller.onWalletWantLock();
    }
    vcashApp.view.setWalletStatus(vcashApp.controller.getWalletStatus());
    return result;
}

bool WalletActions::unlock(VcashApp &vcashApp, wxWindow &parent) {
    wxString title = wxT("Unlock wallet");

    bool result = false;
    if(!vcashApp.controller.isWalletLocked()) {
        wxMessageBox(
                wxT("Cannot unlock wallet. Wallet is not currently locked"),
                title, wxOK | wxICON_EXCLAMATION, &parent);
    } else {
        auto pair = EntryDialog::run(parent
                , title
                , { { wxT("Password"), wxTE_PASSWORD, wxT("Enter your password"), wxDefaultSize } }
                , [&vcashApp](std::vector<wxString> values) {
                    bool unlocked = vcashApp.controller.onWalletWantUnlock(values[0].ToStdString());
                    return unlocked;
                }
        );
        result = pair.first == wxID_OK;
    }
    vcashApp.view.setWalletStatus(vcashApp.controller.getWalletStatus());
    return result;
}

void WalletActions::rescan(VcashApp &vcashApp, wxWindow &parent) {
    wxString title = wxT("Rescan wallet");
    int result = wxMessageBox(
            wxT("Do you want to rescan your wallet?"),
            title, wxYES_NO | wxICON_QUESTION | wxYES_DEFAULT, &parent);
    if(result == wxYES) {
        result = wxMessageBox(
                wxT("The wallet will shutdown. You will have to\n"
                            "restart the wallet in order to complete the rescan."
                ),
                title, wxOK | wxCANCEL | wxOK_DEFAULT | wxICON_INFORMATION, &parent);
        if(result == wxOK)
            vcashApp.controller.rescanWallet();
    }
}

DumpHDSeedDlg::DumpHDSeedDlg(VcashApp &vcashApp, wxWindow &parent)
        : ShowInfoDialog(parent, wxT("Show HD seed"), [this, &vcashApp]() {

    wxString warning = wxT("This is your hierarchical deterministic seed.\n"
                           "Anyone knowing this seed can access your\n"
                           "funds. Keep it safe!");
    wxTextCtrl *seedCtrl = new wxTextCtrl( this, wxID_ANY, wxT(""), wxDefaultPosition
            , wxSize(260, 80), wxTE_MULTILINE | wxTE_BESTWRAP | wxTE_READONLY);

    wxString seed = vcashApp.controller.getHDSeed();

    seedCtrl->SetValue(seed.empty() ? wxT("This is not a hierarchical deterministic wallet!") : seed);
    seedCtrl->SetEditable(false);

    wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);
    int border = 10;
    if(!seed.empty()) {
        vbox->Add(new wxStaticText(this, wxID_ANY, warning), 0, wxALL | wxALIGN_CENTER, border);
        seedCtrl->SetToolTip(warning);
    }

    vbox->Add(seedCtrl, 1, wxALL | wxALIGN_CENTER, border);
    return vbox;
}) {}

SettingsMenu::SettingsMenu(VcashApp &vcashApp, wxWindow &parent) : wxMenu() {
    enum PopupMenu {
        About, ChangePass, Encrypt, Lock, Seed, Unlock, Rescan
    };

    bool loaded = vcashApp.controller.isWalletLoaded();
    if(loaded) {
        wxMenu *submenu = new wxMenu();
        if(!vcashApp.controller.isWalletLocked()) {
            submenu->Append(Seed, wxT("&Show HD seed"));
        } else {
            submenu->Append(Seed, wxT("&Show HD seed (unlock first!)"));
            submenu->Enable(Seed, false);
        }
        if(vcashApp.controller.isWalletCrypted())
            submenu->Append(ChangePass, wxT("&Change password"));
        if(!vcashApp.controller.isWalletCrypted())
            submenu->Append(Encrypt, wxT("&Encrypt"));
        if(vcashApp.controller.isWalletLocked())
            submenu->Append(Unlock, wxT("&Unlock"));
        else if(vcashApp.controller.isWalletCrypted()) {
            submenu->Append(Lock, wxT("&Lock"));
        }
        submenu->Append(Rescan, wxT("&Rescan"));
        AppendSubMenu(submenu, wxT("Wallet"));
        AppendSeparator();
    }

    Append(About, wxT("&About"));

    auto select = parent.GetPopupMenuSelectionFromUser(*this);

    switch(select) {
        case Encrypt: {
            WalletActions::encrypt(vcashApp, parent);
            break;
        }
        case ChangePass: {
            WalletActions::changePassword(vcashApp, parent);
            break;
        }
        case Seed: {
            WalletActions::dumpHDSeed(vcashApp, parent);
            break;
        }
        case Lock: {
            WalletActions::lock(vcashApp, parent);
            break;
        }
        case Unlock: {
            WalletActions::unlock(vcashApp, parent);
            break;
        }
        case Rescan: {
            WalletActions::rescan(vcashApp, parent);
            break;
        }
        case About: {
            class AboutDlg : public ShowInfoDialog {
            public:
                AboutDlg(wxWindow &parent) : ShowInfoDialog(parent, wxT("About wxVcash"), [this]() {
                    wxIcon vcash = Resources::vcashIcon;

                    wxImage image;
                    image.Create(vcash.GetWidth(), vcash.GetHeight());
                    image.InitAlpha();
                    unsigned char *alpha = image.GetAlpha();
                    memset(alpha, wxIMAGE_ALPHA_TRANSPARENT, image.GetWidth()*image.GetHeight());

                    wxGraphicsContext *gc = wxGraphicsContext::Create(image);
                    gc->DrawIcon(vcash, 0, 0, vcash.GetWidth(), vcash.GetHeight());
                    delete gc;
                    int dimension = 64;
                    image.Rescale(dimension, dimension);

                    wxStaticBitmap *bm= new wxStaticBitmap(this, wxID_ANY, wxBitmap(image));
                    wxStaticText *text =
                            new wxStaticText(this, wxID_ANY,
                                             wxT("A wxWidgets wallet for Vcash.\n"
                                                 "Copyright (C) The Vcash Developers.\n\n"
                                                 "Vcash version: "+coin::utility::format_version(coin::constants::version_client)));
                    wxBoxSizer *hbox = new wxBoxSizer(wxHORIZONTAL);

                    hbox->Add(bm, 0, wxALL | wxALIGN_CENTER, 10);
                    hbox->Add(text, 0, wxALL | wxALIGN_CENTER, 10);
                    return hbox;
                }) {}
            };

            new AboutDlg(parent);
            break;
        }
        default:
            break;
    }
}
