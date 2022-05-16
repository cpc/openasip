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
 * @file SRPortDialog.cc
 *
 * Definition of SRPortDialog class.
 *
 * @author Veli-Pekka J��skel�inen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @author Pekka J��skel�inen 2021
 * @note rating: red
 */

#include <string>
#include <wx/wx.h>
#include <wx/statline.h>
#include <wx/spinctrl.h>
#include <wx/valgen.h>
#include <boost/format.hpp>

#include "Application.hh"
#include "SpecialRegisterPort.hh"
#include "SRPortDialog.hh"
#include "WxConversion.hh"
#include "Conversion.hh"
#include "ModelConstants.hh"
#include "ProDeConstants.hh"
#include "ProDeTextGenerator.hh"
#include "Socket.hh"
#include "Machine.hh"
#include "MachineTester.hh"
#include "WarningDialog.hh"
#include "UserManualCmd.hh"
#include "InformationDialog.hh"
#include "GUITextGenerator.hh"
#include "WidgetTools.hh"
#include "ControlUnit.hh"

using boost::format;
using std::string;
using namespace TTAMachine;

BEGIN_EVENT_TABLE(SRPortDialog, wxDialog)
    EVT_TEXT(ID_NAME, SRPortDialog::onName)
    EVT_BUTTON(wxID_OK, SRPortDialog::onOK)
    EVT_BUTTON(wxID_CANCEL, SRPortDialog::onCancel)
    EVT_CHOICE(ID_INPUT_SOCKET, SRPortDialog::onSocketChoice)
    EVT_CHOICE(ID_OUTPUT_SOCKET, SRPortDialog::onSocketChoice)
END_EVENT_TABLE()


/**
 * The Constructor.
 *
 * @param parent Parent window of the dialog.
 * @param port Port to modify.
 */
SRPortDialog::SRPortDialog(
    wxWindow* parent,
    SpecialRegisterPort* port):
    wxDialog(parent, -1, _T(""), wxDefaultPosition),
    port_(port),
    name_(_T("")),
    width_(ModelConstants::DEFAULT_WIDTH),
    inputSocketChoice_(NULL),
    outputSocketChoice_(NULL) {

    oldInput_ = port_->inputSocket();
    oldOutput_ = port_->outputSocket();

    createContents(this, true, true);

    inputSocketChoice_ =
	dynamic_cast<wxChoice*>(FindWindow(ID_INPUT_SOCKET));
    outputSocketChoice_ =
	dynamic_cast<wxChoice*>(FindWindow(ID_OUTPUT_SOCKET));

    FindWindow(ID_NAME)->SetValidator(wxTextValidator(wxFILTER_ASCII, &name_));
    FindWindow(ID_WIDTH)->SetValidator(wxGenericValidator(&width_));

    FindWindow(wxID_OK)->Disable();

    // set texts to widgets
    setTexts();

    TransferDataToWindow();
}

/**
 * The Destructor.
 */
SRPortDialog::~SRPortDialog() {
}


/**
 * Sets texts for widgets.
 */
void
SRPortDialog::setTexts() {
    GUITextGenerator* generator = GUITextGenerator::instance();
    ProDeTextGenerator* prodeTexts = ProDeTextGenerator::instance();

    // Dialog title
    format fmt = prodeTexts->text(
        ProDeTextGenerator::TXT_SR_PORT_DIALOG_TITLE);
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

    WidgetTools::setLabel(prodeTexts, FindWindow(ID_LABEL_WIDTH),
                          ProDeTextGenerator::TXT_LABEL_WIDTH);

    WidgetTools::setLabel(prodeTexts, FindWindow(ID_LABEL_INPUT_SOCKET),
                          ProDeTextGenerator::TXT_LABEL_INPUT_SOCKET);

    WidgetTools::setLabel(prodeTexts, FindWindow(ID_LABEL_OUTPUT_SOCKET),
                          ProDeTextGenerator::TXT_LABEL_OUTPUT_SOCKET);

}


/**
 * Transfers data from the port object to the dialog widgets.
 *
 * @return False, if an error occured in the transfer.
*/
bool
SRPortDialog::TransferDataToWindow() {

    name_ = WxConversion::toWxString(port_->name());
    width_ = port_->width();
    updateSockets();

    // wxWidgets GTK1 version seems to bug with spincontrol and
    // checkbox validators. The widget value has to be set manually.
    dynamic_cast<wxSpinCtrl*>(FindWindow(ID_WIDTH))->SetValue(width_);

    return wxWindow::TransferDataToWindow();
}


