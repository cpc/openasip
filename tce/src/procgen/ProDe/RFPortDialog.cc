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
 * @file RFPortDialog.cc
 *
 * Definition of RFPortDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <wx/wx.h>
#include <wx/statline.h>
#include <boost/format.hpp>

#include "RegisterFile.hh"
#include "RFPortDialog.hh"
#include "WxConversion.hh"
#include "Conversion.hh"
#include "ProDeConstants.hh"
#include "Socket.hh"
#include "Machine.hh"
#include "MachineTester.hh"
#include "WarningDialog.hh"
#include "UserManualCmd.hh"
#include "Port.hh"
#include "WidgetTools.hh"
#include "GUITextGenerator.hh"
#include "ProDeTextGenerator.hh"
#include "InformationDialog.hh"

using boost::format;
using std::string;
using namespace TTAMachine;

BEGIN_EVENT_TABLE(RFPortDialog, wxDialog)
    EVT_TEXT(ID_NAME, RFPortDialog::onName)
    EVT_BUTTON(wxID_OK, RFPortDialog::onOK)
    EVT_BUTTON(wxID_CANCEL, RFPortDialog::onCancel)
    EVT_CHOICE(ID_INPUT_SOCKET, RFPortDialog::onSocketChoice)
    EVT_CHOICE(ID_OUTPUT_SOCKET, RFPortDialog::onSocketChoice)
END_EVENT_TABLE()

/**
 * The Constructor.
 *
 * @param parent Parent window of the dialog.
 * @param port Port to modify.
 */
RFPortDialog::RFPortDialog(
    wxWindow* parent,
    Port* port):
    wxDialog(parent, -1, _T(""), wxDefaultPosition),
    port_(port),
    name_(_T("")),
    inputSocketChoice_(NULL),
    outputSocketChoice_(NULL) {

    oldInput_ = port_->inputSocket();
    oldOutput_ = port_->outputSocket();
    createContents(this, true, true);
    FindWindow(wxID_OK)->Disable();

    // set widget texts
    setTexts();

    TransferDataToWindow();
}

/**
 * The Destructor.
 */
RFPortDialog::~RFPortDialog() {
}

/**
 * Sets texts for widgets.
 */
void
RFPortDialog::setTexts() {
    GUITextGenerator* generator = GUITextGenerator::instance();
    ProDeTextGenerator* prodeTexts = ProDeTextGenerator::instance();

    // Dialog title
    format fmt = prodeTexts->text(
        ProDeTextGenerator::TXT_RF_PORT_DIALOG_TITLE);
    SetTitle(WxConversion::toWxString(fmt.str()));

    // buttons
    WidgetTools::setLabel(generator, FindWindow(wxID_OK),
                          GUITextGenerator::TXT_BUTTON_OK);

    WidgetTools::setLabel(generator, FindWindow(wxID_CANCEL),
                          GUITextGenerator::TXT_BUTTON_CANCEL);

    WidgetTools::setLabel(generator, FindWindow(ID_HELP),
                          GUITextGenerator::TXT_BUTTON_HELP);

    // widget labels
    WidgetTools::setLabel(prodeTexts, FindWindow(ID_NAME),
                          ProDeTextGenerator::TXT_LABEL_NAME);

    WidgetTools::setLabel(prodeTexts, FindWindow(ID_INPUT_SOCKET),
                          ProDeTextGenerator::TXT_LABEL_INPUT_SOCKET);

    WidgetTools::setLabel(prodeTexts, FindWindow(ID_OUTPUT_SOCKET),
                          ProDeTextGenerator::TXT_LABEL_OUTPUT_SOCKET);
}

/**
 * Validates input in the controls, and updates the port.
 */
