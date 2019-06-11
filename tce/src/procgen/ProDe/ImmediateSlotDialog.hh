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
 * @file ImmediateSlotDialog.hh
 *
 * Declaration of ImmediateSlotDialog class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_IMMEDIATE_SLOT_DIALOG_HH
#define TTA_IMMEDIATE_SLOT_DIALOG_HH

#include <wx/wx.h>
#include <wx/listctrl.h>

namespace TTAMachine {
    class Machine;
}


/**
 * Dialog for modifying immediate slots in a machine.
 */
class ImmediateSlotDialog : public wxDialog {
public:
    ImmediateSlotDialog(wxWindow* parent, TTAMachine::Machine* machine);
    virtual ~ImmediateSlotDialog();

private:
    virtual bool TransferDataToWindow();
    wxSizer* createContents(wxWindow* parent, bool call_fit, bool set_sizer);
    void updateSlotList();
    void onAddSlot(wxCommandEvent& event);
    void onDeleteSlot(wxCommandEvent& event);
    void onSlotName(wxCommandEvent& event);
    void onSlotSelection(wxListEvent& event);
    void setTexts();

    /// Parent machine of the immediate slots.
    TTAMachine::Machine* machine_;
    /// Immediate slot list widget.
    wxListCtrl* slotList_;
    /// Text in the slot name widget.
    wxString slotName_;

    enum {
	ID_SLOT_LIST = 10000,
	ID_NAME,
	ID_LABEL_NAME,
	ID_LABE_NAME,
	ID_ADD_SLOT,
	ID_DELETE_SLOT,
	ID_LINE,
	ID_HELP
    };

    DECLARE_EVENT_TABLE()
};
#endif
