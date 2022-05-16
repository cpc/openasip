/*
    Copyright (c) 2002-2009 Tampere University.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
/**
 * @file SocketDialog.cc
 *
 * Definition of SocketDialog class.
 *
 * @author Veli-Pekka J��skel�inen 2004 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <boost/format.hpp>

#include "Application.hh"
#include "SocketDialog.hh"
#include "ModelConstants.hh"
#include "WxConversion.hh"
#include "Machine.hh"
#include "WarningDialog.hh"
#include "Socket.hh"
#include "Bus.hh"
#include "Segment.hh"
#include "Port.hh"
#include "UserManualCmd.hh"
#include "ProDeConstants.hh"
#include "Machine.hh"
#include "MachineTester.hh"
#include "MachineTestReporter.hh"
#include "WidgetTools.hh"
#include "ProDeTextGenerator.hh"
#include "InformationDialog.hh"
#include "GUITextGenerator.hh"
#include "ObjectState.hh"

using boost::format;
using std::string;
using namespace TTAMachine;

BEGIN_EVENT_TABLE(SocketDialog, wxDialog)

    EVT_BUTTON(wxID_OK, SocketDialog::onOK)
    EVT_TEXT(ID_NAME, SocketDialog::onName)

    EVT_BUTTON(ID_ATTACH, SocketDialog::onAttach)
    EVT_BUTTON(ID_DETACH, SocketDialog::onDetach)

    EVT_RADIOBOX(ID_DIRECTION, SocketDialog::onDirection)

    EVT_LIST_ITEM_FOCUSED(ID_ATTACHED_LIST, SocketDialog::onAttachedSelection)
    EVT_LIST_DELETE_ITEM(ID_ATTACHED_LIST, SocketDialog::onAttachedSelection)
    EVT_LIST_ITEM_SELECTED(ID_ATTACHED_LIST, SocketDialog::onAttachedSelection)
    EVT_LIST_ITEM_DESELECTED(ID_ATTACHED_LIST, SocketDialog::onAttachedSelection)

    EVT_LIST_ITEM_FOCUSED(ID_DETACHED_LIST, SocketDialog::onDetachedSelection)
    EVT_LIST_DELETE_ITEM(ID_DETACHED_LIST, SocketDialog::onDetachedSelection)
    EVT_LIST_ITEM_SELECTED(ID_DETACHED_LIST, SocketDialog::onDetachedSelection)
    EVT_LIST_ITEM_DESELECTED(ID_DETACHED_LIST, SocketDialog::onDetachedSelection)

    // too long lines to keep doxygen quiet
END_EVENT_TABLE()


/**
 * The Constructor.
 *
 * @param parent Parent window of the dialog.
 * @param socket Socket to be modified with the dialog.
 */
SocketDialog::SocketDialog(
    wxWindow* parent,
    Socket* socket):
    wxDialog(parent, -1, _T(""), wxDefaultPosition),
    socket_(socket),
    directionBox_(NULL),
    connectedListCtrl_(NULL),
    segmentListCtrl_(NULL),
    attachedSizer_(NULL),
    detachedSizer_(NULL) {

    createContents(this, true, true);
    tester_ = new MachineTester(*socket_->machine());

    name_ = WxConversion::toWxString(socket_->name());

    FindWindow(wxID_OK)->Disable();
    FindWindow(ID_ATTACH)->Disable();
    FindWindow(ID_DETACH)->Disable();

    // Set widget pointers.
    directionBox_ = dynamic_cast<wxRadioBox*>(FindWindow(ID_DIRECTION));
    connectedListCtrl_ =
        dynamic_cast<wxListCtrl*>(FindWindow(ID_ATTACHED_LIST));
    segmentListCtrl_ =
        dynamic_cast<wxListCtrl*>(FindWindow(ID_DETACHED_LIST));

    // Set widget validators.
    FindWindow(ID_NAME)->SetValidator(wxTextValidator(wxFILTER_ASCII, &name_));

    // sets labels for widgets
    setTexts();

    TransferDataToWindow();
}


/**
 * The Destructor.
 */
SocketDialog::~SocketDialog() {
    delete tester_;
}


/**
 * Sets texts for widgets.
 */
