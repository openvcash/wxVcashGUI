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
#include <wx/artprov.h>
#include <wx/statbmp.h>
#include <wx/stdpaths.h>
#endif

#include "MainFrame.h"
#include "MainPanel.h"
#include "Resources.h"
#include "ShowInfoDialog.h"
#include "TaskBarIcon.h"
#include "ToolsFrame.h"
#include "Utils.h"
#include "VcashApp.h"

#ifdef __WXGTK__
#include <X11/Xlib.h>
#endif

IMPLEMENT_APP(wxGUI::VcashApp)

using namespace wxGUI;

// DDE Messages

#define DDE_SERVER_NAME     (wxT("vcash_dde_socket"))
#define DDE_TOPIC           (wxT("dde_topic"))
#define DDE_SEPARATOR      "*"
#define DDE_RAISE           "raise"
#define DDE_URL            "url"

VcashApp::DDEConnection::DDEConnection(VcashApp &vcashApp) : vcashApp(vcashApp) { }

bool VcashApp::DDEConnection::OnExec(const wxString& topic, const wxString& data) {
    std::string toParse = data.ToStdString();

    if(topic==DDE_TOPIC) {
        std::string msg = Utils::extractToken(toParse, DDE_SEPARATOR);

        if(msg==DDE_RAISE) {
            vcashApp.view.restoreFromTray();
        } else if(msg==DDE_URL) {
            vcashApp.parserURI(toParse);
        }
    }
    return true;
}

VcashApp::DDEServer::DDEServer(VcashApp &vcashApp) : vcashApp(vcashApp) { }

VcashApp::DDEConnection* VcashApp::DDEServer::OnAcceptConnection(const wxString &topic) {
    return new DDEConnection(vcashApp);
}


VcashApp::VcashApp() : view(), controller(view), ddeServer(*this) {
#ifdef __WXGTK__
    // This App uses multiple threads.
    // Must be called here in the constructor of the GUI App
    XInitThreads();
#endif
}

const wxCmdLineEntryDesc VcashApp::cmdLineDesc[] =  {
    { wxCMD_LINE_SWITCH , "?", "help", "shows command line help",
                wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_SWITCH , "ib", "import-blockchain", "imports blockchain from blockchain.dat file",
            wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_SWITCH , "eb", "export-blockchain", "exports blockchain to blockchain.dat file",
            wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_SWITCH , "ewt", "erase-wallet-transactions", "erases wallet transactions",
            wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_OPTION, "ws", "wallet-seed", "restores wallet from hierarchical deterministic seed",
            wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL  },
    { wxCMD_LINE_OPTION, "m", "mode", "use spv for light client (currenty unsupported)",
            wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL  },
    { wxCMD_LINE_OPTION, "url", "url", "pass a vcash url to wallet. Syntax is \"vcash:<address>?amount=<amount>\"",
            wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL  },
    { wxCMD_LINE_NONE }
};

// These two methods let us parse command line args
void VcashApp::OnInitCmdLine(wxCmdLineParser &parser) {
    parser.SetLogo(wxT("\nVcash version: "+controller.getVcashVersion()+"\nCopyright (C) The Vcash Developers\n"));
    parser.SetDesc(cmdLineDesc);
    parser.AddUsageText(wxT("\n"));
}

bool VcashApp::OnCmdLineParsed(wxCmdLineParser& parser) {
    wxString value;

    if (parser.Parse() == 0) {
        if(parser.Found("?")) {
            parser.Usage();
            return false;
        } else {
            if(parser.Found("ib"))
                args["import-blockchain"] = "1";
            if(parser.Found("eb"))
                args["export-blockchain"] = "1";
            if(parser.Found("ewt"))
                args["erase-wallet-transactions"] = "1";
            if(parser.Found("ws", &value))
                args["wallet-seed"] = value.ToStdString();
            if(parser.Found("m", &value))
                args["mode"] = value.ToStdString();
            if(parser.Found("url", &value))
                args[DDE_URL] = value.ToStdString();
        }
        return true;
    } else
        return false;
}

#define TK_AMOUNT           "amount"
#define TK_ZEROTIME         "zerotime"
#define TK_CHAINBLENDER     "chainblender"
#define TK_ON               "on"
#define TK_OFF              "off"
#define TK_TRUE             "true"
#define TK_FALSE            "false"

#define QUERY_DELIMITERS            "&;"
#define QUERY_BEGIN                 "?"
#define QUERY_ATTR_VALUE_DELIMITER  "="

