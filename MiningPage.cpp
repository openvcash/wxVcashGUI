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
#include <wx/sizer.h>
#endif

#include "MiningPage.h"
#include "VcashApp.h"

#define startT wxT("Start")
#define stopT wxT("Stop")

using namespace wxGUI;

MiningPage::MiningPage(VcashApp &vcashApp, wxWindow &parent)
        : isMining(false)
        , wxPanel(&parent) {

    vcashApp.view.miningPage = this;

    mining = new wxButton(this, wxID_ANY, startT);
    setMining(isMining);

    wxSizer *pageSizer = new wxBoxSizer(wxHORIZONTAL);
    pageSizer->AddStretchSpacer(1);
    pageSizer->Add(mining, wxSizerFlags().Center());
    pageSizer->AddStretchSpacer(1);

    SetSizerAndFit(pageSizer);

    mining->Bind(wxEVT_BUTTON, [this, &vcashApp](wxCommandEvent &) {
        toogleMining();
        vcashApp.controller.onMiningPressed(isMining);

    });
}

void MiningPage::setMining(bool b) {
    isMining = b;
    mining->SetLabelText(isMining ? stopT : startT);
    mining->SetToolTip(isMining ? wxT("Stop mining") : wxT("Start mining"));
}

bool MiningPage::getMining() {
    return isMining;
}

void MiningPage::toogleMining() {
    setMining(!getMining());
}