void
SocketDialog::setTexts() {
    GUITextGenerator* generator = GUITextGenerator::instance();
    ProDeTextGenerator* prodeTexts = ProDeTextGenerator::instance();

    // Dialog title
    format fmt = prodeTexts->text(ProDeTextGenerator::TXT_SOCKET_DIALOG_TITLE);
    SetTitle(WxConversion::toWxString(fmt.str()));

    // buttons
    WidgetTools::setLabel(generator, FindWindow(wxID_OK),
                          GUITextGenerator::TXT_BUTTON_OK);

    WidgetTools::setLabel(generator, FindWindow(wxID_CANCEL),
                          GUITextGenerator::TXT_BUTTON_CANCEL);

    WidgetTools::setLabel(generator, FindWindow(ID_HELP),
                          GUITextGenerator::TXT_BUTTON_HELP);

    WidgetTools::setLabel(prodeTexts, FindWindow(ID_ATTACH),
                          ProDeTextGenerator::TXT_BUTTON_ATTACH);

    WidgetTools::setLabel(prodeTexts, FindWindow(ID_DETACH),
                          ProDeTextGenerator::TXT_BUTTON_DETACH);

    // widget labels
    WidgetTools::setLabel(prodeTexts, FindWindow(ID_LABEL_NAME),
                          ProDeTextGenerator::TXT_LABEL_NAME);

    WidgetTools::setLabel(prodeTexts, FindWindow(ID_DIRECTION),
                          ProDeTextGenerator::TXT_LABEL_DIRECTION);

    // box sizer label
    fmt = prodeTexts->text(ProDeTextGenerator::TXT_SOCKET_ATTACHED_BOX);
    WidgetTools::setWidgetLabel(attachedSizer_, fmt.str());

    fmt = prodeTexts->text(ProDeTextGenerator::TXT_SOCKET_DETACHED_BOX);
    WidgetTools::setWidgetLabel(detachedSizer_, fmt.str());

    // Create list columns.
    wxListCtrl* attachedList =
        dynamic_cast<wxListCtrl*>(FindWindow(ID_ATTACHED_LIST));
    fmt = prodeTexts->text(ProDeTextGenerator::TXT_COLUMN_BUS);
    attachedList->InsertColumn(0, WxConversion::toWxString(fmt.str()),
                               wxLIST_FORMAT_LEFT, 100);
    fmt = prodeTexts->text(ProDeTextGenerator::TXT_COLUMN_SEGMENT);
    attachedList->InsertColumn(1, WxConversion::toWxString(fmt.str()),
                               wxLIST_FORMAT_LEFT, 80);

    wxListCtrl* detachedList =
        dynamic_cast<wxListCtrl*>(FindWindow(ID_DETACHED_LIST));
    fmt = prodeTexts->text(ProDeTextGenerator::TXT_COLUMN_BUS);
    detachedList->InsertColumn(0, WxConversion::toWxString(fmt.str()),
                               wxLIST_FORMAT_LEFT, 100);
    fmt = prodeTexts->text(ProDeTextGenerator::TXT_COLUMN_SEGMENT);
    detachedList->InsertColumn(1, WxConversion::toWxString(fmt.str()),
                               wxLIST_FORMAT_LEFT, 80);

    // Radio button labels
    fmt = prodeTexts->text(ProDeTextGenerator::TXT_RADIO_DIRECTION_INPUT);
    directionBox_->SetString(0, WxConversion::toWxString(fmt.str()));
    fmt = prodeTexts->text(ProDeTextGenerator::TXT_RADIO_DIRECTION_OUTPUT);
    directionBox_->SetString(1, WxConversion::toWxString(fmt.str()));
}


/**
 * Transfers data from the socket object to the dialog widgets.
 */
bool
SocketDialog::TransferDataToWindow() {
    // Socket direction.
    updateDirection();
    updateConnected();
    return wxDialog::TransferDataToWindow();
}


/**
 * Updates the direction choicer.
 */
void
SocketDialog::updateDirection() {
    if (socket_->direction() == Socket::INPUT) {
        directionBox_->SetStringSelection(
            ProDeConstants::SOCKET_DIRECTION_INPUT);
    } else if (socket_->direction() == Socket::OUTPUT) {
        directionBox_->SetStringSelection(
            ProDeConstants::SOCKET_DIRECTION_OUTPUT);
    }

}


/**
 * Validates input in the controls, and updates the Socket.
 */
