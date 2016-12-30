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
#include <wx/msgdlg.h>
#include <wx/textdlg.h>
#endif

#include "Resources.h"
#include "StatusBarWallet.h"
#include "VcashApp.h"
#include "WalletActions.h"

using namespace wxGUI;

StatusBarWallet::StatusBarWallet(VcashApp &vcashApp, wxWindow &parent)
        : walletStatus(WalletStatus::Unknown)
        , bitmapLocked(Resources::locked)
        , bitmapUnlocked(Resources::unlocked)
        , bitmapUnknown (Resources::empty)
        , StatusBarImage(parent, *new wxBitmap()) {

    bindOnLeftClick([&parent, &vcashApp, this](wxMouseEvent &ev) {
        switch (walletStatus) {
            case WalletStatus::Locked: {
                WalletActions::unlock(vcashApp, parent);
                break;
            }

            case WalletStatus::Unlocked: {
                WalletActions::lock(vcashApp, parent);
                break;
            }

            case WalletStatus::Unencrypted: {
                WalletActions::encrypt(vcashApp, parent);
                break;
            }

            case WalletStatus::Unknown: {
                break;
            }
        }
    });

    bindOnRightClick([&parent, &vcashApp](wxMouseEvent &ev) {
       new SettingsMenu(vcashApp, parent);
    });

    setWalletStatus(walletStatus);
};

WalletStatus StatusBarWallet::getWalletStatus() {
    return walletStatus;
}

void StatusBarWallet::setWalletStatus(WalletStatus st) {
    walletStatus = st;
    switch (walletStatus) {
        case WalletStatus::Locked:
            SetBitmap(bitmapLocked);
            break;
        case WalletStatus::Unlocked:
        case WalletStatus::Unencrypted:
            SetBitmap(bitmapUnlocked);
            break;
        case WalletStatus::Unknown:
            SetBitmap(bitmapUnknown);
            break;
    }
}

