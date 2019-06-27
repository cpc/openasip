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
 * @file IUPortDialog.cc
 *
 * Definition of IUPortDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel-no.spam-cs.tut.fi)
 */

#include <boost/format.hpp>

#include "IUPortDialog.hh"
#include "WxConversion.hh"
#include "Conversion.hh"
#include "ProDeConstants.hh"
#include "Socket.hh"
#include "Port.hh"
#include "Machine.hh"
#include "MachineTester.hh"
#include "WarningDialog.hh"
#include "UserManualCmd.hh"
#include "InformationDialog.hh"
#include "WidgetTools.hh"
#include "GUITextGenerator.hh"
#include "ProDeTextGenerator.hh"

using boost::format;
using std::string;
using namespace TTAMachine;

BEGIN_EVENT_TABLE(IUPortDialog, wxDialog)
    EVT_TEXT(ID_NAME, IUPortDialog::onName)
    EVT_BUTTON(wxID_OK, IUPortDialog::onOK)
END_EVENT_TABLE()

/**
 * The Constructor.
 *
 * @param parent Parent window of the dialog.
 * @param port Immediate unit port to be modified with the dialog.
 */
IUPortDialog::IUPortDialog(
    wxWindow* parent,
    Port* port):
    wxDialog(parent, -1, _T(""), wxDefaultPosition),
    port_(port),
    name_(_T("")),
    outputSocketChoice_(NULL) {

    createContents(this, true, true);
    FindWindow(wxID_OK)->Disable();

    outputSocketChoice_ =
        dynamic_cast<wxChoice*>(FindWindow(ID_OUTPUT_SOCKET));
    FindWindow(ID_NAME)->SetValidator(
        wxTextValidator(wxFILTER_ASCII, &name_));

    // set texts for widgets
    setTexts();

    TransferDataToWindow();
}


/**
 * The Destructor.
 */
IUPortDialog::~IUPortDialog() {
}


/**
 * Sets texts for widgets.
 */
void
IUPortDialog::setTexts() {
    GUITextGenerator* generator = GUITextGenerator::instance();
    ProDeTextGenerator* prodeTexts = ProDeTextGenerator::instance();

    // Dialog title
    format fmt = prodeTexts->text(
        ProDeTextGenerator::TXT_IU_PORT_DIALOG_TITLE);
    SetTitle(WxConversion::toWxString(fmt.str()));

    // buttons
    WidgetTools::setLabel(generator, FindWindow(wxID_OK),
                          GUITextGenerator::TXT_BUTTON_OK);

    WidgetTools::setLabel(generator, FindWindow(wxID_CANCEL),
                          GUITextGenerator::TXT_BUTTON_CANCEL);

    WidgetTools::setLabel(generator, FindWindow(ID_HELP),
                          GUITextGenerator::TXT_BUTTON_HELP);

    // widget labels
    WidgetTools::setLabel(prodeTexts, FindWindow(ID_LABEL_NAME),
                          ProDeTextGenerator::TXT_LABEL_NAME);

    WidgetTools::setLabel(prodeTexts, FindWindow(ID_LABEL_OUTPUT_SOCKET),
                          ProDeTextGenerator::TXT_LABEL_OUTPUT_SOCKET);
}


/**
 * Transfers data from the Port object to the dialog widgets.
 *
 * @return false, if an error occured in the transfer.
 */
bool
IUPortDialog::TransferDataToWindow() {
    name_ = WxConversion::toWxString(port_->name());

    updateSocket();

    return wxWindow::TransferDataToWindow();
}


/**
 * Updates output socket choicer.
 */
