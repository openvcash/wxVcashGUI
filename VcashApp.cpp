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

#include "MainFrame.h"
#include "Resources.h"
#include "TaskBarIcon.h"
#include "ToolsFrame.h"
#include "VcashApp.h"

#ifdef __WXGTK__
#include <X11/Xlib.h>
#endif

IMPLEMENT_APP(wxGUI::VcashApp)

using namespace wxGUI;

VcashApp::VcashApp() : view(), controller(view) {
#ifdef __WXGTK__
    // This App uses multiple threads.
    // Must be called here in the constructor of the GUI App
    XInitThreads();
#endif
}

bool VcashApp::OnInit() {
    if (!wxApp::OnInit())
        return false;

    Resources::init();

    // Create and fill in view
    MainFrame *mainFrame = new MainFrame(*this);
    mainFrame->Show(true);
    SetTopWindow(mainFrame);

    // Create taskbar icon
    taskBarIconEnabled = TaskBarIcon::isEnabled();
    if(taskBarIconEnabled)
        view.taskBarIcon = new TaskBarIcon(*this);

    return controller.onInit();
}

// This method is called before the main frame is destroyed. If
// false is returned, exit is aborted.
bool VcashApp::canExit() {
    if(!controller.isWalletLoaded())
        return false;
    else {
        if(taskBarIconEnabled)
            view.taskBarIcon->disable();
        return true;
    }
}

// This method is called once the main frame has been destroyed
int VcashApp::OnExit() {
    // Destroy taskbar icon (it isn't destroyed automatically)
    if(taskBarIconEnabled)
        view.taskBarIcon->~TaskBarIcon();
    // stop the stack
    return controller.onExit();
}

// Calling this method is the same as clicking close button on main frame.
void VcashApp::exit() {
    // Closes main frame. As a result, all wxWindows are destroyed.
    // Finally, OnExit is called and the stack is stopped.
    view.mainFrame->Close();
}