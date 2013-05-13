/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file AddressSpaceDialog.hh
 *
 * Declaration of AddressSpaceDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel-no.spam-cs.tut.fi)
 */

#ifndef TTA_ADDRESS_SPACE_DIALOG_HH
#define TTA_ADDRESS_SPACE_DIALOG_HH

#include <wx/wx.h>
#include <wx/spinctrl.h>
#include <set>

class NumberControl;

namespace TTAMachine {
    class Machine;
    class AddressSpace;
}

/**
 * Dialog for editing address space properties.
 */
class AddressSpaceDialog : public wxDialog {
public:
    AddressSpaceDialog(
        wxWindow* parent,
        TTAMachine::Machine* machine,
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
    
    void onAddId(wxCommandEvent& event);
    void onDeleteId(wxCommandEvent& event);
    void onSpinId(wxSpinEvent& event);
    void onIdListSelection(wxListEvent& event);
    void updateIdLists();
    bool isFreeId(unsigned id);

    void setTexts();

    /// Machine containing all the address spaces.
    TTAMachine::Machine* machine_;
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

    /// Boxsizer containing the address space id widget.
    wxStaticBoxSizer* idSizer_;
    /// List control containing the address space ids.
    wxListCtrl* idListCtrl_;
    /// SpinCtrl containing an address space id.
    wxSpinCtrl* idSpinCtrl_;

    /// Container for unique id numbers.
    std::set<unsigned> idNumbers_;

    // IDs for dialog controls
    enum {
        ID_NAME = 10000,
        ID_WIDTH,
        ID_MIN_ADDRESS,
        ID_MAX_ADDRESS,
        ID_BIT_WIDTH,

        ID_ID_LIST,
        ID_SPIN_ID,
        ID_ADD_ID,
        ID_DELETE_ID,
        
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