/**
 * Updates input and output socket choicers.
 */
void
SRPortDialog::updateSockets() {

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
 * Resets the original input and output sockets for the port and closes
 * the dialog.
 */
void
SRPortDialog::onCancel(wxCommandEvent&) {
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
 * Validates input in the controls, and updates the port object.
 */
void
SRPortDialog::onOK(wxCommandEvent&) {

    if (!Validate()) {
        return;
    }

    if (!TransferDataFromWindow()) {
        return;
    }

    // check whether FU already has a port of tht name.

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

	// TODO: Remove dyanmic_cast and assert when MOM return type is
	// fixed.
        ControlUnit* gcu = dynamic_cast<ControlUnit*>(port_->parentUnit());
	assert(gcu != NULL);

        for (int i = 0; i < gcu->portCount(); i++) {
            string name = gcu->port(i)->name();
            if (name == WxConversion::toString(name_)) {
                ProDeTextGenerator* prodeTexts =
                    ProDeTextGenerator::instance();
                format message =
                    prodeTexts->text(ProDeTextGenerator::MSG_ERROR_SAME_NAME);
                format a_port =
                    prodeTexts->text(ProDeTextGenerator::COMP_A_PORT);
                format machine =
                    prodeTexts->text(ProDeTextGenerator::COMP_FUNCTION_UNIT);
                format port =
                    prodeTexts->text(ProDeTextGenerator::COMP_PORT);
                message % trimmedName % a_port.str() % machine.str() %
                    port.str();
                WarningDialog warning(
                    this, WxConversion::toWxString(message.str()));
                warning.ShowModal();
                return;
            }
        }
    }

    // update attributes
    port_->setName(trimmedName);
    port_->setWidth(width_);

    EndModal(wxID_OK);
}

/**
 * Updates the port object when user changes input/output socket selection.
 */
void
SRPortDialog::onSocketChoice(wxCommandEvent&) {
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
 * Disables OK-button if the name field is empty.
 */
void
SRPortDialog::onName(wxCommandEvent&) {
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
SRPortDialog::createContents(
    wxWindow *parent, bool call_fit, bool set_sizer) {

    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    wxFlexGridSizer *item1 = new wxFlexGridSizer( 2, 0, 0 );

    wxStaticText *item2 = new wxStaticText( parent, ID_LABEL_NAME, wxT("Name:"), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item2, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxTextCtrl *item3 = new wxTextCtrl( parent, ID_NAME, wxT(""), wxDefaultPosition, wxSize(200,-1), 0 );
    item1->Add( item3, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item4 = new wxStaticText( parent, ID_LABEL_WIDTH, wxT("Width:"), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item4, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxSpinCtrl *item5 = new wxSpinCtrl( parent, ID_WIDTH, wxT("1"), wxDefaultPosition, wxSize(200,-1), 0, 1, 10000, 1 );
    item1->Add( item5, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item6 = new wxStaticText( parent, ID_LABEL_INPUT_SOCKET, wxT("Input Socket:"), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item6, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxString strs7[] =
    {
        wxT("NONE")
    };
    wxChoice *item7 = new wxChoice( parent, ID_INPUT_SOCKET, wxDefaultPosition, wxSize(200,-1), 1, strs7, 0 );
    item1->Add( item7, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item8 = new wxStaticText( parent, ID_LABEL_OUTPUT_SOCKET, wxT("Output Socket:"), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item8, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5 );

    wxString strs9[] =
    {
        wxT("NONE")
    };
    wxChoice *item9 = new wxChoice( parent, ID_OUTPUT_SOCKET, wxDefaultPosition, wxSize(200,-1), 1, strs9, 0 );
    item1->Add( item9, 0, wxALIGN_CENTER|wxALL, 5 );

    item0->Add( item1, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticLine *item10 = new wxStaticLine( parent, ID_LINE, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    item0->Add(item10, 0, wxGROW | wxALL, 5);

    wxBoxSizer *item11 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item12 = new wxButton( parent, ID_HELP, wxT("&Help"), wxDefaultPosition, wxDefaultSize, 0 );
    item11->Add( item12, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item13 = new wxButton( parent, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    item11->Add( item13, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item14 = new wxButton( parent, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    item11->Add( item14, 0, wxALIGN_CENTER|wxALL, 5 );

    item0->Add( item11, 0, wxALIGN_CENTER|wxALL, 5 );

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }

    return item0;
}
