
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
 * @file EditParameterDialog.cc
 *
 * Definition of EditParameterDialog class.
 *
 * @author Viljami Korhonen 2008 (viljami.korhonen-no.spam-.tut.fi)
 * @note rating: red
 */

#include <string>
#include <wx/statline.h>
#include <wx/valgen.h>
#include <wx/spinctrl.h>
#include <boost/format.hpp>

#include "CallExplorerPlugin.hh"
#include "Conversion.hh"
#include "WxConversion.hh"
#include "WarningDialog.hh"
#include "AddressSpace.hh"
#include "ModelConstants.hh"
#include "NumberControl.hh"
#include "InformationDialog.hh"
#include "MachineTester.hh"
#include "GUITextGenerator.hh"
#include "WidgetTools.hh"
#include "ProDeTextGenerator.hh"
#include "DesignSpaceExplorer.hh"
#include "EditParameterDialog.hh"

using boost::format;
using std::string;
using namespace TTAMachine;

BEGIN_EVENT_TABLE(EditParameterDialog, wxDialog)
    EVT_BUTTON(wxID_CLOSE, EditParameterDialog::onClose)
    EVT_BUTTON(ID_CANCEL, EditParameterDialog::onClose)
    EVT_BUTTON(ID_OK, EditParameterDialog::onOk)
END_EVENT_TABLE()


/**
 * The Constructor.
 *
 * @param parent Parent window of the dialog.
 * @param addressSpace The address space to be modified with the dialog.
 */
EditParameterDialog::EditParameterDialog(
    wxWindow* parent, 
    ExplorerPluginParameter* parameter):
    wxDialog(parent, -1, _T(""), wxDefaultPosition), parameter_(parameter)
    {
    createContents(this, true, true);
        
    paramName_ = dynamic_cast<wxStaticText*>(FindWindow(ID_NAME));
    paramType_ = dynamic_cast<wxStaticText*>(FindWindow(ID_TYPE));
    paramValue_ = dynamic_cast<wxTextCtrl*>(FindWindow(ID_VALUE));

    // set widget texts
    setTexts();

    TransferDataToWindow();
}


/**
 * The Destructor.
 */
EditParameterDialog::~EditParameterDialog() {
}


/**
 * Sets texts for widgets.
 */
void
EditParameterDialog::setTexts() {
    ProDeTextGenerator* prodeTexts = ProDeTextGenerator::instance();

    // Dialog title
    format fmt = prodeTexts->text(
        ProDeTextGenerator::TXT_EDIT_PARAMETER_TITLE);
    SetTitle(WxConversion::toWxString(fmt.str()));
    
    paramName_->SetLabel(WxConversion::toWxString(parameter_->name()));
    paramType_->SetLabel(WxConversion::toWxString(parameter_->typeAsString()));
    paramValue_->Clear();
    paramValue_->AppendText(WxConversion::toWxString(parameter_->value()));
}


/**
 * Transfers data from the AddressSpace object to the dialog widgets.
 *
 * @return false, if an error occured in the transfer, true otherwise.
 */
bool
EditParameterDialog::TransferDataToWindow() {
    return wxWindow::TransferDataToWindow();
}


/**
 * Validates input in the controls, and updates the AddressSpace.
 */
void
EditParameterDialog::onClose(wxCommandEvent&) {
    EndModal(wxID_OK);
}


/**
 * Stores new value set by the user.
 */
void 
EditParameterDialog::onOk(wxCommandEvent&) {
    
    std::string value = WxConversion::toString(paramValue_->GetValue());
    
    parameter_->setValue(value);
    
    EndModal(wxID_OK);
}


/**
 * Creates the dialog window contents.
 *
 * This method was generated with wxDesigner, thus the ugly code and
 * too long lines.
 *
 * @return Main sizer of the created contents.
 * @param parent The dialog window.
 * @param call_fit If true, fits the contents inside the dialog.
 * @param set_sizer If true, sets the main sizer as dialog contents.
 */
wxSizer*
EditParameterDialog::createContents(
    wxWindow* parent,
    bool call_fit,
    bool set_sizer) {
        
    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer *item1 = new wxBoxSizer( wxHORIZONTAL );

    wxStaticText *item2 = new wxStaticText( parent, ID_TEXT, wxT("Parameter Name:"), wxDefaultPosition, wxDefaultSize, 0 );
    item1->Add( item2, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item3 = new wxStaticText( parent, ID_NAME, wxT("xxxxxxxxxxxxxxx"), wxDefaultPosition, wxSize(150,-1), 0 );
    item1->Add( item3, 0, wxALIGN_CENTER|wxALL, 5 );

    item0->Add(item1, 0, wxALIGN_RIGHT | wxALL, 5);

    wxBoxSizer *item4 = new wxBoxSizer( wxHORIZONTAL );

    wxStaticText *item5 = new wxStaticText( parent, ID_TEXT, wxT("Parameter Type:"), wxDefaultPosition, wxDefaultSize, 0 );
    item4->Add( item5, 0, wxALIGN_CENTER|wxALL, 5 );

    wxStaticText *item6 = new wxStaticText( parent, ID_TYPE, wxT("xxxxxxxxxxxx"), wxDefaultPosition, wxSize(150,-1), 0 );
    item4->Add( item6, 0, wxALIGN_CENTER|wxALL, 5 );

    item0->Add(item4, 0, wxALIGN_RIGHT | wxALL, 5);

    wxBoxSizer *item7 = new wxBoxSizer( wxHORIZONTAL );

    wxStaticText *item8 = new wxStaticText( parent, ID_TEXT, wxT("Value:"), wxDefaultPosition, wxDefaultSize, 0 );
    item7->Add( item8, 0, wxALIGN_CENTER|wxALL, 5 );

    wxTextCtrl *item9 = new wxTextCtrl( parent, ID_VALUE, wxT(""), wxDefaultPosition, wxSize(110,-1), 0 );
    item7->Add( item9, 0, wxALIGN_CENTER|wxALL, 5 );

    item0->Add( item7, 0, wxALIGN_CENTER|wxALL, 5 );

    wxBoxSizer *item10 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item11 = new wxButton( parent, ID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    item10->Add( item11, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item12 = new wxButton( parent, ID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    item10->Add(item12, 0, wxALL, 5);

    item0->Add( item10, 0, wxALIGN_CENTER|wxALL, 5 );

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }
    
    return item0;
}


