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
#include <wx/sizer.h>
#include <wx/textentry.h>
#include <wx/valnum.h>
#include <wx/valtext.h>
#endif

#include "TransferPage.h"
#include "VcashApp.h"
#include "wxStack.h"

using namespace wxGUI;

TransferPage::TransferPage(VcashApp &vcashApp, wxWindow &parent) : wxPanel(&parent) {
    vcashApp.view.transferPage = this;

    double vcashMaxSupply = wxStack::maxMoneySupply / wxStack::oneVcash;
    int decimals = 6;
    wxFloatingPointValidator<double>
            amountValidator(decimals, nullptr, wxNUM_VAL_ZERO_AS_BLANK | wxNUM_VAL_NO_TRAILING_ZEROES);
    amountValidator.SetRange(0, vcashMaxSupply);

    wxTextValidator addressValidator(wxFILTER_ALPHANUMERIC);
#if defined(__WXOSX__)
    // ToDo: validator doesn't work in OS X. In any case, we validate on Send button pressing
    amountCtrl = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0 /*, amountValidator*/);
#else
    amountCtrl = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0, amountValidator);
#endif
    addressCtrl = new wxTextCtrl(this, wxID_ANY, wxT(""), wxDefaultPosition, wxDefaultSize, 0 /*, addressValidator*/);

    zeroTimeCtrl = new wxCheckBox(this, wxID_ANY, wxT("ZeroTime"));
    zeroTimeCtrl->SetValue(false);
    blendedOnlyCtrl = new wxCheckBox(this, wxID_ANY, wxT("Blended Only"));
    blendedOnlyCtrl->SetValue(false);

    wxButton *send = new wxButton(this, wxID_ANY, wxT("Send"));

    amountCtrl->SetToolTip(wxT("Amount of coins to send"));
    addressCtrl->SetToolTip(wxT("Destination address to send coins to"));
    zeroTimeCtrl->SetToolTip(wxT("Enable ZeroTime for this transaction"));
    blendedOnlyCtrl->SetToolTip(wxT("Use only blended coins for this transaction"));
    send->SetToolTip(wxT("Send coins now"));

    amountCtrl->SetHint(wxT("0.0"));
    addressCtrl->SetHint(wxT("destination address"));

    const int cols = 2, vgap = 5, hgap = 10, border = 20;
    wxFlexGridSizer *fgs = new wxFlexGridSizer(cols, vgap, hgap);
    fgs->Add(new wxStaticText(this, wxID_ANY, wxT("Pay:")), wxSizerFlags().Right());
    fgs->Add(amountCtrl, 1, wxRIGHT | wxEXPAND, 165);
    fgs->Add(new wxStaticText(this, wxID_ANY, wxT("To:")), wxSizerFlags().Right());
    fgs->Add(addressCtrl, 1, wxEXPAND);
    fgs->AddGrowableCol(1, 1);

    wxSizer *sizerV = new wxBoxSizer(wxVERTICAL);
    sizerV->Add(zeroTimeCtrl);
    sizerV->Add(blendedOnlyCtrl);

    wxSizer *sizerH = new wxBoxSizer(wxHORIZONTAL);
    sizerH->Add(sizerV, 1, wxALL | wxEXPAND, 0);
    sizerH->Add(send, wxSizerFlags());

    fgs->Add(0, border);
    fgs->Add(0, 0);

    fgs->Add(0, 0);
    fgs->Add(sizerH, wxSizerFlags().Expand());

    // Add a border
    wxSizer *pageSizer = new wxBoxSizer(wxVERTICAL);
    pageSizer->Add(fgs, 1, wxALL | wxEXPAND, border);

    SetSizerAndFit(pageSizer);

    send->Bind(wxEVT_BUTTON,
       [&parent, &vcashApp, this](wxCommandEvent &) {
           Controller &controller = vcashApp.controller;

           std::string pay = amountCtrl->GetValue().ToStdString();

           if (!controller.validateAmount(pay)) {
               wxMessageBox(wxT("\"") + pay + wxT("\" is not a valid amount of coins."), wxT("Error"),
                            wxOK | wxICON_EXCLAMATION, &parent
               );
               return;
           }

           std::string to = addressCtrl->GetValue().ToStdString();
           if (!controller.validateAddress(to)) {
               wxMessageBox(wxT("\"") + to + wxT("\" is not a valid address."), wxT("Error"), wxOK | wxICON_EXCLAMATION,
                            &parent
               );
               return;
           }

           wxString msg =
                   wxT("Send ") + pay + wxT(" coins to ") + to + wxT("?\nA transaction fee will be added.");
           auto response = wxMessageBox(msg, wxT("Send Coins"), wxYES_NO | wxYES_DEFAULT | wxICON_QUESTION,
                                        &parent
           );

           if (response == wxYES) {
               bool zeroTime = zeroTimeCtrl->GetValue();
               bool blendedOnly = blendedOnlyCtrl->GetValue();
               controller.onSendPressed(pay, to, zeroTime, blendedOnly);
           }
    });
}

void TransferPage::setDestinationAddress(const std::string &address) {
    addressCtrl->SetValue(address);
}

void TransferPage::setsetAmount(const std::string &amount) {
    amountCtrl->SetValue(amount);
}

void TransferPage::setZerotime(bool state) {
    zeroTimeCtrl->SetValue(state);
}

void TransferPage::setChainblender(bool state) {
    blendedOnlyCtrl->SetValue(state);
}