void
RFPortDialog::onOK(wxCommandEvent&) {

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

    // check whether RF already has a port of tht name.
    if (port_->name() != trimmedName) {
	Unit* rf = port_->parentUnit();
        for (int i = 0; i < rf->portCount(); i++) {
            string name = rf->port(i)->name();
            if (name == WxConversion::toString(name_)) {
                ProDeTextGenerator* prodeTexts =
                    ProDeTextGenerator::instance();
                format message =
                    prodeTexts->text(ProDeTextGenerator::MSG_ERROR_SAME_NAME);
                format a_port =
                    prodeTexts->text(ProDeTextGenerator::COMP_A_PORT);
                format rf =
                    prodeTexts->text(ProDeTextGenerator::COMP_REGISTER_FILE);
                format port =
                    prodeTexts->text(ProDeTextGenerator::COMP_PORT);
                message % trimmedName % a_port.str() % rf.str() % port.str();
                WarningDialog warning(
                    this, WxConversion::toWxString(message.str()));
                warning.ShowModal();
                return;
            }
        }
    }

    port_->setName(trimmedName);
    // update parent RFs max write and read parameters
    RegisterFile* RF = dynamic_cast<RegisterFile*>(port_->parentUnit());
    assert (RF != NULL);
    //RF->updateMaxReadsAndWrites();
    EndModal(wxID_OK);
}


/**
 * Resets the original output and input sockets for the port and closes the
 * dialog.
 */
void
RFPortDialog::onCancel(wxCommandEvent&) {
    port_->detachAllSockets();
    if (oldInput_ != NULL) {
        port_->attachSocket(*oldInput_);
    }
    if (oldOutput_ != NULL) {
        port_->attachSocket(*oldOutput_);
    }
    EndModal(wxID_CANCEL);
}

/**
 * Transfers data from the port object to the dialog widgets.
 *
 * @return False, if an error occured in the transfer.
 */
bool
RFPortDialog::TransferDataToWindow() {
    name_ = WxConversion::toWxString(port_->name());
    updateSockets();
    return wxWindow::TransferDataToWindow();
}

/**
 * Updates the port object when user changes input/output socket selection.
 */
void
RFPortDialog::onSocketChoice(wxCommandEvent&) {
    port_->detachAllSockets();
    Machine::SocketNavigator navigator =
        port_->parentUnit()->machine()->socketNavigator();

    // set input socket
    string inputSocketName =
        WxConversion::toString(inputSocketChoice_->GetStringSelection());
    if (inputSocketName != WxConversion::toString(ProDeConstants::NONE)) {
        port_->attachSocket(*(navigator.item(inputSocketName)));
    }

    // set output socket
    string outputSocketName =
	WxConversion::toString(outputSocketChoice_->GetStringSelection());
    if (outputSocketName != WxConversion::toString(ProDeConstants::NONE)) {
	port_->attachSocket(*(navigator.item(outputSocketName)));
    }
    updateSockets();
}

/**
 * Updates input and output socket choicers.
 */
