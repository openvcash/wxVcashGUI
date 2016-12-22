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
 
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include <wx/clipbrd.h>
#include <wx/menu.h>
#include "wx/sizer.h"
#endif

#include "AddressesPage.h"
#include "BlockExplorer.h"
#include "ToolsFrame.h"
#include "VcashApp.h"

namespace wxGUI {
    int cmpAddresses(wxIntPtr item1, wxIntPtr item2, wxIntPtr sortDt) {
        AddressesPage::SortData *sortData = (AddressesPage::SortData *) sortDt;
        AddressesPage *addressesPage = sortData->addressesPage;
        wxListCtrl *listCtrl = addressesPage->addresses;
        auto order = sortData->order;

        for(int i=0; i<order.size(); i++) {
            auto col = order[i].first;
            int result;

            switch(col) {
                case AddressesPage::Account: {
                    long index1 = listCtrl->FindItem(-1, item1);
                    auto str1 = listCtrl->GetItemText(index1, col);

                    long index2 = listCtrl->FindItem(-1, item2);
                    auto str2 = listCtrl->GetItemText(index2, col);

                    result = str1.Cmp(str2);
                    break;
                }

                case AddressesPage::Address: {
                    long index1 = listCtrl->FindItem(-1, item1);
                    auto str1 = listCtrl->GetItemText(index1, col);

                    long index2 = listCtrl->FindItem(-1, item2);
                    auto str2 = listCtrl->GetItemText(index2, col);

                    result = str1.Cmp(str2);
                    break;
                }
            }
            if((result != 0) || (i==order.size()-1))
                return order[i].second ? result : -result;
        }
    }
}

using namespace wxGUI;

AddressesPage::AddressesPage(VcashApp &vcashApp, wxWindow &parent)
        : wxPanel(&parent) {

    vcashApp.view.addressesPage = this;

    addresses = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                               wxLC_REPORT | wxLC_SINGLE_SEL | wxBORDER_NONE);
    addresses->InsertColumn(Account, "Account", wxLIST_FORMAT_LEFT, 100);
    addresses->InsertColumn(Address, "Address", wxLIST_FORMAT_LEFT, 330);

    wxSizer *pageSizer = new wxBoxSizer(wxVERTICAL);
    pageSizer->Add(addresses, 1, wxALL | wxEXPAND, 5);

    SetSizerAndFit(pageSizer);

    addresses->Bind(wxEVT_LIST_ITEM_RIGHT_CLICK, [this, &vcashApp](wxListEvent &event) {
        long index = event.GetIndex();
        bool onAddress = index >= 0;

        enum PopupMenu {
            Copy, BlockExperts, VcashExplorer, New, QR
        };

        wxMenu *explorers = new wxMenu();;
        explorers->Append(BlockExperts, wxT("Block Experts"));
        explorers->Enable(BlockExperts, onAddress);
        explorers->Append(VcashExplorer, wxT("Vcash Explorer"));
        explorers->Enable(VcashExplorer, onAddress);

        wxMenu popupMenu;
        popupMenu.AppendSubMenu(explorers, wxT("&Block explorer"));

        popupMenu.Append(Copy, wxT("&Copy"));
        popupMenu.Enable(Copy, onAddress);
        popupMenu.Append(New, wxT("&New"));

        popupMenu.Append(QR, wxT("&QR Code"));
        popupMenu.Enable(QR, onAddress);

        auto select = GetPopupMenuSelectionFromUser(popupMenu);

        switch (select) {
            case Copy: {
                if (index >= 0) {
                    wxString address = addresses->GetItemText(index, Address);

                    auto clipboard = wxTheClipboard;

                    if (clipboard->Open()) {
                        clipboard->Clear();
                        clipboard->SetData(new wxTextDataObject(address));
                        clipboard->Flush();

                        clipboard->Close();
                    }
                }
                break;
            }

            case BlockExperts: {
                if (index >= 0) {
                    std::string address = addresses->GetItemText(index, Address).ToStdString();
                    wxLaunchDefaultBrowser(BlockExperts::addressURL(address));
                }
                break;
            }

            case VcashExplorer: {
                if (index >= 0) {
                    std::string address = addresses->GetItemText(index, Address).ToStdString();
                    wxLaunchDefaultBrowser(VcashExplorer::addressURL(address));
                }
                break;
            }

            case New: {
                wxString account = (index >= 0) ? addresses->GetItemText(index, Account) : wxT("");
                vcashApp.controller.onConsoleCommandEntered("getnewaddress " + account.ToStdString());
                break;
            }

            case QR:
                if (index >= 0) {
                    //toDo: generate QR code
                    ;
                }
                break;

            default:
                break;
        }

        event.Skip();
    });

    // Sort is according to first element in order vector. true means ascending order.
    // In case of tie, we use next next element in vector
    sortData = { this, { { Account, true }, { Address, true } }};

    addresses->Bind(wxEVT_LIST_COL_CLICK, [this](wxListEvent &ev) {
        Column column = static_cast<Column >(ev.GetColumn());

        int i;
        for(i=0; (i<sortData.order.size()) && (sortData.order[i].first != column); i++)
            ;

        if(i<sortData.order.size()) {
            std::pair<Column, bool> p = sortData.order[i];
            p.second = !p.second; // invert order

            // move clicked column to first position
            for(int j=i; j>0; j--)
                sortData.order[j] = sortData.order[j-1];
            sortData.order[0] = p;
            addresses->SortItems(cmpAddresses, (wxIntPtr) &sortData);
        }
    });
}

void AddressesPage::addAddress(const std::string &account, const std::string &address) {
    wxListItem item{};
    int id = addresses->GetItemCount();
    item.SetId(id);
    long index = addresses->InsertItem(item);

    addresses->SetItemPtrData(index, (wxUIntPtr) id);
    addresses->SetItem(index, Account, wxString(account));
    addresses->SetItem(index, Address, wxString(address));
    addresses->SortItems(cmpAddresses, (wxIntPtr) &sortData);
}
