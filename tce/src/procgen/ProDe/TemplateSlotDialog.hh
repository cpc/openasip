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
 * @file TemplateSlotDialog.hh
 *
 * Declaration of TemplateSlotDialog.
 *
 * @author Veli-Pekka Jääskeläinen 2004 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_TEMPLATE_SLOT_DIALOG_HH
#define TTA_TEMPLATE_SLOT_DIALOG_HH

#include <wx/wx.h>
#include <wx/spinctrl.h>

namespace TTAMachine {
    class InstructionTemplate;
    class TemplateSlot;
    class Bus;
    class ImmediateUnit;
}

/**
 * Dialog for editing telplate slot properties.
 */
class TemplateSlotDialog : public wxDialog {
public:
    TemplateSlotDialog(
        wxWindow* parent,
        TTAMachine::InstructionTemplate* it,
        TTAMachine::TemplateSlot* slot = NULL);

    virtual ~TemplateSlotDialog();

private:
    wxSizer* createContents(wxWindow *parent, bool call_fit, bool set_sizer);
    virtual bool TransferDataToWindow();
    void onOK(wxCommandEvent& event);
    void setTexts();

    /// Instruction template slot to edit.
    TTAMachine::TemplateSlot* slot_;
    /// Parent instruction template of the slot.
    TTAMachine::InstructionTemplate* it_;
    /// Choice control for the slot.
    wxChoice* slotChoice_;
    /// Choice control fot the destination IU.
    wxChoice* destinationChoice_;
    /// Spin control for the width.
    wxSpinCtrl* widthCtrl_;
    /// Bit width of the slot.
    int width_;
    /// Destination immediate unit.
    TTAMachine::ImmediateUnit* destination_;

    // enumerated IDs for dialog widgets
    enum {
        ID_LABEL_SLOT = 10000,
        ID_SLOT,
        ID_LABEL_WIDTH,
        ID_WIDTH,
        ID_LABEL_DESTINATION,
        ID_DESTINATION,
        ID_LINE,
        ID_HELP
    };

    DECLARE_EVENT_TABLE()
};
#endif
