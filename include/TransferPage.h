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
 
#ifndef TRANSFERPAGE_H
#define TRANSFERPAGE_H

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/checkbox.h>
#include <wx/panel.h>
#include <wx/textctrl.h>
#endif

namespace wxGUI {

    class VcashApp;

    class TransferPage : public wxPanel {
    public:
        TransferPage(VcashApp &vcashApp, wxWindow &parent);
        void setDestinationAddress(const std::string &address);
        void setsetAmount(const std::string &amount);
        void setZerotime(bool state);
        void setChainblender(bool state);
    private:
        wxTextCtrl *addressCtrl, *amountCtrl;
        wxCheckBox *zeroTimeCtrl, *blendedOnlyCtrl;
    };
}

#endif // TRANSFERPAGE_H