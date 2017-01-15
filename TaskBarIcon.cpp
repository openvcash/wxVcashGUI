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

#include "ContextMenu.h"
#include "MainFrame.h"
#include "Resources.h"
#include "TaskBarIcon.h"
#include "VcashApp.h"

using namespace wxGUI;

TaskBarIcon::TaskBarIcon(VcashApp &vcashApp) : vcashApp(vcashApp), wxTaskBarIcon() {
    SetIcon(Resources::vcashIcon, wxT("Vcash"));

    Bind(wxEVT_TASKBAR_LEFT_DOWN, [this](wxTaskBarIconEvent &ev) {
        this->vcashApp.view.minimizeToRestoreFromTray();
    });

    Bind(wxEVT_MENU, [this] (wxCommandEvent &ev) {
        VcashApp &vcashApp = this->vcashApp;
        ContextMenu::MenuEntry selection = static_cast<ContextMenu::MenuEntry>(ev.GetId());
        vcashApp.view.restoreFromTray();
        ContextMenu::processSelection(vcashApp, *vcashApp.view.mainFrame, selection);
    });
}

bool TaskBarIcon::isEnabled() {
    return wxTaskBarIcon::IsAvailable();
}

void TaskBarIcon::disable() {
    wxImage image = Resources::vcashImage64.ConvertToDisabled();
    wxIcon icon;
    icon.CopyFromBitmap(wxBitmap(image));
    SetIcon(icon);
}

wxMenu* TaskBarIcon::CreatePopupMenu() {
    return new ContextMenu(vcashApp);
}