void VcashApp::parserURI(std::string uri) {
    bool schemeFound = false;
    std::vector<std::string> schemes = { "vcash://", "vcash:", "xvc://", "xvc:" };
    for(auto const& scheme : schemes)
        if(Utils::isPrefix(scheme, uri)) {
            uri = uri.erase(0, scheme.length());
            schemeFound = true;
            break;
        }
    if(!schemeFound)
        return;

    std::string address = Utils::extractToken(uri, QUERY_BEGIN);
    if(address.empty())
        return;

    std::map<std::string, std::string> mapAttrValues;
    std::vector<std::string> booleanAttributes = { TK_ZEROTIME, TK_CHAINBLENDER };
    std::vector<std::string> attributes = { TK_AMOUNT };

    while(!uri.empty()) {
        std::string attrValue = Utils::extractToken(uri, QUERY_DELIMITERS);
        std::string attr = Utils::extractToken(attrValue, QUERY_ATTR_VALUE_DELIMITER);
        std::string value = attrValue;

        if(std::find(booleanAttributes.begin(), booleanAttributes.end(), attr) != booleanAttributes.end()) {
            if(value==TK_ON || value==TK_TRUE)
                mapAttrValues[attr] = TK_TRUE;
            else if(value==TK_OFF || value==TK_FALSE)
                mapAttrValues[attr] = TK_FALSE;
            else
                return;
        } else if (std::find(attributes.begin(), attributes.end(), attr) != attributes.end()) {
            mapAttrValues[attr] = value;
        } else
            return;
    }

    view.setDestinationAddress(address);

    std::string amount = Utils::find(TK_AMOUNT, mapAttrValues);
    if(!amount.empty())
        view.setAmount(amount);

    std::string zerotime = Utils::find(TK_ZEROTIME, mapAttrValues);
    if(zerotime==TK_TRUE)
        view.setZerotime(true);
    else if(zerotime==TK_FALSE)
        view.setZerotime(false);

    std::string chainblender = Utils::find(TK_CHAINBLENDER, mapAttrValues);
    if(chainblender==TK_TRUE)
        view.setChainblender(true);
    else if(chainblender==TK_FALSE)
        view.setChainblender(false);

    view.showMainPage(MainPage::Transfer);

    class InfoDlg : public ShowInfoDialog {
    public:
        InfoDlg(VcashApp &vcashApp, wxWindow &parent, const std::string &address, const std::string &amount)
                : ShowInfoDialog(parent, wxT("Vcash"), [this, address, amount]() {
            wxStaticBitmap *bm = new wxStaticBitmap(this, wxID_ANY, wxArtProvider::GetBitmap(wxART_WARNING,  wxART_OTHER, wxSize(64,64)));
            wxStaticText *text =
                    new wxStaticText(this, wxID_ANY,
                            !address.empty() && !amount.empty() ?
                                     wxT("A transfer to pay "+amount+" coins to "+address+" has been loaded.\n"
                                         "Press send button if you really want to send these coins.")
                            : wxT("Address "+address+" has been loaded in transfer page.")
                              );
            wxBoxSizer *hbox = new wxBoxSizer(wxHORIZONTAL);

            hbox->Add(bm, 0, wxALL | wxALIGN_CENTER, 10);
            hbox->Add(text, 0, wxALL | wxALIGN_CENTER, 10);
            return hbox;
        }, 4500) { }
    };

    new InfoDlg(*this, *view.mainFrame, address, amount);
}

bool VcashApp::OnInit() {
    if (!wxApp::OnInit())
        return false;

    std::string sendCoinsArgs = Utils::find(DDE_URL, args);

    wxString ddeServerName = wxStandardPaths::Get().GetTempDir()+"/"+DDE_SERVER_NAME;

    if(singleInstanceChecker.IsAnotherRunning()) {
        wxClient client;
        wxConnectionBase *conn = client.MakeConnection(wxT("localhost"), ddeServerName, DDE_TOPIC);

        if(!sendCoinsArgs.empty()) {
            std::string msg = DDE_URL;
            msg += DDE_SEPARATOR + sendCoinsArgs;
            conn->Execute(msg);
        }
        conn->Execute(DDE_RAISE);
        conn->~wxConnectionBase();
        return false;
    }

    // Start a DDEServer
    ddeServer.Create(ddeServerName);

    // Load resources
    Resources::init();

    // Create and fill in view
    MainFrame *mainFrame = new MainFrame(*this);
    mainFrame->Show(true);
    SetTopWindow(mainFrame);

    if(!sendCoinsArgs.empty())
        parserURI(sendCoinsArgs);

    // try to restore seed if no wallet exists
    bool isClient = Utils::find("mode", args) == "spv";
    if(!controller.walletExists(isClient)) {
        std::string seed = Utils::find("wallet-seed", args);
        if(seed.empty()) {
            auto pair = view.restoreHDSeed(*this);
            if (pair.first) {
                seed = pair.second;
                args["wallet-seed"] = seed;
            }
        }
    }

    // Create taskbar icon
    taskBarIconEnabled = TaskBarIcon::isEnabled();
    if(taskBarIconEnabled)
        view.taskBarIcon = new TaskBarIcon(*this);

    // Set global keys accelerators
    #define CTRL_T      (wxID_HIGHEST + 1)
    #define CTRL_M      (wxID_HIGHEST + 2)
    #define NUM_ACCELS  2

    wxAcceleratorEntry entries[NUM_ACCELS];
    entries[0].Set(wxACCEL_CTRL, 't', CTRL_T);
    entries[1].Set(wxACCEL_CTRL, 'm', CTRL_M);

    wxAcceleratorTable accel(NUM_ACCELS, entries);
    view.mainFrame->SetAcceleratorTable(accel);
    view.toolsFrame->SetAcceleratorTable(accel);

    Bind(wxEVT_MENU, [this](wxCommandEvent &ev) {
        view.showHideToolsFrame();
    }, CTRL_T);

    Bind(wxEVT_MENU, [this](wxCommandEvent &ev) {
        view.runContextMenu(*this, false);
    }, CTRL_M);

    return controller.onInit(args);
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