void
RFPortDialog::updateSockets() {

    inputSocketChoice_->Clear();
    inputSocketChoice_->Append(ProDeConstants::NONE);
    outputSocketChoice_->Clear();
    outputSocketChoice_->Append(ProDeConstants::NONE);

    MachineTester tester(*(port_->parentUnit()->machine()));

    // Add ports to the choicers
    Machine::SocketNavigator navigator =
	port_->parentUnit()->machine()->socketNavigator();

    for (int i = 0; i < navigator.count(); i++) {
	Socket* socket = navigator.item(i);
	wxString socketName = WxConversion::toWxString(socket->name());

	// Add available input sockets.
	Socket* input = port_->inputSocket();
	if (input != NULL) {
	    port_->detachSocket(*input);
	}
	bool legal = tester.canConnect(*socket, *port_);
        if (legal && socket->direction() == Socket::INPUT) {
	    inputSocketChoice_->Append(socketName);
	}
	if (input != NULL) {
	    port_->attachSocket(*input);
	}

	// Add available output sockets.
	Socket* output = port_->outputSocket();
	if (output != NULL) {
	    port_->detachSocket(*output);
	}
        legal = tester.canConnect(*socket, *port_);
	if (legal && socket->direction() == Socket::OUTPUT) {
	    outputSocketChoice_->Append(socketName);
	}
	if (output != NULL) {
	    port_->attachSocket(*output);
	}
	// ignore sockets with unknown direction
    }

    // set input socket choice
    if (port_->inputSocket() == NULL) {
	inputSocketChoice_->SetStringSelection(ProDeConstants::NONE);
    } else {
	wxString socketName =
	    WxConversion::toWxString(port_->inputSocket()->name());
	inputSocketChoice_->SetStringSelection(socketName);
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
 * Validates input in the controls, and updates the ComponentDescriptor.
 */
void
RFPortDialog::onName(wxCommandEvent&) {
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
 * Creates contents of the dialog window. Initially generated with
 * wxDesigner, the code will be cleaned up later.
 *
 * @param parent Parent dialog of the contents.
 * @param call_fit If true, fits sizer in dialog window.
 * @param set_sizer If true, sets sizer as dialog's sizer.
 * @return Top level sizer of the contents.
 */
wxSizer*
RFPortDialog::createContents(
    wxWindow *parent, bool call_fit, bool set_sizer) {

    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );
    wxFlexGridSizer *item1 = new wxFlexGridSizer( 2, 0, 0 );

    // name element
    wxStaticText *item2 =
	new wxStaticText( parent, -1, wxT("Name:"),
			  wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item2, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );
    wxTextCtrl *item3 =
	new wxTextCtrl(parent, ID_NAME, wxT(""), wxDefaultPosition,
		       wxSize(160,-1), 0,
                       wxTextValidator(wxFILTER_ASCII, &name_) );
    item1->Add( item3, 0, wxALIGN_CENTER|wxALL, 5 );

    // input socket element
    wxStaticText *item4 =
	new wxStaticText(parent, -1, wxT("Input Socket:"),
			 wxDefaultPosition, wxDefaultSize, 0);
    item1->Add( item4, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );
    wxString *strs5 = (wxString*) NULL;
    inputSocketChoice_ =
	new wxChoice(parent, ID_INPUT_SOCKET, wxDefaultPosition,
		     wxSize(100,-1), 0, strs5, 0);
    item1->Add( inputSocketChoice_, 0,
                wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    // output socket element
    wxStaticText *item6 =
	new wxStaticText(parent, -1, wxT("Output Socket:"),
			 wxDefaultPosition, wxDefaultSize, 0);
    item1->Add( item6, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );
    wxString *strs7 = (wxString*) NULL;
    outputSocketChoice_ =
	new wxChoice(parent, ID_OUTPUT_SOCKET, wxDefaultPosition,
		     wxSize(100,-1), 0, strs7, 0);
    item1->Add( outputSocketChoice_, 0,
                wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );
    item0->Add( item1, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );
    wxStaticLine *item8 =
	new wxStaticLine(parent, -1, wxDefaultPosition, wxSize(20,-1),
			 wxLI_HORIZONTAL);
    item0->Add( item8, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );
    wxBoxSizer *item9 = new wxBoxSizer( wxHORIZONTAL );

    // buttons
    wxButton *item10 =
	new wxButton(parent, ID_HELP, wxT("&Help"), wxDefaultPosition,
		     wxDefaultSize, 0);
    item9->Add( item10, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item11 =
	new wxButton(parent, wxID_OK, wxT("&OK"), wxDefaultPosition,
		     wxDefaultSize, 0);
    item9->Add( item11, 0, wxALIGN_CENTER|wxALL, 5 );
    wxButton *item12 =
	new wxButton(parent, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition,
		     wxDefaultSize, 0);
    item9->Add( item12, 0, wxALIGN_CENTER|wxALL, 5 );
    item0->Add( item9, 0, wxALIGN_CENTER|wxALL, 5 );

    if (set_sizer) {
        parent->SetAutoLayout( TRUE );
        parent->SetSizer( item0 );
        if (call_fit)
        {
            item0->Fit( parent );
            item0->SetSizeHints( parent );
        }
    }
    return item0;
}
