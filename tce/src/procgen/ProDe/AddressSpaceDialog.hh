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
 * @file AddressSpaceDialog.hh
 *
 * Declaration of AddressSpaceDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel@cs.tut.fi)
 */

#ifndef TTA_ADDRESS_SPACE_DIALOG_HH
#define TTA_ADDRESS_SPACE_DIALOG_HH

#include <wx/wx.h>
#include <wx/spinctrl.h>

class NumberControl;

namespace TTAMachine {
    class AddressSpace;
}

/**
 * Dialog for editing address space properties.
 */
class AddressSpaceDialog : public wxDialog {
public:
    AddressSpaceDialog(
        wxWindow* parent,
        TTAMachine::AddressSpace* addressSpace);
    ~AddressSpaceDialog();

private:
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);
    virtual bool TransferDataToWindow();
    void onName(wxCommandEvent& event);
    void onOK(wxCommandEvent& event);
    void onHelp(wxCommandEvent& event);
    void onMinAddress(wxCommandEvent& event);
    void onMinHex(wxCommandEvent& event);
    void onMaxAddress(wxCommandEvent& event);
    void onMaxHex(wxCommandEvent& event);
    void onMinToDec(wxCommandEvent& event);
    void onMinToHex(wxCommandEvent& event);
    void onMaxToDec(wxCommandEvent& event);
    void onMaxToHex(wxCommandEvent& event);
    void onBitWidth(wxSpinEvent& event);
    void onBitWidthText(wxCommandEvent& event);
    void setTexts();

    /// Address space to modify with the dialog.
    TTAMachine::AddressSpace* as_;
    /// Name of the address space.
    wxString name_;
    /// The bit width of the minimum addressable word.
    int width_;
    /// The lowest address in the address space.
    wxString minAddress_;
    /// The highest address in the address space.
    wxString maxAddress_;
    /// Widget for the lowest address hex value.
    NumberControl* minControl_;
    /// Widget for the highest address hex value.
    NumberControl* maxControl_;

    /// Boxsizer containing the name widget.
    wxStaticBoxSizer* nameSizer_;
    /// Boxsizer containing the min-address widget.
    wxStaticBoxSizer* minAddressSizer_;
    /// Boxsizer containing the max-address widget.
    wxStaticBoxSizer* maxAddressSizer_;
    /// Boxsizer containing the width widget.
    wxStaticBoxSizer* widthSizer_;
    /// SpinCtrl containing the bit width of the max-address.
    wxSpinCtrl *bitWidthSpinCtrl_;

    // IDs for dialog controls
    enum {
        ID_NAME = 10000,
        ID_WIDTH,
        ID_MIN_ADDRESS,
        ID_MAX_ADDRESS,
        ID_BIT_WIDTH,
        ID_RADIO_MIN_DEC,
        ID_RADIO_MIN_HEX,
        ID_RADIO_MAX_DEC,
        ID_RADIO_MAX_HEX,
        ID_TEXT,
        ID_LINE,
        ID_HELP
    };

    DECLARE_EVENT_TABLE()
};
#endif