void
SocketDialog::onOK(wxCommandEvent&) {

    if (!Validate()) {
        return;
    }
    if (!TransferDataFromWindow()) {
        return;
    }

    string trimmedName =
        WxConversion::toString(name_.Trim(false).Trim(true));

    // Check the name validity.
    if (!MachineTester::isValidComponentName(trimmedName)) {
        ProDeTextGenerator* prodeTexts = ProDeTextGenerator::instance();
        format message =
            prodeTexts->text(ProDeTextGenerator::MSG_ERROR_ILLEGAL_NAME);
        InformationDialog warning(
            this, WxConversion::toWxString(message.str()));
        warning.ShowModal();
        return;
    }

    if (trimmedName != socket_->name()) {
        Machine::SocketNavigator navigator =
            socket_->machine()->socketNavigator();
        for (int i = 0; i < navigator.count(); i++) {
            Socket* socket = navigator.item(i);
            if (trimmedName == socket->name()) {
                ProDeTextGenerator* prodeTexts =
                    ProDeTextGenerator::instance();
                format message =
                    prodeTexts->text(ProDeTextGenerator::MSG_ERROR_SAME_NAME);
                format a_soc =
                    prodeTexts->text(ProDeTextGenerator::COMP_A_SOCKET);
                format machine =
                    prodeTexts->text(ProDeTextGenerator::COMP_MACHINE);
                format soc =
                    prodeTexts->text(ProDeTextGenerator::COMP_SOCKET);
                message %
                    trimmedName % a_soc.str() % machine.str() % soc.str();
                WarningDialog warning(
                    this, WxConversion::toWxString(message.str()));
                warning.ShowModal();
                return;
            }
        }
    }
    socket_->setName(trimmedName);
    EndModal(wxID_OK);
}


/**
 * Enables and disables OK button based on input in the socket name.
 */
void
SocketDialog::onName(wxCommandEvent&) {
    if (!TransferDataFromWindow()) {
        assert(false);
    }
    wxString trimmedName = name_.Trim(false).Trim(true);
    if (trimmedName == _T("")) {
        FindWindow(wxID_OK)->Disable();
    } else {
        FindWindow(wxID_OK)->Enable();
    }
}


/**
 * Checks whether socket direction can be changed.
 *
 * Due to the port connections to the sockets it might be
 * impossible. If the socket is connected to the port already
 * containing a connection to socket of this direction, changing the
 * direction is not allowed.
 */
void
SocketDialog::onDirection(wxCommandEvent&) {

    wxString direction = directionBox_->GetStringSelection();

    if (direction.IsSameAs(ProDeConstants::SOCKET_DIRECTION_INPUT) &&
        !tester_->canSetDirection(*socket_, Socket::INPUT)) {

        // It wasn't legal to change the direction to input,
        // display an error message.
        string message =
            MachineTestReporter::socketDirectionSettingError(
                *socket_, Socket::INPUT, *tester_);

        WarningDialog dialog(this, WxConversion::toWxString(message));
        dialog.ShowModal();
        directionBox_->SetStringSelection(
            ProDeConstants::SOCKET_DIRECTION_OUTPUT);

    } else if(direction.IsSameAs(ProDeConstants::SOCKET_DIRECTION_OUTPUT) &&
        !tester_->canSetDirection(*socket_, Socket::OUTPUT)) {

        // It wasn't legal to change the direction to output,
        // display an error message.
        string message =
            MachineTestReporter::socketDirectionSettingError(
                *socket_, Socket::OUTPUT, *tester_);

        WarningDialog dialog(this, WxConversion::toWxString(message));

        dialog.ShowModal();
        directionBox_->SetStringSelection(
            ProDeConstants::SOCKET_DIRECTION_INPUT);
    }

    direction = directionBox_->GetStringSelection();

    if (direction.IsSameAs(ProDeConstants::SOCKET_DIRECTION_INPUT)) {
        socket_->setDirection(Socket::INPUT);
    }
    if (direction.IsSameAs(ProDeConstants::SOCKET_DIRECTION_OUTPUT)) {
        socket_->setDirection(Socket::OUTPUT);
    }
}


/**
 * Attaches selected segments on the segment list to the socket.
 */
