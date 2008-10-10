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
 * @file MemoryValueDialog.cc
 *
 * Definition of MemoryValueDialog class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "MemoryValueDialog.hh"
#include "NumberControl.hh"
#include "InformationDialog.hh"
#include "Conversion.hh"
#include "WxConversion.hh"

using std::string;

BEGIN_EVENT_TABLE(MemoryValueDialog, wxDialog)
    EVT_BUTTON(wxID_OK, MemoryValueDialog::onOK)
END_EVENT_TABLE()

/**
 * Constructor.
 *
 * @param parent Parent window.
 * @param pos Position of the dialog.
 */
MemoryValueDialog::MemoryValueDialog(wxWindow* parent, unsigned maxBits) :
    wxDialog(parent, -1, _T("Memory value"), wxDefaultPosition,
             wxDefaultSize),
    maxBits_(maxBits) {

    createContents(this, true, true);
    value_ = dynamic_cast<NumberControl*>(FindWindow(ID_VALUE));
}

/**
 * Destructor.
 */
MemoryValueDialog::~MemoryValueDialog() {
}

/**
 * Returns the mode of the number control.
 *
 * @return The mode.
 */
long
MemoryValueDialog::mode() {
    return value_->mode();
}

/**
 * Sets the value.
 *
 * @param value Value to be set.
 */
void
MemoryValueDialog::setValue(int value) {
    value_->setValue(value);
}

/**
 * Returns the value user typed.
 *
 * @return User given value.
 */
int
MemoryValueDialog::intValue() {
    return value_->intValue();
}

/**
 * Returns the value user typed.
 *
 * @return User given value as double.
 */
double
MemoryValueDialog::doubleValue() {
    return value_->doubleValue();
}

/**
 * Event handler for the OK button.
 *
 * Checks that the value doesn't use more than maximum number of bits allowed.
 */
void
MemoryValueDialog::onOK(wxCommandEvent&) {

    std::string binary = Conversion::toBinary(intValue());
    if (binary.length() > maxBits_) {
        wxString message = _T("Bit width of the value exceeds the maximum");
        message.Append(_T(" bit width of "));
        message.Append(WxConversion::toWxString(maxBits_));
        message.Append(_T("."));
        InformationDialog dialog(this, message);
        dialog.ShowModal();
        return;
    }

    EndModal(wxID_OK);
}

/**
 * Creates the contents of the dialog.
 *
 * @param parent The parent window.
 * @param call_fit If true fits the contents inside the dialog.
 * @param set_sizer If true sets the main sizer as dialog contents.
 * @return The created sizer.
 */
wxSizer*
MemoryValueDialog::createContents(
    wxWindow* parent,
    bool call_fit,
    bool set_sizer) {

    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    wxWindow *item1 = new NumberControl(
        parent, ID_VALUE, wxDefaultPosition,
        wxSize(140, 20), NumberControl::MODE_BINARY |
        NumberControl::MODE_UNSIGNED | NumberControl::MODE_HEXADECIMAL |
        NumberControl::MODE_FLOAT);

    item0->Add( item1, 0, wxALIGN_CENTER|wxALL, 5 );

    wxBoxSizer *item2 = new wxBoxSizer( wxHORIZONTAL );

    wxButton *item3 = new wxButton( parent, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item3, 0, wxALIGN_CENTER|wxALL, 5 );

    wxButton *item4 = new wxButton( parent, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item4, 0, wxALIGN_CENTER|wxALL, 5 );

    item0->Add( item2, 0, wxALIGN_CENTER|wxALL, 5 );

    if (set_sizer)
    {
        parent->SetSizer( item0 );
        if (call_fit)
            item0->SetSizeHints( parent );
    }

    return item0;
}
