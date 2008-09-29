/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file ProDeBusOrderDialog.cc
 *
 * Implementation of ProDeBusOrderDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <wx/statline.h>
#include <wx/listctrl.h>
#include "ProDeBusOrderDialog.hh"
#include "Machine.hh"
#include "WxConversion.hh"
#include "Bus.hh"

BEGIN_EVENT_TABLE(ProDeBusOrderDialog, wxDialog)
    EVT_BUTTON(ID_UP, ProDeBusOrderDialog::onUp)
    EVT_BUTTON(ID_DOWN, ProDeBusOrderDialog::onDown)
    EVT_LIST_ITEM_SELECTED(ID_LIST, ProDeBusOrderDialog::onBusSelectionChanged)
    EVT_LIST_ITEM_DESELECTED(ID_LIST, ProDeBusOrderDialog::onBusSelectionChanged)
    END_EVENT_TABLE()

/**
 * The Constructor.
 *
 * @param parent Parent window of the dialog.
 * @param machine Machine containing the buses to order.
 */
ProDeBusOrderDialog::ProDeBusOrderDialog(
    wxWindow* parent, TTAMachine::Machine& machine) :
    wxDialog(parent, -1, _T("Transport Bus Order"), wxDefaultPosition),
    machine_(machine) {

    createContents(this, true, true);

    list_ = dynamic_cast<wxListCtrl*>(FindWindow(ID_LIST));
    list_->InsertColumn(0, _T("index"), wxLIST_FORMAT_LEFT, 50);
    list_->InsertColumn(1, _T("bus"), wxLIST_FORMAT_LEFT, 180);

    // Disable conditional buttons.
    FindWindow(ID_UP)->Disable();
    FindWindow(ID_DOWN)->Disable();

    updateBusList();

}

/**
 * The Destructor.
 */
ProDeBusOrderDialog::~ProDeBusOrderDialog() {
}


/**
 * Updates the bus list order.
 */
void
ProDeBusOrderDialog::updateBusList() {
    list_->DeleteAllItems();
    const TTAMachine::Machine::BusNavigator& navigator =
        machine_.busNavigator();

    for (int i = 0; i < navigator.count(); i++) {
        std::string busName = navigator.item(i)->name();
        list_->InsertItem(i, WxConversion::toWxString(i));
        list_->SetItem(i, 1, WxConversion::toWxString(busName));
    }
}

/**
 * Moves the selected bus one position up (decreases the index by one).
 */
void
ProDeBusOrderDialog::onUp(wxCommandEvent&) {

    const TTAMachine::Machine::BusNavigator& navigator =
        machine_.busNavigator();

    int position = selectedBus();
    if (position <= 0) {
        return;
    }
    machine_.setBusPosition(*navigator.item(position), position - 1);

    updateBusList();

    // Reselect the moved bus.
    list_->SetItemState(
        position - 1, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);

    list_->EnsureVisible(position - 1);
}


/**
 * Moves the selected bus one position down (increases the index by one).
 */
void
ProDeBusOrderDialog::onDown(wxCommandEvent&) {

    const TTAMachine::Machine::BusNavigator& navigator =
        machine_.busNavigator();

    int position = selectedBus();
    if (position >= (navigator.count() - 1)) {
        return;
    }

    machine_.setBusPosition(*navigator.item(position), position + 1);

    updateBusList();

    // Reselect the moved bus.
    list_->SetItemState(
        position + 1, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);

    list_->EnsureVisible(position + 1);
}

/**
 * Enables and disables Up/Down buttons according to bus selection.
 */
void
ProDeBusOrderDialog::onBusSelectionChanged(wxListEvent&)  {

    const TTAMachine::Machine::BusNavigator& navigator =
        machine_.busNavigator();

    int selection = selectedBus();

    if (selection < 0) {
        FindWindow(ID_UP)->Disable();
        FindWindow(ID_DOWN)->Disable();
        return;
    }

    if (selection > 0) {
        FindWindow(ID_UP)->Enable();
    } else {
        FindWindow(ID_UP)->Disable();
    }

    if (selection < (navigator.count() - 1)) {
        FindWindow(ID_DOWN)->Enable();
    } else {
        FindWindow(ID_DOWN)->Disable();
    }
}

/**
 * Returns index of the selected bus, or -1 if no bs is selected.
 *
 * @return Index of bus selected in the bus list.
 */
int
ProDeBusOrderDialog::selectedBus() const {
    int item = -1;
    item = list_->GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    return item;
}


/**
 * Creates the dialog widgets.
 */
wxSizer*
ProDeBusOrderDialog::createContents(
    wxWindow* parent, bool call_fit, bool set_sizer) {

    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer *item1 = new wxBoxSizer( wxHORIZONTAL );

    wxListCtrl *item2 = new wxListCtrl( parent, ID_LIST, wxDefaultPosition, wxSize(250,300), wxLC_REPORT|wxLC_SINGLE_SEL|wxSUNKEN_BORDER );
    item1->Add( item2, 0, wxALIGN_CENTER|wxALL, 5 );

    wxBoxSizer *item3 = new wxBoxSizer( wxVERTICAL );

    wxButton *item4 = new wxButton( parent, ID_UP, wxT("&Up"), wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add( item4, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item5 = new wxButton( parent, ID_DOWN, wxT("&Down"), wxDefaultPosition, wxDefaultSize, 0 );
    item3->Add( item5, 0, wxALIGN_CENTER|wxALL, 5 );

    item1->Add( item3, 0, wxALIGN_BOTTOM|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

    item0->Add( item1, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticLine *item6 = new wxStaticLine( parent, ID_LINE, wxDefaultPosition, wxSize(20,-1), wxLI_HORIZONTAL );
    item0->Add( item6, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item7 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item8 = new wxButton( parent, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    item7->Add( item8, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item9 = new wxButton( parent, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    item7->Add( item9, 0, wxALIGN_CENTER|wxALL, 5 );

    item0->Add( item7, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }

    return item0;
}