void
SocketDialog::onAttach(wxCommandEvent&) {

    long item = -1;
    item = segmentListCtrl_->GetNextItem(
        item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

    // Socket state is saved, so it can be restored if the socket can't
    // be connected to all selected segments.
    ObjectState* savedState = socket_->saveState();
    MachineTester tester(*socket_->machine());

    // Try to connect socket to all selected segments.
    while (item != -1) {

        Segment* seg = segment(segmentListCtrl_, item);
        assert(seg != NULL);

        if (!tester.canConnect(*socket_, *seg)) {
            // Socket couldn't be connected to all selected segments.
            std::string error =
                MachineTestReporter::socketSegmentConnectionError(
                    *socket_, *seg, tester);
            wxString message = WxConversion::toWxString(error);
            InformationDialog dialog(this, message);
            dialog.ShowModal();
            socket_->loadState(savedState);
            break;
        }
        socket_->attachBus(*seg);
        item = segmentListCtrl_->GetNextItem(
            item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    }
    delete savedState;
    savedState = NULL;

    updateConnected();
    updateDirection();

    // Select the first item in the segment list (if possible) to improve
    // the dialog usability.
    if (segmentListCtrl_->GetItemCount() > 0) {
        segmentListCtrl_->SetItemState(
            0, wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED,
            wxLIST_STATE_SELECTED | wxLIST_STATE_FOCUSED);
    }

    wxListEvent dummy;
    onDetachedSelection(dummy);

}

/**
 * Detaches selected segments on the connection list from the socket.
 */
void
SocketDialog::onDetach(wxCommandEvent&) {

    long item = -1;
    item = connectedListCtrl_->GetNextItem(
        item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);

    // Detach socket from all selected segments.
    while (item != -1) {
        Segment* seg = segment(connectedListCtrl_, item);
        assert(seg != NULL);
        socket_->detachBus(*seg);
        item = connectedListCtrl_->GetNextItem(
            item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    }

    updateConnected();
    updateDirection();
    wxListEvent dummy;
    onAttachedSelection(dummy);

}

/**
 * Returns pointer to a segment with given index on a list.
 *
 * @param listCtrl List containing the segment.
 * @param index List index of the segment.
 * @return Pointer to the segment.
 */
Segment*
SocketDialog::segment(wxListCtrl* listCtrl, int index) {

    wxListItem busItem;
    busItem.SetId(index);
    busItem.SetColumn(0);
    listCtrl->GetItem(busItem);
    string busName = WxConversion::toString(busItem.GetText());

    wxListItem segItem;
    segItem.SetId(index);
    segItem.SetColumn(1);
    listCtrl->GetItem(segItem);
    string segmentName = WxConversion::toString(segItem.GetText());

    Machine::BusNavigator navigator = socket_->machine()->busNavigator();
    Segment* segment = navigator.item(busName)->segment(segmentName);
    return segment;
}



/**
 * Updates connected list elements.
 */
void
SocketDialog::updateConnected() {

    connectedListCtrl_->DeleteAllItems();
    segmentListCtrl_->DeleteAllItems();

    // update connections list
    for (int i = 0; i < socket_->segmentCount(); i++) {

        Segment* segment = socket_->segment(i);

        wxString segmentName =
            WxConversion::toWxString(segment->name());
        wxString busName =
            WxConversion::toWxString(segment->parentBus()->name());

        connectedListCtrl_->InsertItem(i, busName);
        connectedListCtrl_->SetItem(i, 1, segmentName);
    }

    // update list of segments available for connection
    Machine::BusNavigator navigator = socket_->machine()->busNavigator();
    int segments = 0;
    for (int i = 0; i < navigator.count(); i++) {
        Bus* bus = navigator.item(i);
        for (int j = 0; j < bus->segmentCount(); j++) {
            Segment* segment = bus->segment(j);
            if (tester_->canConnect(*socket_, *segment)) {
                wxString segmentName =
                    WxConversion::toWxString(segment->name());
                wxString busName =
                    WxConversion::toWxString(segment->parentBus()->name());

                segmentListCtrl_->InsertItem(segments, busName);
                segmentListCtrl_->SetItem(segments, 1, segmentName);
                segments++;
            }
        }
    }

    // Disable the direction control if the socket is not
    // connected to a bus.
    if (socket_->segmentCount() > 0) {
        FindWindow(ID_DIRECTION)->Enable();
    } else {
        FindWindow(ID_DIRECTION)->Disable();
    }
}


/**
 * Disables and enables Detach button under the connected list.
 *
 * If a (bus, segment) pair is selected, button is enabled. If no
 * (bus, segment) pair is selected the button will be disabled.
 */
void
SocketDialog::onAttachedSelection(wxListEvent&) {
    if (connectedListCtrl_->GetSelectedItemCount() < 1) {
        FindWindow(ID_DETACH)->Disable();
        return;
    }
    FindWindow(ID_DETACH)->Enable();
}


/**
 * Disables and enables Attach button under the connected list.
 *
 * If a (bus, segment) pair is selected, button is enabled. If no
 * (bus, segment) pair is selected the button will be disabled.
 */
void
SocketDialog::onDetachedSelection(wxListEvent&) {
    if (segmentListCtrl_->GetSelectedItemCount() < 1) {
        FindWindow(ID_ATTACH)->Disable();
        return;
    }
    FindWindow(ID_ATTACH)->Enable();
}


/**
 * Creates the dialog window contents.
 *
 * This method was initially generated with wxDesigner, code will be
 * cleaned up later.
 *
 * @return Main sizer of the created contents.
 * @param parent The dialog window.
 * @param call_fit If true, fits the contents inside the dialog.
 * @param set_sizer If true, sets the main sizer as dialog contents.
 */
wxSizer*
SocketDialog::createContents(
    wxWindow *parent, bool call_fit, bool set_sizer) {

    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    wxGridSizer *item1 = new wxGridSizer( 2, 0, 0 );

    wxBoxSizer *item2 = new wxBoxSizer( wxHORIZONTAL );

    wxStaticText *item3 = new wxStaticText( parent, ID_LABEL_NAME, wxT("Name:"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item3, 0, wxALIGN_CENTER|wxALL, 5 );

    wxTextCtrl *item4 = new wxTextCtrl( parent, ID_NAME, wxT(""), wxDefaultPosition, wxSize(120,-1), 0 );
    item2->Add( item4, 0, wxALIGN_CENTER|wxALL, 5 );

    item1->Add(item2, 0, wxGROW | wxALL, 5);

    wxString strs5[] =
    {
        wxT("Input"),
        wxT("Output")
    };
    wxRadioBox *item5 = new wxRadioBox( parent, ID_DIRECTION, wxT("Direction:"), wxDefaultPosition, wxDefaultSize, 2, strs5, 1, wxRA_SPECIFY_COLS );
    item1->Add(item5, 0, wxGROW | wxALL, 5);

    item0->Add(item1, 0, wxGROW | wxALL, 5);

    wxBoxSizer *item6 = new wxBoxSizer( wxHORIZONTAL );

    wxStaticBox *item8 = new wxStaticBox( parent, -1, wxT("Attached buses:") );
    wxStaticBoxSizer *item7 = new wxStaticBoxSizer( item8, wxVERTICAL );
    attachedSizer_ = item7;

    wxListCtrl *item9 = new wxListCtrl( parent, ID_ATTACHED_LIST, wxDefaultPosition, wxSize(200,120), wxLC_REPORT|wxSUNKEN_BORDER );
    item7->Add( item9, 0, wxALIGN_CENTER|wxALL, 5 );

    item6->Add( item7, 0, wxALIGN_CENTER|wxALL, 5 );

    wxBoxSizer *item10 = new wxBoxSizer( wxVERTICAL );

    wxButton *item11 = new wxButton( parent, ID_ATTACH, wxT("&Attach"), wxDefaultPosition, wxDefaultSize, 0 );
    item10->Add( item11, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item12 = new wxButton( parent, ID_DETACH, wxT("&Detach"), wxDefaultPosition, wxDefaultSize, 0 );
    item10->Add( item12, 0, wxALIGN_CENTER|wxALL, 5 );

    item6->Add( item10, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticBox *item14 = new wxStaticBox( parent, -1, wxT("Detached buses:") );
    wxStaticBoxSizer *item13 = new wxStaticBoxSizer( item14, wxVERTICAL );
    detachedSizer_ = item13;

    wxListCtrl *item15 = new wxListCtrl( parent, ID_DETACHED_LIST, wxDefaultPosition, wxSize(200,120), wxLC_REPORT|wxSUNKEN_BORDER );
    item13->Add( item15, 0, wxALIGN_CENTER|wxALL, 5 );

    item6->Add( item13, 0, wxALIGN_CENTER|wxALL, 5 );

    item0->Add( item6, 0, wxALIGN_CENTER, 5 );

    wxStaticLine *item16 = new wxStaticLine( parent, ID_LINE, wxDefaultPosition, wxSize(20,-1), wxLI_HORIZONTAL );
    item0->Add(item16, 0, wxGROW | wxALL, 5);

    wxGridSizer *item17 = new wxGridSizer( 2, 0, 0 );

    wxButton *item18 = new wxButton( parent, ID_HELP, wxT("&Help"), wxDefaultPosition, wxDefaultSize, 0 );
    item17->Add( item18, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item19 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item20 = new wxButton( parent, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    item19->Add( item20, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item21 = new wxButton( parent, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    item19->Add( item21, 0, wxALIGN_CENTER|wxALL, 5 );

    item17->Add(item19, 0, wxALIGN_RIGHT | wxALL, 5);

    item0->Add(item17, 0, wxGROW | wxALL, 5);

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }

    return item0;
}