void
IUPortDialog::updateSocket() {

    outputSocketChoice_->Clear();
    outputSocketChoice_->Append(ProDeConstants::NONE);

    MachineTester tester(*(port_->parentUnit()->machine()));

    // Add ports to the choicer
    Machine::SocketNavigator navigator =
	port_->parentUnit()->machine()->socketNavigator();

    for (int i = 0; i < navigator.count(); i++) {
	Socket* socket = navigator.item(i);
	wxString socketName = WxConversion::toWxString(socket->name());

	// Add available output sockets.
	Socket* output = port_->outputSocket();
	if (output != NULL) {
	    port_->detachSocket(*output);
	}
        bool legal = tester.canConnect(*socket, *port_);
	if (legal && socket->direction() == Socket::OUTPUT) {
	    outputSocketChoice_->Append(socketName);
	}
	if (output != NULL) {
	    port_->attachSocket(*output);
	}
	// ignore sockets with unknown direction
    }

    // set output socket choice
    if (port_->outputSocket() == NULL) {
	outputSocketChoice_->SetStringSelection(ProDeConstants::NONE);
    } else {
	wxString socketName =
	    WxConversion::toWxString(port_->outputSocket()->name());
	outputSocketChoice_->SetStringSelection(socketName);
    }
}



/**
 * Validates input in the controls, and updates the port.
 */
void
IUPortDialog::onOK(wxCommandEvent&) {

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

    if (port_->name() != trimmedName) {
	Unit* iu = port_->parentUnit();
        for (int i = 0; i < iu->portCount(); i++) {
            string name = iu->port(i)->name();
            if (name == WxConversion::toString(name_)) {
                ProDeTextGenerator* prodeTexts =
                    ProDeTextGenerator::instance();
                format message =
                    prodeTexts->text(ProDeTextGenerator::MSG_ERROR_SAME_NAME);
                format a_port =
                    prodeTexts->text(ProDeTextGenerator::COMP_A_PORT);
                format iu =
                    prodeTexts->text(ProDeTextGenerator::COMP_IMMEDIATE_UNIT);
                format port =
                    prodeTexts->text(ProDeTextGenerator::COMP_PORT);
                message % trimmedName % a_port.str() % iu.str() % port.str();
                WarningDialog warning(
                    this, WxConversion::toWxString(message.str()));
                warning.ShowModal();
                return;
            }
        }
    }

    port_->setName(trimmedName);
    port_->detachAllSockets();
    Machine::SocketNavigator navigator =
	port_->parentUnit()->machine()->socketNavigator();

    // set output socket
    string outputSocketName =
	WxConversion::toString(outputSocketChoice_->GetStringSelection());
    if (outputSocketName != WxConversion::toString(ProDeConstants::NONE)) {
	port_->attachSocket(*(navigator.item(outputSocketName)));
    }

    EndModal(wxID_OK);
}


/**
 * Validates input in the controls, and updates the port.
 */
void
IUPortDialog::onName(wxCommandEvent&) {
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
IUPortDialog::createContents(
    wxWindow *parent, bool call_fit, bool set_sizer) {

    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    wxFlexGridSizer *item1 = new wxFlexGridSizer( 2, 0, 0 );

    wxStaticText *item2 = new wxStaticText( parent, ID_LABEL_NAME, wxT("Name:"), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item2, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxTextCtrl *item3 = new wxTextCtrl( parent, ID_NAME, wxT(""), wxDefaultPosition, wxSize(140,-1), 0 );
    item1->Add( item3, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticText *item4 = new wxStaticText( parent, ID_LABEL_OUTPUT_SOCKET, wxT("Output Socket"), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item4, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxString *strs5 = (wxString*) NULL;
    wxChoice *item5 = new wxChoice( parent, ID_OUTPUT_SOCKET, wxDefaultPosition, wxSize(100,-1), 0, strs5, 0 );
    item1->Add( item5, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    item0->Add( item1, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxStaticLine *item6 = new wxStaticLine( parent, ID_LINE, wxDefaultPosition, wxSize(20,-1), wxLI_HORIZONTAL );
    item0->Add( item6, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxBoxSizer *item7 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item8 = new wxButton( parent, ID_HELP, wxT("&Help"), wxDefaultPosition, wxDefaultSize, 0 );
    item7->Add( item8, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item9 = new wxButton( parent, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    item7->Add( item9, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item10 = new wxButton( parent, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    item7->Add( item10, 0, wxALIGN_CENTER|wxALL, 5 );

    item0->Add( item7, 0, wxALIGN_CENTER|wxALL, 5 );

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }

    return item0;
}
