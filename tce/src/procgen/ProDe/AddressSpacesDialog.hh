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
 * @file AddressSpacesDialog.hh
 *
 * Declaration of AddressSpacesDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel-no.spam-cs.tut.fi)
 */

#ifndef TTA_ADDRESS_SPACES_DIALOG_HH
#define TTA_ADDRESS_SPACES_DIALOG_HH

#include <wx/wx.h>
#include <wx/listctrl.h>

namespace TTAMachine {
    class Machine;
    class AddressSpace;
}

class wxListCtrl;

/**
 * Dialog for listing and editing address spaces of the machine.
 */
class AddressSpacesDialog : public wxDialog {
public:
    AddressSpacesDialog(wxWindow* parent, TTAMachine::Machine* machine);
    ~AddressSpacesDialog();

private:
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);
    TTAMachine::AddressSpace* selectedAS();
    void updateASList();
    void onAdd(wxCommandEvent& event);
    void onActivateAS(wxListEvent& event);
    void onEdit(wxCommandEvent& event);
    void onDelete(wxCommandEvent& event);
    void onASSelection(wxListEvent& event);
    void onASRightClick(wxListEvent& event);
    void setTexts();

    /// Machine containing the address spaces to list.
    TTAMachine::Machine* machine_;
    /// List control for listing the address spaces.
    wxListCtrl* asList_;

    // enumerated ids for dialog controls
    enum {
        ID_ADD = 10000,
        ID_EDIT,
        ID_HELP,
        ID_DELETE,
        ID_LIST,
        ID_LINE
    };

    DECLARE_EVENT_TABLE()
};
#endif
